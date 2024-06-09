
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define palavraTam 4 //bytes

typedef struct{
    unsigned int conjBit; //Bit de endereço do conjunto;
    unsigned int tagBit; //Bit de endereço da tag;
    unsigned int blocoBit; //Bit de endereço do bloco/celula/palavra 
    bool vBit; //Bit de validade;
    int *celulas; //Armazena os dados/palavras do bloco da memoria principal, portanto, seu tamanho depende da PalavrasPorBloco
}Linha; 

typedef struct{
    Linha *linhas; //Linhas do conjunto. O ponteiro é necessario para criar um vetor linhas dinamicamente (Em tempo de execução)
}Conj; 

typedef struct{
    Conj *conjunto; //Conjuntos da cache. O ponteiro é necessario para criar um vetor conjunto dinamicamente (Em tempo de execução)
    unsigned int mcTam; //Tamanho da cache
    unsigned int linhasPorConj; //Linhas por conj
    int hit; 
    int miss;
}Cache;

typedef struct{
    int *data;
}Bloco;

typedef struct{
    unsigned int palavrasPorBloco;
    unsigned int mpTam;
    unsigned int endereco;
    Bloco *bloco; //Vetor dinamico bloco, simula o bloco da memoria.Por exemplo: data[palavrasPorBloco = 4] é um bloco com 4 palavras/dados
}MemPrincipal;

typedef struct{ //Quantidade de bits dde cada campo do endereço
    unsigned int palavra_qntBit; 
    unsigned int bloco_qntBit;
    unsigned int conj_qntBit;
    unsigned int tag_qntBit;
    unsigned int end_qntBit;
}QntBit;

MemPrincipal* mpInit(unsigned int mpTam, unsigned int palavrasPorBloco){
    int i, j;
    MemPrincipal *memPrincipal = malloc(sizeof(MemPrincipal));
    if(!memPrincipal)
        return NULL;

    memPrincipal->palavrasPorBloco = palavrasPorBloco;
    memPrincipal->mpTam = mpTam;
    memPrincipal->endereco = 0;

    memPrincipal->bloco = malloc((mpTam/palavrasPorBloco)*sizeof(MemPrincipal));
    for(i = 0; i < (mpTam/palavrasPorBloco); i++){
        memPrincipal->bloco[i].data = malloc(palavrasPorBloco*sizeof(MemPrincipal));
        for(j = 0; j < palavrasPorBloco; j++){
            memPrincipal->bloco[i].data[j] = rand() % 100; 
        }
    }

    return memPrincipal;
}

Cache* cacheInit(unsigned int mcTam, unsigned int linhasPorConj){
        int i, j;
        Cache *cache = malloc(sizeof(Cache)); //Alocando memoria dinamicamente para cache;
        if(!cache)
            return NULL;

        unsigned int conjQnt = mcTam/linhasPorConj; //Quantidade de conjuntos;
        cache->conjunto = malloc(conjQnt*sizeof(Conj)); //Criando um vetor conjunto dinamicamente em função da quantidade de conjuntos;
        cache->mcTam = mcTam;
        cache->linhasPorConj = linhasPorConj;
        cache->hit = 0;
        cache->miss = 0; 
    

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

QntBit calcBit(Cache *cache, MemPrincipal *memPrincipal){
    QntBit qntBit;
     qntBit.bloco_qntBit = log2((memPrincipal->mpTam/palavraTam)/memPrincipal->palavrasPorBloco); // bloco_qntBit = s
     qntBit.conj_qntBit = log2((cache->mcTam/(4*cache->linhasPorConj))/cache->linhasPorConj); //conj_qntBit = d
     qntBit.tag_qntBit = qntBit.bloco_qntBit - qntBit.conj_qntBit; //tag_qntBit = s-d
     qntBit.palavra_qntBit = log2(palavraTam); //palavra_qntBit = w;
     qntBit.end_qntBit = log2(memPrincipal->mpTam/palavraTam);
    return  qntBit;
}

unsigned int TagBit(unsigned int endereco, QntBit qntBit){
    return endereco >> (qntBit.palavra_qntBit + qntBit.conj_qntBit);
}

unsigned int ConjBit(unsigned int endereco, QntBit qntBit){
    return (endereco >> qntBit.palavra_qntBit) & ((1 << qntBit.conj_qntBit) - 1);
}

unsigned int BlocoBit(unsigned int endereco, QntBit qntBit){ 
    return  endereco >> qntBit.palavra_qntBit;
}

unsigned int PalavraBit(unsigned int endereco, QntBit qntBit){ 
    return  endereco & ((1 << qntBit.palavra_qntBit) - 1);
}

int buscarLinha(unsigned int conjBit, unsigned int tagBit, Cache *cache){ //Percorre as linhas do conjunto até encontrar o endereço e retorna o indice da linha
    int i, j, k;
    while(cache->conjunto[conjBit].linhas[i].vBit){ //Enquanto o vBit for verdadeiro o laço vai percorrer as linhas e testar se a tagBit é igual a tag
       if(cache->conjunto[conjBit].linhas[i].tagBit == tagBit)
            return i;
       i++;
    }
    return -1; //miss - o endereço nao está na cache;   
}

