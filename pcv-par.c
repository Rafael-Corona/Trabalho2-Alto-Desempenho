/*Turma: A	Grupo 7

8504480	Guilherme Alves Lindo
11796893 	Luiz Fernando Rabelo
11031663 	Marcus Vinicius Medeiros Pará
4769989 	Rafael Corona
11795526 	Tulio Santana Ramos
*/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <omp.h>

// Definição de constantes e função auxiliar para retornar o valor mínimo
#define MAX_DIST 100
#define ROOT 0
#define min(a,b) (a) > (b) ? (b) : (a);

// Função auxiliar, para efetuar a cópia de um array "src" para "dest"
void copia_arr(int* dest, int* src, int n)
{
	for (int i = 0; i < n; i++)
	{
		dest[i] = src[i];
	}
}

// Função para geração das arestas do grafo
void funcao_geradora(int** graph, int n)
{
	srand(12345);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i != j)
				graph[i][j] = rand() % MAX_DIST + 1;
			else
			{
				graph[i][j] = INT_MAX/2;
			}
		}
	}
	return;
}

// Função auxiliar para criação do grafo, organizado como uma matriz
int** cria_grafo(int n) {
	int** graph = (int**)malloc(sizeof(int*) * n);
	for (int i = 0; i < n; i++)
	{
		graph[i] = (int*)malloc(sizeof(int) * n);
	}

	funcao_geradora(graph, n);

	return graph;
}

// Função para determinação do caminho percorrido
long caminho(int** graph, int* visitados, int* melhor_caminho, const int n, const int vertice, const int posicao_caminho, const int posicao_inicial)
{
	if (posicao_caminho+1 == n)
	{
		return graph[vertice][posicao_inicial];
	}

	long min_dist = LONG_MAX;
	int min_pos = -1;
	int* melhor_caminho_local = (int*)malloc(sizeof(int) * n);

	copia_arr(melhor_caminho_local, melhor_caminho, n);

	// Percorrer + verificação de nós já visitados
	for (int i = 0; i < n; i++)
	{
		if (!visitados[i] && i != vertice)
		{
			visitados[i] = 1;
			
			// Recursão para nova verificação e comparação para determinar menor distância percorrida
			long temp = caminho(graph, visitados, melhor_caminho_local, n, i, posicao_caminho + 1, posicao_inicial) + graph[vertice][i];
			if (temp < min_dist)
			{
				copia_arr(melhor_caminho, melhor_caminho_local, n);
				
				min_dist = temp;
				min_pos = i;
				melhor_caminho[posicao_caminho] = i;
			}

			visitados[i] = 0;
		}
	}

	free(melhor_caminho_local);

	if (min_pos == -1)
	{
		printf("Erro\n");
	}
	return min_dist;
}

// Função main do código paralelo
int main(int argc, char *argv[])
{
    int n, rank, n_proc;

	// Tratamento de erros para execução
	if (argc == 2)
		n = atoi(argv[1]);
	else {
		printf("Número de argumento inválidos. Uso correto: mpirun -np <n_proc> <path of executable> <n_vertices>");
		return 1;
	}

	// Craição do grafo
	int** graph = cria_grafo(n);
	long menor_caminho_dist;
	int* melhor_caminho;
	MPI_Status status;

	// Inicialização MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

	// Inicialização/Alocação de variável auxiliares
	int block_size = n/n_proc;
	long local_menor_caminho_dist = LONG_MAX;
	int vertice_inicial = rank*block_size;
	int vertice_final = vertice_inicial + block_size;
	long temp_menor_dist = LONG_MAX;

	int* local_visitados = (int*)malloc(sizeof(int) * n);
	int* local_melhor_caminho_tmp = (int*)malloc(sizeof(int) * n);
	int* local_melhor_caminho = (int*)malloc(sizeof(int) * n);
	memset(local_visitados, 0, n*sizeof(int));
	memset(local_melhor_caminho_tmp, 0, n*sizeof(int));
	memset(local_melhor_caminho, 0, n*sizeof(int));

	local_visitados[0] = 1;
	
	// Trecho para diretiva "time" -> para coleta do tempo de execução
	#ifdef TIME
	FILE *output_fp;
	double start, end;
	char filename[255] = "par";
	char nVertices[15], nProcessors[15];
	if (rank == ROOT){ //processo mestre inicia o cronômetro
		sprintf(nVertices, "%d",n);
		strcat(filename, "-n");
		strcat(filename, nVertices);    
		sprintf(nProcessors, "%d",n_proc);
		strcat(filename, "-np");
		strcat(filename, nProcessors);    
		strcat(filename, ".stats");
		output_fp = fopen(filename, "a");
		start = omp_get_wtime();	
	}
	#endif

	if (rank == n_proc - 1) {
		vertice_final = n;
	}

	// Percorrer grafo e determinar menor caminho
	for (int vertice = vertice_inicial; vertice < vertice_final; vertice++) {
		if (vertice != 0) {
			local_visitados[vertice] = 1;
			local_melhor_caminho_tmp[0] = vertice;

			temp_menor_dist = caminho(graph, local_visitados, local_melhor_caminho_tmp, n, vertice, 1, 0) + graph[0][vertice];
			if(temp_menor_dist < local_menor_caminho_dist) {
				copia_arr(local_melhor_caminho, local_melhor_caminho_tmp, n);
						
				local_menor_caminho_dist = temp_menor_dist;
				local_melhor_caminho[0] = vertice;
			}
			local_visitados[vertice] = 0;
		}
	}
	
	// Redução
	MPI_Reduce(&local_menor_caminho_dist, &menor_caminho_dist, 1, MPI_LONG, MPI_MIN, ROOT, MPI_COMM_WORLD);
	MPI_Bcast(&menor_caminho_dist, 1, MPI_LONG, ROOT, MPI_COMM_WORLD);

	if (menor_caminho_dist == local_menor_caminho_dist)
		MPI_Send(local_melhor_caminho, n, MPI_INT, ROOT, 0, MPI_COMM_WORLD);

	// Verificação para apenas 1 impressão, pelo root
	if (rank == ROOT) {
		melhor_caminho = (int*)malloc(sizeof(int) * n);
		MPI_Recv(melhor_caminho, n, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

		printf("%ld\n", menor_caminho_dist);
		printf("0 -> %d : %d\n", melhor_caminho[0], graph[0][melhor_caminho[0]]);
		for (int i = 1; i < n; i++)
		{
			printf("%d -> %d : %d\n", melhor_caminho[i-1], melhor_caminho[i],graph[melhor_caminho[i-1]][melhor_caminho[i]]);
		}

		free(melhor_caminho);
		#ifdef TIME
		end = omp_get_wtime();
		fprintf(output_fp,"%lf\n", end - start);
		fclose(output_fp);	
		#endif 
	}

	// Liberação de memória
	free(local_visitados);
	free(local_melhor_caminho_tmp);
	free(local_melhor_caminho);

	for (int i = 0; i < n; i++)
	{
		free(graph[i]);
	}
	free(graph);
	
	// Finalização MPI
	MPI_Finalize();
	
    return 0;
}
