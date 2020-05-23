#include "stdio.h"

class DispositivoES
{
public:
    void executar(char* nomeProcesso, int id);
    DispositivoES();
    ~DispositivoES();
};

DispositivoES::DispositivoES()
{

}

DispositivoES::~DispositivoES()
{
}

void DispositivoES::executar(char* nomeProcesso, int id)
{
    printf("\nProcesso %s: Instrução de I/O para o dispositivo %d", nomeProcesso, id);
}