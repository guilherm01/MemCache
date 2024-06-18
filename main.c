#include "func.h"

int main() {
    unsigned int mpTam, palavrasPorBloco, mcTam, linhasPorConj;
    unsigned int endereco[5] = {0};
    unsigned int numEnderecos = 0;

    // Lê o arquivo de configuração
    lerConfig("config.txt", &mpTam, &palavrasPorBloco, &mcTam, &linhasPorConj, endereco, &numEnderecos);

    MemPrincipal *memPrincipal = mpInit(mpTam, palavrasPorBloco);
    if (!memPrincipal) {
        printf("Erro ao iniciar MP\n");
        return 0;
    }

    Cache *cache = cacheInit(mcTam, linhasPorConj, palavrasPorBloco);
    if (!cache) {
        printf("Erro ao iniciar Cache\n");
        return 0;
    }

    QntBit qntBit = calcBit(cache, memPrincipal);
    int escolha;

    for (unsigned int i = 0; i < numEnderecos; i++) {
        if (endereco[i] > (pow(2, qntBit.end_qntBit) - 1)) {
            printf("Endereco nao existe na Memoria Principal\n");
            exit(1);
        }
    }

    do {
        printf("\nMenu Principal:\n");
        printf("1. Bits para o enderecamento\n");
        printf("2. Posicao do endereco\n");
        printf("3. Mapeamento\n");
        printf("4. ImprimirCache\n");
        printf("5. ImprimirMP\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: \n");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1:
                printf("Quantidade de bits para o endereco: %d\n", qntBit.end_qntBit);
                printf("[{s} BLOCO (%u) | {w} PALAVRA (%u)] \n\n", qntBit.bloco_qntBit, qntBit.palavra_qntBit);
                printf("[TAG(%u) | {d} CONJUNTO (%u) | {w} PALAVRA(%u)] \n", qntBit.tag_qntBit, qntBit.conj_qntBit, qntBit.palavra_qntBit);
                break;
            case 2: {
                unsigned int tagBit, blocoBit, conjBit, palavraBit;
                for (unsigned int i = 0; i < numEnderecos; i++) {
                    tagBit = TagBit(endereco[i], qntBit);
                    blocoBit = BlocoBit(endereco[i], qntBit);
                    conjBit = ConjBit(endereco[i], qntBit);
                    palavraBit = PalavraBit(endereco[i], qntBit);
                    printf("Endereco[%d]\n", endereco[i]);
                    printf("TAG [%u]\n BLOCO [%u]\n PALAVRA [%u] \n CONJUNTO [%u] \n\n", tagBit, blocoBit, palavraBit, conjBit);
                }
                break;
            }
            case 3:
                for (unsigned int i = 0; i < numEnderecos; i++) {
                    mapeamento(cache, memPrincipal, qntBit, endereco[i]);
                }
                printf("\nFalhas[%d]\nAcertos[%d]\nSubstituicoes[%d]\n", cache->miss, cache->hit, cache->contLFU);
                break;
            case 4: 
                imprimirCache(cache, memPrincipal, qntBit);
                break;
            case 5:
                imprimirMP(memPrincipal, qntBit);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (escolha != 0);

    return 0;   
}

// #include "func.h"

// int main(){

//     MemPrincipal *memPrincipal;
//     memPrincipal = mpInit(64, 2);
//     if(!memPrincipal){
//         printf("Erro ao iniciar MP\n"); 
//         return 0; 
//     }
    

//     Cache *cache;
//     cache = cacheInit(32, 2, 2);
//     if(!cache){
//         printf("Erro ao iniciar Cache\n"); 
//         return 0; 
//     }
//     unsigned int endereco[5];
//     QntBit qntBit; 
//     qntBit = calcBit(cache, memPrincipal);
//     int escolha;
//     for(int i = 0; i < 5; i++){
//         printf("Informe o endereco [%d]\n", i);
//         scanf("%d", &endereco[i]);
//         if(endereco[i] > (pow(2, qntBit.end_qntBit) - 1)){
//             printf("Endereco nao existe nao Memoria Principal\n");
//             exit(1);
//         }
//     }
    
//     do {
//         printf("\nMenu Principal:\n");
//         printf("1. Bits para o enderecamento\n");
//         printf("2. Posicao do endereco\n");
//         printf("3. Mapeamento\n");
//         printf("4. ImprimirCache\n");
//         printf("5. ImprimirMP\n");
//         printf("0. Sair\n");
//         printf("Escolha uma opção: \n");
//         scanf("%d", &escolha);

//         switch (escolha) {
//             case 1:
//                 printf("Quantidade de bits para o endereco: %d\n", qntBit.end_qntBit);
//                 printf("[{s} BLOCO (%u) | {w} PALAVRA (%u)] \n\n", qntBit.bloco_qntBit, qntBit.palavra_qntBit);
//                 printf("[TAG(%u) | {d} CONJUNTO (%u) | {w} PALAVRA(%u)] \n", qntBit.tag_qntBit, qntBit.conj_qntBit, qntBit.palavra_qntBit);
//                 break;
//             case 2: {
//                 unsigned int tagBit;
//                 unsigned int blocoBit;
//                 unsigned int conjBit;
//                 unsigned int palavraBit;
//                 for(int i = 0; i < 5; i++){
//                     tagBit = TagBit(endereco[i], qntBit);
//                     blocoBit = BlocoBit(endereco[i], qntBit);
//                     conjBit = ConjBit(endereco[i], qntBit);
//                     palavraBit = PalavraBit(endereco[i], qntBit);
//                     printf("Endereco[%d]\n", endereco[i]);
//                     printf("TAG [%u]\n BLOCO [%u]\n PALAVRA [%u] \n CONJUNTO [%u] \n\n", tagBit, blocoBit, palavraBit, conjBit);
                    
//                 }
//                 break;
//             }
//             case 3:
//                 for(int i = 0; i < 5; i++){
//                     mapeamento(cache, memPrincipal, qntBit, endereco[i]);
//                 }
//                 printf("\nFalhas[%d]\nAcertos[%d]\nSubstituicoes[%d]\n", cache->miss, cache->hit, cache->contLFU);
//                 break;
//             case 4: 
//                 imprimirCache(cache, memPrincipal, qntBit);
//                 break;
//             case 5:
//                 imprimirMP(memPrincipal, qntBit);
//                 break;
//             case 0:
//                 printf("Saindo...\n");
//                 break;
//             default:
//                 printf("Opção inválida. Tente novamente.\n");
//         }
//     } while (escolha != 0);

//     return 0;   
// }
