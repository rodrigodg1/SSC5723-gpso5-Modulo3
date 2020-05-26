#include <stdio.h>

class Quadro
{
public:
    bool livre;
    int tamanho;
    char *nomeProcesso;
    void setNomeProcesso(char *nome);
    Quadro();
    ~Quadro();
};

void Quadro::setNomeProcesso(char *nome)
{
    strcpy(this->nomeProcesso, nome);
}

Quadro::Quadro()
{
    this->nomeProcesso = (char *)malloc(2 * sizeof(char));
}

Quadro::~Quadro()
{
}