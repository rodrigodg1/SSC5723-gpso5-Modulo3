
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>



/*
prox = ponteiro para o proximo bloco de memoria
livre = verifica se o bloco de mem. esta livre
tamanho = tamanho que foi alocado na memoria
endereco_memoria = inicio do end. de mem. da onde a mem. foi alocada.
*/

typedef struct _BLOCO
{
    struct _BLOCO *prox;
    bool livre;
    size_t tamanho;
    void *endereco_memoria;
} BLOCO;



// tamanho da estrutura
#define TAM_BLOCO sizeof(BLOCO)

// recebe o tamanho que precisa alocar e adiciona um novo no na lista
// primeiro bloco
BLOCO *alocaBlocoMemoria(size_t tamanho)
{

    BLOCO *bloco = (BLOCO *)sbrk(0);

    //recebe o endereco de memoria
    void *endmem = (void *)sbrk(0);
    //altera o local de parada do prog. aumentando o tamanho do bloco e quanta memoria necessita ser alocada
    void *aloca_mem = (void *)sbrk(TAM_BLOCO + tamanho);

    //verifica se existe erros
    if (aloca_mem == (void *)-1)
    {
        return NULL;
        //retorna o bloco alocado
    }
    else
    {
        //cria o bloco
        bloco->prox = NULL;
        bloco->livre = false;
        bloco->tamanho = tamanho;
        bloco->endereco_memoria = endmem + TAM_BLOCO;
        return bloco;
    }
}

//insere na sequencia da lista encadeada
void alocaProxBlocoMemoria(size_t tamanho, BLOCO **primeiro)
{
    BLOCO *atual = *primeiro;
    void *aloca_mem = NULL;
    void *endmem = (void *)sbrk(0);
    //verifica se a lista esta vazia, se estiver aloca o bloco com o seu respectivo tamanho
    if (atual == NULL)
    {
        *primeiro = alocaBlocoMemoria(tamanho);
    }
    else
    {
        //se nao, vai ate o final da lista
        while (atual->prox != NULL)
        {
            atual = atual->prox;
        }
        // chegou no final, cria um novo bloco
        BLOCO *novoBloco = sbrk(0);

        //aloca esse bloco com respectivo tamanho
        aloca_mem = (void *)sbrk(TAM_BLOCO + tamanho);

        if (aloca_mem == (void *)-1) {}

        // adiciona no fim da lista
        else
        {
            novoBloco->prox = NULL;
            novoBloco->livre = false;
            novoBloco->tamanho = tamanho;
            novoBloco->endereco_memoria = endmem + TAM_BLOCO;
            //adiciona o novo bloco na lista
            atual->prox = novoBloco;
        }
    }
}


void liberaFrame(BLOCO **primeiro)
{
    if (*primeiro == NULL)
    {
    }
    else
    {
        (*primeiro)->livre = true;
    }
}

// mostra o encadeamento dos blocos na MEM Principal
void mostraBlocos(BLOCO *atual)
{

    //equanto existir blocos na lista
    while (atual != NULL)
    {
        printf("LIVRE = %d , TAMANHO = %d , END. DE MEM = %p , END. DO BLOCO ATUAL = %p , END. PROX BLOCO = %p \n",
        atual->livre,atual->tamanho,atual->endereco_memoria,atual,atual->prox);

        atual = atual->prox;
    }
    
}

int main(){


    // bloco de memoria
    BLOCO *bloco = NULL;

    // aloca um bloco com tamanho 10
    alocaProxBlocoMemoria(10,&bloco);

    // aloca um bloco com tamanho 40
    alocaProxBlocoMemoria(40,&bloco);

    //aloca uma bloco com tamanho 60
    alocaProxBlocoMemoria(60,&bloco);


    mostraBlocos(bloco);

    printf("\nDepois de Liberar o Segundo BLOCO\n");
    blocoMemLivre(&(bloco->prox));
    mostraBlocos(bloco);


}
