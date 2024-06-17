#include "func.h"

void lerConfiguracoes(const char* nomeArquivo, unsigned int* mpTam, unsigned int* palavrasPorBloco, unsigned int* mcTam, unsigned int* linhasPorConj, unsigned int enderecos[], unsigned int* qtdEnderecos) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de configuracoes: %s\n", nomeArquivo);
        exit(1);
    }

    char linha[100];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (sscanf(linha, "Tamanho da MP: %u", mpTam)) continue;
        if (sscanf(linha, "Quantidade de palavras por bloco: %u", palavrasPorBloco)) continue;
        if (sscanf(linha, "Tamanho da Cache: %u", mcTam)) continue;
        if (sscanf(linha, "Quantidade de linhas por conjunto: %u", linhasPorConj)) continue;
        if (strncmp(linha, "Endereços:", 10) == 0) {
            char* token = strtok(linha + 10, ", ");
            *qtdEnderecos = 0;
            while (token != NULL) {
                enderecos[*qtdEnderecos] = atoi(token);
                token = strtok(NULL, ", ");
                (*qtdEnderecos)++;
            }
        }
    }

    fclose(arquivo);
}

int main() {
    unsigned int mpTam, palavrasPorBloco, mcTam, linhasPorConj, qtdEnderecos;
    unsigned int enderecos[100];

    char caminhoArquivo[100] = "config.txt"; // Caminho padrão
    printf("Digite o caminho do arquivo de configuracoes (ou pressione Enter para usar 'config.txt'): ");
    fgets(caminhoArquivo, sizeof(caminhoArquivo), stdin);
    
    size_t len = strlen(caminhoArquivo);
    if (len > 0 && caminhoArquivo[len-1] == '\n') {
        caminhoArquivo[len-1] = '\0';
    }

    if (strlen(caminhoArquivo) == 0) {
        strcpy(caminhoArquivo, "config.txt");
    }

    lerConfiguracoes(caminhoArquivo, &mpTam, &palavrasPorBloco, &mcTam, &linhasPorConj, enderecos, &qtdEnderecos);

    MemPrincipal *memPrincipal;
    memPrincipal = mpInit(mpTam, palavrasPorBloco);
    if(!memPrincipal){
        printf("Erro ao iniciar MP\n"); 
        return 0; 
    }

    Cache *cache;
    cache = cacheInit(mcTam, linhasPorConj, palavrasPorBloco);
    if(!cache){
        printf("Erro ao iniciar Cache\n"); 
        return 0; 
    }

    int escolha;
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
            case 1:
                printf("Quantidade de bits de cada campo do endereco: \n\n");
                printf("[{s} BLOCO (%u) | {w} PALAVRA (%u)] \n\n", qntBit.bloco_qntBit, qntBit.palavra_qntBit);
                printf("[TAG(%u) | {d} CONJUNTO (%u) | {w} PALAVRA(%u)] \n", qntBit.tag_qntBit, qntBit.conj_qntBit, qntBit.palavra_qntBit);
                break;
            case 2: {
                unsigned int endereco;
                printf("Digite o endereco: ");
                scanf("%u", &endereco);
                unsigned int tagBit = TagBit(endereco, qntBit);
                unsigned int blocoBit = BlocoBit(endereco, qntBit);
                unsigned int conjBit = ConjBit(endereco, qntBit);
                unsigned int palavraBit = PalavraBit(endereco, qntBit);

                printf("TAG [%u]\n BLOCO [%u]\n PALAVRA [%u] \n CONJUNTO [%u] \n", tagBit, blocoBit, palavraBit, conjBit);
                break;
            }
            case 3: {
                for (unsigned int i = 0; i < qtdEnderecos; i++) {
                    printf("Mapeando endereco %u:\n", enderecos[i]);
                    mapeamento(cache, memPrincipal, qntBit, enderecos[i]);
                }
                break;
            }
            case 4: 
                imprimirCache(cache, memPrincipal, qntBit, 0);
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


//Este código se encontra comentado pois é uma versão do programa estável que eu usei para dar rollback caso qualquer merda acontecesse.
//Qualquer coisa é só apagar ele antes de entregar o trabalho.

// #include "func.h"

// void imprimirCache(Cache *cache, MemPrincipal *memPrincipal, QntBit qntBit, unsigned int endereco){
//     unsigned int conjQnt = (cache->mcTam/(palavraTam*memPrincipal->palavrasPorBloco))/cache->linhasPorConj;

//     for(int conj = 0; conj < conjQnt; conj++){
//         printf("Conjunto [%d]\n", conj);
//         for(int line = 0; line < cache->linhasPorConj; line++){
//             printf("Line [%d]: V[%d] - AC[%d] - TAG[%d] - ", line, cache->conjunto[conj].linhas[line].vBit, cache->conjunto[conj].linhas[line].acesso, cache->conjunto[conj].linhas[line].tagBit);
//             for(int celula = 0; celula < memPrincipal->palavrasPorBloco; celula++){
//                 printf("%d|", cache->conjunto[conj].linhas[line].celulas[celula]);
//             }
//             printf("\n");
//         }
//     }
// }

// int main(){
//     MemPrincipal *memPrincipal;
//     memPrincipal = mpInit(64, 4);
//     if(!memPrincipal){
//         printf("Erro ao iniciar MP\n"); 
//         return 0; 
//     }

//     Cache *cache;
//     cache = cacheInit(64, 2, 4);
//     if(!cache){
//         printf("Erro ao iniciar Cache\n"); 
//         return 0; 
//     }

//     int escolha;
//     unsigned int endereco = 14;
//     QntBit qntBit; 
//     qntBit = calcBit(cache, memPrincipal);
    
//     do {
//         printf("\nMenu Principal:\n");
//         printf("1. qntBits do endereco\n");
//         printf("2. Posicao do endereco\n");
//         printf("3. Mapeamento\n");
//         printf("4. ImprimirCache\n");
//         printf("0. Sair\n");
//         printf("Escolha uma opção: \n");
//         scanf("%d", &escolha);

//         switch (escolha) {
//             case 1:
//                 printf("Quantidade de bits de cada campo do endereco: \n\n");
//                 printf("[{s} BLOCO (%u) | {w} PALAVRA (%u)] \n\n", qntBit.bloco_qntBit, qntBit.palavra_qntBit);
//                 printf("[TAG(%u) | {d} CONJUNTO (%u) | {w} PALAVRA(%u)] \n", qntBit.tag_qntBit, qntBit.conj_qntBit, qntBit.palavra_qntBit);
//                 break;
//             case 2: {
//                 unsigned int tagBit = TagBit(endereco, qntBit);
//                 unsigned int blocoBit = BlocoBit(endereco, qntBit);
//                 unsigned int conjBit = ConjBit(endereco, qntBit);
//                 unsigned int palavraBit = PalavraBit(endereco, qntBit);

//                 printf("TAG [%u]\n BLOCO [%u]\n PALAVRA [%u] \n CONJUNTO [%u] \n", tagBit, blocoBit, palavraBit, conjBit);
//                 break;
//             }
//             case 3:
//                 mapeamento(cache, memPrincipal, qntBit, endereco);
//                 break;
//             case 4: 
//                 imprimirCache(cache, memPrincipal, qntBit, endereco);
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
