
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#define palavraTam 4 //bytes

typedef struct{
    unsigned int tagBit; //Bit de endereço da tag;
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
    int contLFU; //Fazer a contagem de quantas vezes houve substituição
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

    for(i = 0; i < (celulasQnt/palavrasPorBloco); i++){
        memPrincipal->bloco[i].data = malloc(palavrasPorBloco*sizeof(int));
        if(!memPrincipal->bloco[i].data){ 
            for(int l = 0; l < i; l++)
                free(memPrincipal->bloco[l].data); //Libera a memoria ja alocada para os dados/palavras
            free(memPrincipal->bloco); //Libera a memoria ja alocada para o bloco
            free(memPrincipal); //Libera a memoria ja alocada para a MP
        }
        for(j = 0; j < palavrasPorBloco; j++){
            memPrincipal->bloco[i].data[j] = (rand() % 100);
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
    cache->contLFU = 0;

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
     qntBit.palavra_qntBit = log2(memPrincipal->palavrasPorBloco); //palavra_qntBit = w;
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
    return -1; //Miss - o endereço nao está na cache;   
}

int conjuntoCheio(unsigned int conjBit, Cache*cache){ //Verifica se o conjunto está cheio
    int i = 0;
    while(cache->conjunto[conjBit].linhas[i].vBit && (i < cache->linhasPorConj))
        i++;
    
    if((i >= cache->linhasPorConj))
        return -1; //Conjunto cheio;
    return 0; 
}

void escreverCache(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){
    unsigned int conjBit = ConjBit(endereco, qntBit);
    unsigned int blocoBit = BlocoBit(endereco, qntBit);
    unsigned int tagBit = TagBit(endereco, qntBit);
    int i = 0; 
    while(cache->conjunto[conjBit].linhas[i].vBit && (i < cache->linhasPorConj))
        i++; //Indice da linha vazia mais proxima;
        
    for(int j = 0; j < memPrincipal->palavrasPorBloco; j++){
        cache->conjunto[conjBit].linhas[i].celulas[j] = memPrincipal->bloco[blocoBit].data[j];
    }
    cache->conjunto[conjBit].linhas[i].tagBit = tagBit;
    cache->conjunto[conjBit].linhas[i].vBit = true;
    printf("- Endereco[%d] o bloco é gravado na LINHA[%d]|CONJ[%d]\n", endereco, i, conjBit);
    
}

void substLFU(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){ //Faz a substituição pelo método LFU
    int indiceLFU = 0;
    unsigned int conjBit = ConjBit(endereco, qntBit);
    unsigned int blocoBit = BlocoBit(endereco, qntBit);
    int LFU = cache->conjunto[conjBit].linhas[0].acesso; // Inicializa com o acesso da primeira linha do primeiro conjunto
    int i;

    // Percorre todas as linhas do conjunto para encontrar a menos frequentemente usada (indice)
    for(i = 0; i < cache->linhasPorConj; i++){
        if(cache->conjunto[conjBit].linhas[i].acesso < LFU){
            LFU = cache->conjunto[conjBit].linhas[i].acesso;
            indiceLFU = i;
        }
    }
    //Substitui a linha LFU pelo novo bloco
    for(i = 0; i < memPrincipal->palavrasPorBloco; i++)
        cache->conjunto[conjBit].linhas[indiceLFU].celulas[i] = memPrincipal->bloco[blocoBit].data[i];
    cache->conjunto[conjBit].linhas[indiceLFU].tagBit = TagBit(endereco, qntBit);
    cache->conjunto[conjBit].linhas[indiceLFU].vBit = true;
    cache->conjunto[conjBit].linhas[indiceLFU].acesso = 0; 
    cache->contLFU++;
    printf("- Conjunto[%d] cheio! \n", conjBit);
    printf("- Endereco[%d] é substituido na LINHA[%d]|CONJ[%d]\n", endereco, indiceLFU, conjBit);
    
}

void imprimirCache(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit){
    unsigned int conjQnt = (cache->mcTam/(palavraTam*memPrincipal->palavrasPorBloco))/cache->linhasPorConj;

    for(int conj = 0; conj < conjQnt; conj++){
        printf("Conjunto [%d]\n", conj);
        for(int line = 0; line < cache->linhasPorConj; line++){
            printf("Line [%d]: V[%d] - ACESSOS[%d] - TAG[%d] - Bloco: ", line, cache->conjunto[conj].linhas[line].vBit, cache->conjunto[conj].linhas[line].acesso, cache->conjunto[conj].linhas[line].tagBit);
            for(int celula = 0; celula < memPrincipal->palavrasPorBloco; celula++){
                printf("%d|", cache->conjunto[conj].linhas[line].celulas[celula]);
            }
            printf("\n");
        }
    }
}

void imprimirMP(MemPrincipal *memPrincipal, QntBit qntBit){
    unsigned int blocoQnt = ((memPrincipal->mpTam/palavraTam)/memPrincipal->palavrasPorBloco);
    printf("\nMEMORIA PRINCIPAL \n");
    for(int bloco = 0; bloco < blocoQnt; bloco++){
        printf("Bloco [%d]: ", bloco);
        for(int celula = 0; celula < memPrincipal->palavrasPorBloco; celula++){
            printf("%d|", memPrincipal->bloco[bloco].data[celula]);
        }
        printf("\n");
    }
}

void mapeamento(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){
    unsigned int tagBit = TagBit(endereco, qntBit);
    unsigned int conjBit = ConjBit(endereco, qntBit);
    unsigned int blocoBit = BlocoBit(endereco, qntBit); 
    unsigned int palavraBit = PalavraBit(endereco, qntBit); 
    unsigned int linha = buscarLinha(conjBit, tagBit, cache); 
    /*printf("conjBit: %d\n", conjBit);
    printf("blocoBit: %d\n", blocoBit);
    printf("palavraBit: %d\n", palavraBit);
    printf("tagBit: %d\n", tagBit);
    printf("Linha: %d\n\n", linha);*/
    if(conjuntoCheio(conjBit, cache)){ //Verifica se o conjunto ta cheio para saber se vai substituir ou só escrever na linha mais proxima
        if(linha != -1){ //Testa se o endereço está na cache (linha != -1) ou nao
            if(cache->conjunto[conjBit].linhas[linha].celulas[palavraBit] == memPrincipal->bloco[blocoBit].data[palavraBit]){
                cache->hit++;
                printf("- Hit! Endereco[%d]|CONJ[%d]|LINHA[%d]|TAG[%d]| esta na cache\n", endereco, conjBit, linha, tagBit);

            }
            else{
                cache->miss++; //Miss! endereço não está na cache, então escreve usando LFU;
                printf("- Miss! Endereco[%d]|CONJ[%d]|TAG[%d]| nao esta na cache \n", endereco, conjBit, tagBit);
                substLFU(cache, memPrincipal, qntBit, endereco);
            }
        }
        else{
            cache->miss++;
            printf("- Miss! Endereco[%d]|CONJ[%d]|TAG[%d]| nao esta na cache \n", endereco, conjBit, tagBit);
            substLFU(cache, memPrincipal, qntBit, endereco);
        }
    }
    else{ //Conjunto não está cheio
        if(linha != -1){ //Testa se a tag do endereço está na cache (linha != -1) ou nao
            if(cache->conjunto[conjBit].linhas[linha].celulas[palavraBit] == memPrincipal->bloco[blocoBit].data[palavraBit]){ //Testa se o dado do endereço está na linha da cache
                cache->hit++;  //Hit! endereço está na cache
                printf("- Hit! Endereco[%d]|CONJ[%d]|LINHA[%d]|TAG[%d]| esta na cache\n", endereco, conjBit, linha, tagBit);
             }
            else{
                cache->miss++; //Miss! endereço não está na cache, então escreve na proxima linha
                printf("- Miss! Endereco[%d]|CONJ[%d]|TAG[%d]| nao esta na cache \n", endereco, conjBit, tagBit);
                escreverCache(cache, memPrincipal, qntBit, endereco);
            }
        }
        else{ //Endereço nao está na cache, portanto carrega o bloco na linha mais proxima
            cache->miss++;
            printf("- Miss! Endereco[%d]|CONJ[%d]|TAG[%d]| nao esta na cache \n", endereco, conjBit, tagBit);
            escreverCache(cache, memPrincipal, qntBit, endereco);
        }
    }
    printf("\n");
    imprimirCache(cache, memPrincipal, qntBit);
    printf("\n\n");
}


