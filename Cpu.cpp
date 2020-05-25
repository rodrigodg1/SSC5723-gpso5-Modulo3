#include "stdio.h"

class Cpu
{
public:
    void executar(char *nomeProcesso, int instrucao);
};

void Cpu::executar(char *nomeProcesso, int instrucao)
{
    printf("\nProcesso %s: Executando instrução %d na CPU", nomeProcesso, instrucao);
}
