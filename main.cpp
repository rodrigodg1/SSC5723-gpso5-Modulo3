
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <ctype.h>
#include "Cpu.cpp"
#include "DispositivoES.cpp"
#include "Pagina.cpp"
#include "Processo.cpp"

#define MAX_PROCESSES 100

/* Função usada para obter o tamanho de um processo a partir de uma linha do arquivo */
int obterTamanho(char *linha)
{
    char tamanhoStr[4];

    for (int i = 5; linha[i] != '\\'; i++)
    {
        if (isdigit(linha[i]))
        {
            tamanhoStr[i - 5] = linha[i];
        }
        else
        {
            break;
        }
    }

    return atoi(tamanhoStr);
}

/* Função usada para obter o endereço lógico de uma operação a partir de uma linha do arquivo */
int obterEndereco(char *linha)
{
    char enderecoStr[4];

    for (int i = 5; linha[i] != ')'; i++)
    {
        if (isdigit(linha[i]))
        {
            enderecoStr[i - 5] = linha[i];
        }
        else
        {
            break;
        }
    }

    return atoi(enderecoStr);
}

/* Função para executar o LRU e retornar o ID do quadro a ser ocupado (ou da vítima) */
int executarLru(Pagina *quadros, int quantidadeDePaginas)
{
    int menorValor = 0;
    int menorId = -1;

    for (int i = 0; i < quantidadeDePaginas; i++)
    {
        if (quadros[i].livre)
        {
            return i;
        }

        if (quadros[i].contador < menorValor)
        {
            menorValor = quadros[i].contador;
            menorId = i;
        }
    }

    quadros[menorId].livre = 1;
    return menorId;
}

/* Função usada para alocar as páginas de um processo */
void alocarPaginas(Processo processo, int tamanhoPagina, int quantidadeDePaginas, int tamanho)
{
    Pagina **paginas = new Pagina *[quantidadeDePaginas];

    for (int i = 0; i < quantidadeDePaginas; i++)
    {
        Pagina *pagina = new Pagina();
        pagina->contador = 0;
        pagina->livre = 1;
        pagina->tamanho = tamanhoPagina;

        paginas[i] = pagina;
    }

    processo.paginas = *paginas;
}

/* Função usada para alocar os quadros ocupados por um processo na memória principal */
int *alocarQuadros(Processo *processo, Pagina *quadros, int quantidadeDePaginas, int tamanho, int tamanhoPagina)
{
    int quantidadeAAlocar = tamanho / tamanhoPagina;
    int *idQuadros = (int *)calloc(quantidadeAAlocar, sizeof(int));

    for (int i = 0; i < quantidadeAAlocar; i++)
    {
        int idQuadro = executarLru(quadros, quantidadeDePaginas);
        processo->paginas[i].idQuadro = idQuadro;
        processo->paginas[i].livre = 0;
        quadros[idQuadro].nomeProcesso = processo->nome;
        quadros[idQuadro].livre = 0;
    }

    return idQuadros;
}

/* Função usada para atualizar os contadores do LRU */
void atualizarContadores(Pagina *quadros, int quantidadeDeQuadros, int quadroReferenciado)
{
    for (int i = 0; i < quantidadeDeQuadros; i++)
    {
        quadros[i].contador >> 1;

        if (quadroReferenciado == i)
        {
            quadros[i].contador = quadros[i].contador | 16;
        }
    }
}

/* Função usada para realizar a leitura ou escrita de um endereço */
int realizarLeituraOuEscrita(bool leitura, Pagina *quadros, Processo processo, int tamanhoPagina, int endereco, int quantidadeDePaginas)
{
    int idPagina = (int)((long)tamanhoPagina + (long)endereco - 1) / (long)endereco;

    if (processo.paginas[idPagina].idQuadro != -1 && strcmp(quadros[processo.paginas[idPagina].idQuadro].nomeProcesso, processo.nome) == 0)
    {
        if (leitura)
        {
            printf("Processo %s: Lendo endereço %d da página %d -> quadro %d", processo.nome, endereco, idPagina, processo.paginas[idPagina].idQuadro);
        }
        else
        {
            printf("Processo %s: Escrevendo no endereço %d da página %d -> quadro %d", processo.nome, endereco, idPagina, processo.paginas[idPagina].idQuadro);
        }
    }
    else
    {
        printf("Page fault");
        int idQuadro = executarLru(quadros, quantidadeDePaginas);
        processo.paginas[idPagina].idQuadro = idQuadro;

        printf("Processo %s: Swap in do quadro %d para a página %d", processo.nome, idQuadro, idPagina);

        if (leitura)
        {
            printf("Processo %s: Lendo endereço %d da página %d -> quadro %d", processo.nome, endereco, idPagina, idQuadro);
        }
        else
        {
            printf("Processo %s: Escrevendo no endereço %d da página %d -> quadro %d", processo.nome, endereco, idPagina, idQuadro);
        }
    }
}

/* Função usada para obter um processo de um array pelo seu nome */
Processo obterProcessoPeloNome(char *nome, Processo processos[])
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (strcmp(processos[i].nome, nome))
        {
            return processos[i];
        }
    }
}

int main(int argc, char *argv[])
{
    int tamanhoPagina = (int)strtol(argv[1], NULL, 10);
    int tamanhoEndLogico = (int)strtol(argv[2], NULL, 10);
    int tamanhoMemFisica = (int)strtol(argv[3], NULL, 10);
    int tamanhoMemSecundaria = (int)strtol(argv[4], NULL, 10);
    int quantidadeDePaginas = tamanhoEndLogico / tamanhoPagina;
    int quantidadeDeQuadros = tamanhoMemFisica / tamanhoPagina;

    if (tamanhoMemFisica % tamanhoPagina != 0)
    {
        printf("\nO tamanho da memória física deve ser múltiplo do tamanho das páginas");
        return -1;
    }

    Pagina **quadros = new Pagina *[quantidadeDeQuadros];

    // Inicialização dos quadros da memória principal
    for (int i = 0; i < quantidadeDeQuadros; i++)
    {
        Pagina *quadro = new Pagina();
        quadro->livre = 1;
        quadro->tamanho = tamanhoPagina;

        quadros[i] = quadro;
    }

    FILE *arq;

    if ((arq = fopen("entrada", "r")) == NULL)
    {
        printf("\nErro ao abrir o arquivo");
        return -1;
    }

    char *linha = NULL;
    size_t comprimento = 0;
    char delim[] = " ";

    DispositivoES *dispositivoES = new DispositivoES();
    Cpu *cpu = new Cpu();

    Processo *processos[MAX_PROCESSES];
    int numProcessos = 0;
    int noLinha = 0;

    while (getline(&linha, &comprimento, arq) != -1)
    {
        if (noLinha != 0)
        {
            char nome[2];
            strncpy(nome, linha, 2);

            switch (linha[3])
            {
            case 'C':
            {
                int tamanho;

                tamanho = obterTamanho(linha);

                processos[numProcessos] = new Processo(nome, tamanho);

                break;
            }
            case 'R':
            {
                break;
            }
            case 'W':
            {
                break;
            }
            case 'P':
            {
                cpu->executar(nome, (int)strtol(&linha[6], NULL, 10));
                break;
            }
            case 'I':
            {
                dispositivoES->executar(nome, (int)strtol(&linha[6], NULL, 10));
                break;
            }
            default:
            {
                break;
            }
            }
        }

        noLinha++;
    }

    fclose(arq);

    printf("\n\n");
}
