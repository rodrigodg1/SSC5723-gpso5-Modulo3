#include <stdio.h>

class Processo
{
public:
    char* nome;
    size_t tamanho;
    Pagina* paginas;
    Processo();
    Processo(char* nomeProcesso, size_t tamanhoProcesso);
    ~Processo();
};

Processo::Processo()
{
    
}

Processo::Processo(char* nomeProcesso, size_t tamanhoProcesso)
{
    printf("\nCriando processo %s", nomeProcesso);
    this->tamanho = tamanhoProcesso; 
    this->nome = nome;
}

Processo::~Processo()
{

}