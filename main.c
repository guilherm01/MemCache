
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

typedef struct{
    unsigned int tagBit; //Bit de tag;
    unsigned int conjBit; //Bit de conjunto;
    unsigned int blocoBit; //Bit de bloco/celula/palavra 
    bool vBit; //Bit de validade;
}Linha; 

typedef struct{
    Linha *linhas; //Linhas do conjunto. O ponteiro é necessario para criar um vetor dinamicamente
}Conj; 

typedef struct{
    Conj *conjunto; //Conjuntos da cache. O ponteiro é necessario para criar um vetor dinamicamente
    unsigned int mcTam; //Tamanho da cache
    unsigned int linhasPorConj; //Linhas por conj
    int miss; //Endereco nao esta na cache
    int hit;  //Endereco esta na cache
}Cache;

Cache* cache_init(unsigned int mcTam, unsigned int linhasPorConj){
        int i, j;
        unsigned int conjQnt = mcTam/linhasPorConj; //Quantidade de conjuntos;
        Cache *cache = malloc(sizeof(Cache)); //Alocando memoria dinamicamente para cache;
        cache->conjunto = malloc(conjQnt*sizeof(Conj)); //Criando um vetor conjunto dinamicamente em função da quantidade de conjuntos;
        cache->mcTam = mcTam;
        cache->linhasPorConj = linhasPorConj;
        int miss = 0;
        int hit = 0;

        for(i = 0; i < conjQnt; i++){
            cache->conjunto[i].linhas = malloc(linhasPorConj*sizeof(Linha)); //Criando um vetor linha dinamicamente em função da quantidade de linhasPorConjunto;
            for(j = 0; j < linhasPorConj; j++){
                cache->conjunto[i].linhas[j].tagBit = 0;
                cache->conjunto[i].linhas[j].blocoBit = 0;
                cache->conjunto[i].linhas[j].conjBit = 0;
                cache->conjunto[i].linhas[j].vBit = false;
            }
        }   
        return cache; 
}

//Vamos manipular bits - deslocamento (<< && >>) - para encontrar os indices de cada endereco (tag, conjunto, bloco). 

int main(){


    return 0;
}