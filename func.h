
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
    int acesso; //Fazer a contagem de acesso de cada linha para substituir por LFU
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
    unsigned int celulasQnt = mpTam/palavraTam; //Quantidade de celulas/palavras na memoria principal

    memPrincipal->bloco = malloc((celulasQnt/palavrasPorBloco)*sizeof(Bloco));
    if(!memPrincipal->bloco){
        free(memPrincipal);
        return NULL;
    }

    for(i = 0; i < (mpTam/palavrasPorBloco); i++){
        memPrincipal->bloco[i].data = malloc(palavrasPorBloco*sizeof(int));
        if(!memPrincipal->bloco[i].data){ 
            for(int l = 0; l < i; l++)
                free(memPrincipal->bloco[l].data); //Libera a memoria ja alocada para os dados/palavras
            free(memPrincipal->bloco); //Libera a memoria ja alocada para o bloco
            free(memPrincipal); //Libera a memoria ja alocada para a MP
        }
        for(j = 0; j < palavrasPorBloco; j++){
            memPrincipal->bloco[i].data[j] = rand() % 100; 
        }
    }

    return memPrincipal;
}

Cache* cacheInit(unsigned int mcTam, unsigned int linhasPorConj, unsigned int palavraPorBloco) {
    int i, j, k;
    Cache *cache = malloc(sizeof(Cache)); //Alocando memória dinamicamente para cache
    if (!cache)
        return NULL;
    
    unsigned int linhasQnt = mcTam / (palavraTam * palavraPorBloco); //Quantidade de linhas totais na cache
    cache->conjunto = malloc((linhasQnt / linhasPorConj) * sizeof(Conj)); //Criando um vetor conjunto dinamicamente em função da quantidade de conjuntos
    if (!cache->conjunto) {
        free(cache); //Libera a memória alocada para cache 
        return NULL;
    }

    cache->mcTam = mcTam;
    cache->linhasPorConj = linhasPorConj;
    cache->hit = 0;
    cache->miss = 0;

    for (i = 0; i < (linhasQnt / linhasPorConj); i++) {
        cache->conjunto[i].linhas = malloc(linhasPorConj * sizeof(Linha)); //Criando um vetor linha dinamicamente em função da quantidade de linhasPorConjunto
        if (!cache->conjunto[i].linhas) {
            for (int l = 0; l < i; l++){
                free(cache->conjunto[l].linhas); //Libera as linhas já alocadas
            }
            free(cache->conjunto); //Libera o conjunto já alocado
            free(cache); //Libera a estrutura cache
            return NULL;
        }

        for (j = 0; j < linhasPorConj; j++) {
            cache->conjunto[i].linhas[j].tagBit = 0;
            cache->conjunto[i].linhas[j].blocoBit = 0;
            cache->conjunto[i].linhas[j].conjBit = 0;
            cache->conjunto[i].linhas[j].vBit = false;
            cache->conjunto[i].linhas[j].acesso = 0;

            cache->conjunto[i].linhas[j].celulas = malloc(palavraPorBloco * sizeof(int)); //Criando um vetor celula para armazenar o bloco da mp
            if (!cache->conjunto[i].linhas[j].celulas) {
                for (int l = 0; l <= i; l++) {
                    for (int m = 0; m < j; m++) {
                        free(cache->conjunto[l].linhas[m].celulas); //Libera as células já alocadas
                    }
                    free(cache->conjunto[l].linhas); //Libera as linhas já alocadas
                }
                free(cache->conjunto); //Libera o conjunto já alocado
                free(cache); //Libera a estrutura cache
                return NULL;
            }

            for (k = 0; k < palavraPorBloco; k++){
                cache->conjunto[i].linhas[j].celulas[k] = 0;
            }
        }
    }

    return cache;
}


QntBit calcBit(Cache *cache, MemPrincipal *memPrincipal){
    QntBit qntBit;
     qntBit.bloco_qntBit = log2((memPrincipal->mpTam/palavraTam)/memPrincipal->palavrasPorBloco); // bloco_qntBit = s
     qntBit.conj_qntBit = log2((cache->mcTam/(palavraTam*memPrincipal->palavrasPorBloco))/cache->linhasPorConj); //conj_qntBit = d
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
    int i = 0;
    while(cache->conjunto[conjBit].linhas[i].vBit && (i < cache->linhasPorConj)){ //Enquanto o vBit for verdadeiro o laço vai percorrer as linhas e testar se a tagBit é igual a tag
        if(cache->conjunto[conjBit].linhas[i].tagBit == tagBit){
            cache->conjunto[conjBit].linhas[i].acesso++;
            return i; //Hit - o endereço está na cache
        }
        cache->conjunto[conjBit].linhas[i].acesso++;
        i++;
    }
    cache->conjunto[conjBit].linhas[i].acesso++;
    return -1; //Miss - o endereço nao está na cache;   
}

