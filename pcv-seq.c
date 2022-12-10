/*Turma: A	Grupo 7

8504480	Guilherme Alves Lindo
11796893 	Luiz Fernando Rabelo
11031663 	Marcus Vinicius Medeiros Pará
4769989 	Rafael Corona
11795526 	Tulio Santana Ramos
*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>

// Definição de constantes
#define MAX_DIST 100

// Função auxiliar, para efetuar a cópia de um array "src" para "dest"
void copia_arr(int* arr1, int* arr2, int n)
{
	for (int i = 0; i < n; i++)
	{
		arr1[i] = arr2[i];
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

// Função para determinação do caminho percorrido
long caminho(int ** graph, int* visitados, int* melhor_caminho, const int n, const int pos, const int posicao_caminho, const int posicao_inicial)
{
	if (posicao_caminho+1 == n)
	{
		return graph[pos][posicao_inicial];
	}

	long min_dist = LONG_MAX;
	int min_pos = -1;
	int* melhor_caminho_local = (int*)malloc(sizeof(int) * n);

	copia_arr(melhor_caminho_local, melhor_caminho, n);

	// Percorrer + verificação de nós já visitados
	for (int i = 0; i < n; i++)
	{
		if (visitados[i] == 0 && i != pos)
		{
			visitados[i] = 1;

			// Recursão para nova verificação e comparação para determinar menor distância percorrida			
			long temp = caminho(graph, visitados, melhor_caminho_local, n, i, posicao_caminho + 1, posicao_inicial) + graph[pos][i];
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

// Função main do código sequencial
int main(int argc, char** argv)
{
	int n;
	srand(12345);

	// Tratamento de erros para execução
	if (argc == 2)
		n = atoi(argv[1]);
	else {
		printf("Número de argumento inválidos. Uso correto: <path_to_executable> <n_vertices>");
		return 1;
	}

	// Alocação do grafo
	int** graph = (int**)malloc(sizeof(int*) * n);
	for (int i = 0; i < n; i++)
	{
		graph[i] = (int*)malloc(sizeof(int) * n);
	}

	// Atribuição de valores para o grafo
	funcao_geradora(graph, n);

	// Trecho para diretiva "time" -> para coleta do tempo de execução
    #ifdef TIME
	char filename[255] = "seq";
	char runs[10], nVertices[15];
	sprintf(nVertices, "%d",n);
	strcat(filename, runs);
	strcat(filename, "-n");
	strcat(filename, nVertices);    
	strcat(filename, ".stats");
	FILE *output_fp = fopen(filename, "a");
        
	double start, end;
        start = omp_get_wtime();	
	#endif

	// Alocação e atribuição de valores de variáveis auxiliares
	int* visitados = (int*)malloc(sizeof(int) * n);
	int* melhor_caminho = (int*)malloc(sizeof(int) * n);

	for (int i = 0; i < n; i++)
	{
		visitados[i] = 0;
		melhor_caminho[i] = 0;
	}

	// Impressão dos resultados
	visitados[0] = 1;
	printf("%ld\n", caminho(graph, visitados, melhor_caminho, n, 0, 0, 0));

	printf("0 -> %d : %d\n", melhor_caminho[0], graph[0][melhor_caminho[0]]);
	for (int i = 1; i < n; i++)
	{
		printf("%d -> %d : %d\n", melhor_caminho[i-1], melhor_caminho[i],graph[melhor_caminho[i-1]][melhor_caminho[i]]);
	}

	// Trecho para diretiva "time" -> para coleta do tempo de execução
	#ifdef TIME
	end = omp_get_wtime();
	fprintf(output_fp,"%lf\n", end - start);
	fclose(output_fp);
	#endif

	// Liberação de memória
	for (int i = 0; i < n; i++)
	{
		free(graph[i]);
	}
	free(graph);

	return 0;
}
