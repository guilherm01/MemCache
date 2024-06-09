#include "func.h"

int main(){
    Cache *cache;
        cache = cacheInit(32, 2);
        if(cache == NULL){
        printf("Erro ao iniciar Cache\n"); 
        return 0; 
    }

    MemPrincipal *memPrincipal;
        memPrincipal = mpInit(64, 4);
        if(memPrincipal == NULL){
        printf("Erro ao iniciar MP\n"); 
        return 0; 
    }
    
    QntBit qntBit;
        qntBit = calcBit(cache, memPrincipal);
            printf("Palavra: %d bits\n", qntBit.palavra_qntBit);
            printf("Bloco: %d bits\n", qntBit.bloco_qntBit);
            printf("Conj: %d bits\n", qntBit.conj_qntBit);
            printf("Tag: %d bits\n", qntBit.tag_qntBit);
            printf("Endereco: %d bits\n\n", qntBit.end_qntBit);
        unsigned int endereco = 14;

        unsigned int tagBit = TagBit(endereco);
            printf("tagBit: %d\n", tagBit);
        unsigned int conjBit = ConjBit(endereco); 
            printf("conjBit: %d\n", conjBit);  
        unsigned int blocoBit = BlocoBit(endereco); 
            printf("blocoBit: %d\n", blocoBit);  
        unsigned int palavraBit = PalavraBit(endereco); 
            printf("palavraBit: %d\n", palavraBit);     

    
    return 0;
}