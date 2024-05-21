#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Definindo as constantes

#define MAX_CACHE 32 * 1024 //32KB
#define MAX_MP 256 * 1024 //256KB
#define TAM_PALAVRA 4 //4 Bytes
#define PALAVRA_MAX_BLOCO 8

typedef struct linhaCache
{
    int *palavras;
    int validacao; //Variável que vou usar para verificar se o bloco da cache é pode realmente ser utilizado.
    int tag;
    int contLfu;
};

typedef struct memoriaPrincipal
{
    int **blocos;
    int tamanhoBloco;
    int numeroBlocos;
};


typedef struct cache
{
    int numConjuntos;
    int linhasPorConjunto;
};

//To do
//Fazer o código de leitura do arquivo de entrada.
//Fazer o código para iniciar a memória principal e a cache.
//Fazer as funções de mapeamento e substituição.
//Interface gráfica que interage com o usuário.
