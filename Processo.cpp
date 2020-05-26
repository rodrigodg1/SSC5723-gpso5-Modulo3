#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Processo
{
public:
    char *nome;
    int tamanho;
    Pagina *paginas;
    Processo();
    Processo(char *nomeProcesso, int tamanhoProcesso, int quantidadeDePaginas);
    ~Processo();
};

Processo::Processo()
{
}

Processo::Processo(char *nomeProcesso, int tamanhoProcesso, int quantidadeDePaginas)
{
    printf("\nCriando processo %s", nomeProcesso);
    this->tamanho = tamanhoProcesso;
    this->nome = (char *)malloc(2 * sizeof(char));
    this->paginas = (Pagina *)malloc(quantidadeDePaginas * sizeof(Pagina));
    strcpy(this->nome, nomeProcesso);
}

Processo::~Processo()
{
}