int conjuntoCheio(unsigned int conjBit, Cache*cache){ //Verifica se o conjunto está cheio
    int i = 0;
    while(cache->conjunto[conjBit].linhas[i].vBit && (i < cache->linhasPorConj))
        i++;

    if(cache->conjunto[conjBit].linhas[i].vBit == true)
        return -1; //Conjunto cheio;
    return 0; 
}

int buscarEndereco(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){ //Percorre o bloco dentro da linha pra verficar se tem o dado do endereco da MP
    unsigned int conjBit = ConjBit(endereco, qntBit);
    unsigned int blocoBit = BlocoBit(endereco, qntBit);
    unsigned int palavraBit = PalavraBit(endereco, qntBit);
    unsigned int tagBit = TagBit(endereco, qntBit);
    unsigned int linha = buscarLinha(conjBit, tagBit, cache);

    for(int i = 0; i < memPrincipal->palavrasPorBloco; i++){
        if(cache->conjunto[conjBit].linhas[linha].celulas[i] == memPrincipal->bloco[blocoBit].data[palavraBit])
            return -1; //Endereco está na cache;
        return 0; //Endereco nao está na cache
    }
}

void escreverCache(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){
    unsigned int conjBit = ConjBit(endereco, qntBit) - 1;
    unsigned int blocoBit = BlocoBit(endereco, qntBit) - 1;
    unsigned int tagBit = TagBit(endereco, qntBit);
    unsigned int linha = buscarLinha(conjBit, tagBit, cache);

    for(int i = 0; i < memPrincipal->palavrasPorBloco; i++)
        cache->conjunto[conjBit].linhas[linha].celulas[i] = memPrincipal->bloco[blocoBit].data[i];
    cache->conjunto[conjBit].linhas[linha].vBit = true;
    
}

void substLFU(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){ //Faz a substituição pelo método LFU
    int LFU = cache->conjunto[0].linhas[0].acesso; // Inicializa com o acesso da primeira linha do primeiro conjunto
    int indiceLFU = 0;
    unsigned int conjBit = ConjBit(endereco, qntBit) - 1;
    unsigned int blocoBit = BlocoBit(endereco, qntBit) - 1;

    // Percorre todas as linhas do conjunto para encontrar a menos frequentemente usada (indice)
    for(int i = 0; i < cache->linhasPorConj; i++){
        if(cache->conjunto[conjBit].linhas[i].acesso < LFU){
            LFU = cache->conjunto[conjBit].linhas[i].acesso;
            indiceLFU = i;
        }
    }
    //Substitui a linha LFU pelo novo bloco
    for(int i = 0; i < memPrincipal->palavrasPorBloco; i++)
        cache->conjunto[conjBit].linhas[indiceLFU].celulas[i] = memPrincipal->bloco[blocoBit].data[i];
    cache->conjunto[conjBit].linhas[indiceLFU].vBit = true;
    
}

void mapeamento(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){
    unsigned int tagBit = TagBit(endereco, qntBit);
    unsigned int conjBit = ConjBit(endereco, qntBit) - 1;
    unsigned int blocoBit = BlocoBit(endereco, qntBit) - 1; 
    unsigned int palavraBit = PalavraBit(endereco, qntBit) - 1; 
    unsigned int linha = buscarLinha(conjBit, tagBit, cache);

    if(conjuntoCheio(conjBit, cache)){
        if(cache->conjunto[conjBit].linhas[linha].celulas[palavraBit] == memPrincipal->bloco[blocoBit].data[palavraBit]){
            cache->hit++;
            printf("Hit!\n");
            //Hit! endereço está na cache
        }
        else{
            cache->miss++; //Miss! endereço não está na cache, então escreve usando LFU;
            substLFU(cache, memPrincipal, qntBit, endereco);
        }
    }
    else{ //Conjunto não está cheio
        if(cache->conjunto[conjBit].linhas[linha].celulas[palavraBit] == memPrincipal->bloco[blocoBit].data[palavraBit]){
            cache->hit++;  //Hit! endereço está na cache

        }
        else{
            cache->miss++; //Miss! endereço não está na cache, então escreve na proxima linha
            escreverCache(cache, memPrincipal, qntBit, endereco);
        }
    }
}
    