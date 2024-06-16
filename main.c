#include "func.h"

void imprimirCache(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){
    unsigned int conjQnt = (cache->mcTam/(palavraTam*memPrincipal->palavrasPorBloco))/cache->linhasPorConj;
    unsigned int linhaQnt = cache->mcTam/(palavraTam*memPrincipal->palavrasPorBloco);
    for(int conj = 0; conj < conjQnt; conj++){
        printf("Conjunto [%d]\n", conj);
        for(int line = 0; line < linhaQnt; line++){
            printf("Line [%d]: V[%d] - AC[%d] - TAG[%d] - ", line, cache->conjunto[conj].linhas[line].vBit, cache->conjunto[conj].linhas[line].acesso, cache->conjunto[conj].linhas[line].tagBit);
            for(int celula = 0; celula < memPrincipal->palavrasPorBloco; celula++){
                printf("%d|", cache->conjunto[conj].linhas[line].celulas[celula]);
            }
            printf("\n");
        }
    }
}

int main(){
    MemPrincipal *memPrincipal;
    memPrincipal = mpInit(64, 4);
    if(!memPrincipal){
        printf("Erro ao iniciar MP\n"); 
        return 0; 
    }

    Cache *cache;
    cache = cacheInit(64, 2, 4);
    if(!cache){
        printf("Erro ao iniciar Cache\n"); 
        return 0; 
    }

    int escolha;
    unsigned int endereco = 14;
    QntBit qntBit; 
    qntBit = calcBit(cache, memPrincipal);
    
    do {
        printf("\nMenu Principal:\n");
        printf("1. qntBits do endereco\n");
        printf("2. Posicao do endereco\n");
        printf("3. Mapeamento\n");
        printf("4. ImprimirCache\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: \n");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1: //Quantidade de bits do endereço
                printf("Quantidade de bits de cada campo do endereco: \n\n");
                printf("[{s} BLOCO (%u) | {w} PALAVRA (%u)] \n\n", qntBit.bloco_qntBit, qntBit.palavra_qntBit);
                printf("[TAG(%u) | {d} CONJUNTO (%u) | {w} PALAVRA(%u)] \n", qntBit.tag_qntBit, qntBit.conj_qntBit, qntBit.palavra_qntBit);
                break;
            case 2: { //Posição do endereço
                unsigned int tagBit = TagBit(endereco, qntBit);
                unsigned int blocoBit = BlocoBit(endereco, qntBit);
                unsigned int conjBit = ConjBit(endereco, qntBit);
                unsigned int palavraBit = PalavraBit(endereco, qntBit);

                printf("TAG [%u]\n BLOCO [%u]\n PALAVRA [%u] \n CONJUNTO [%u] \n", tagBit, blocoBit, PalavraBit, ConjBit);
                break;
            }
            case 3: //Mapeamento
                mapeamento(cache, memPrincipal, qntBit, endereco);
                break;
            case 4: //Imprimir cache
                imprimirCache(cache, memPrincipal, qntBit, endereco);
                break;
            case 0: //Sair do programa
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (escolha != 0);

    return 0;   
}
