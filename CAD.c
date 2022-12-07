// CAD.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#define MAX_DIST 25

void copia_arr(int* arr1, int* arr2, int n)
{
	for (int i = 0; i < n; i++)
	{
		arr1[i] = arr2[i];
	}
}

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
				//graph[i][j] /= 2;
			}
		}
	}
	return;
}

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

	for (int i = 0; i < n; i++)
	{
		if (visitados[i] == 0 && i != pos)
		{
			visitados[i] = 1;
			
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

int main(int argc, char** argv)
{
	int n;
	srand(12345);

	if (argc == 2)
		n = atoi(argv[1]);
	else {
		printf("Número de argumento inválidos. Uso correto: mpirun -np <n_proc> <path of executable> <n_vertices>");
		return 1;
	}
	
	int** graph = (int**)malloc(sizeof(int*) * n);
	for (int i = 0; i < n; i++)
	{
		graph[i] = (int*)malloc(sizeof(int) * n);
	}

	funcao_geradora(graph, n);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			printf("%d\t",graph[i][j]);
		}
		printf("\n");
	}

	time_t t_0 = time(NULL);

	int* visitados = (int*)malloc(sizeof(int) * n);
	int* melhor_caminho = (int*)malloc(sizeof(int) * n);

	for (int i = 0; i < n; i++)
	{
		visitados[i] = 0;
		melhor_caminho[i] = 0;
	}

	visitados[0] = 1;
	printf("%ld\n", caminho(graph, visitados, melhor_caminho, n, 0, 0, 0));

	printf("0 -> %d : %d\n", melhor_caminho[0], graph[0][melhor_caminho[0]]);
	for (int i = 1; i < n; i++)
	{
		printf("%d -> %d : %d\n", melhor_caminho[i-1], melhor_caminho[i],graph[melhor_caminho[i-1]][melhor_caminho[i]]);
	}

	#ifdef TIME
	printf("%lds\n", time(NULL) - t_0);
	#endif

	for (int i = 0; i < n; i++)
	{
		free(graph[i]);
	}
	free(graph);

	return 0;
}

