
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <ctype.h>
#include <unistd.h>
#include "Cpu.cpp"
#include "DispositivoES.cpp"
#include "Pagina.cpp"
#include "Processo.cpp"
#include "Quadro.cpp"

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

    for (int i = 6; linha[i] != ')'; i++)
    {
        if (isdigit(linha[i]))
        {
            enderecoStr[i - 6] = linha[i];
        }
        else
        {
            break;
        }
    }

    return atoi(enderecoStr);
}

/* Função para executar o LRU e retornar o ID do quadro a ser ocupado (ou da vítima) */
int executarLru(
    Quadro **quadros,
    int quantidadeDeQuadros,
    int *chamadas,
    int numChamadas)
{
    // Se houver quadro livre, não precisa realizar swap
    for (int i = 0; i < quantidadeDeQuadros; i++)
    {
        if (quadros[i]->livre)
        {
            return i;
        }
    }

    int contador = 0;

    // Se não houver, procura pelas chamadas pela mais antiga que ainda não sofreu swap
    for (int i = numChamadas; i >= 0; i--)
    {
        bool temNovaChamada = 0;

        // Procura se chamada ao quadro foi repetida posteriormente (a partir da segunda iteração)
        if (i != numChamadas) {
            for (int j = i + 1; j < numChamadas; j++)
            {
                if (chamadas[i] == chamadas[j])
                {
                    temNovaChamada = 1;
                    break;
                }
            }
        }

        // Se o quadro não foi chamado após, incrementa o contador (quadro em uso)
        if (!temNovaChamada) {
            contador++;

            // Se o contador for igual à quantidade de quadros, esse é o quadro a ser removido
            if (contador > quantidadeDeQuadros) {
                quadros[chamadas[i]]->livre = 1;
                return chamadas[i];
            }
        }
    }
}

// Função para atualizar a lista de chamadas aos quadros com uma nova
int *atualizaChamadas(
    int idQuadro, 
    int numChamadas, 
    int *chamadas)
{
    int *novasChamadas;

    if (numChamadas == 0)
    {
        novasChamadas = (int *)malloc(sizeof(int));
    }
    else
    {
        novasChamadas = (int *)realloc(chamadas, sizeof(int) * numChamadas + 1);
    }

    novasChamadas[numChamadas] = idQuadro;

    return novasChamadas;
}

/* Função usada para alocar as páginas de um processo */
Processo *alocarPaginas(
    Processo *processo,
    int tamanhoPagina,
    int quantidadeDePaginas,
    int tamanho)
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

    (*processo).paginas = *paginas;

    return processo;
}

/* Função usada para alocar os quadros ocupados por um processo na memória principal */
int alocarQuadros(
    Processo *processo,
    Quadro **quadros,
    int quantidadeDeQuadros,
    int tamanho, 
    int tamanhoPagina,
    int **chamadas,
    int numChamadas)
{
    int quantidadeAAlocar = tamanho / tamanhoPagina;

    if (quantidadeAAlocar == 0)
    {
        quantidadeAAlocar = 1;
    }

    for (int i = 0; i < quantidadeAAlocar; i++)
    {
        int idQuadro = executarLru(quadros, quantidadeDeQuadros, *chamadas, numChamadas);
        processo->paginas[i].idQuadro = idQuadro;
        processo->paginas[i].livre = 0;
        quadros[idQuadro]->setNomeProcesso(processo->nome);
        quadros[idQuadro]->livre = 0;
        *chamadas = atualizaChamadas(idQuadro, numChamadas, *chamadas);
        numChamadas++;
    }

    return quantidadeAAlocar;
}

int obterPaginaPeloEndereco(
    int endereco,
    int tamanhoPagina)
{
    if (endereco < tamanhoPagina)
    {
        return 0;
    }

    return (int)((long)tamanhoPagina + (long)endereco - 1) / (long)endereco;
}

/* Função usada para realizar a leitura ou escrita de um endereço */
void realizarLeituraOuEscrita(
    bool leitura,
    Quadro **quadros,
    Processo processo,
    int tamanhoPagina,
    int endereco,
    int quantidadeDeQuadros,
    int **chamadas,
    int numChamadas)
{
    int idPagina = obterPaginaPeloEndereco(endereco, tamanhoPagina);
    int test = processo.paginas[idPagina].idQuadro;
    char *test2 = quadros[processo.paginas[idPagina].idQuadro]->nomeProcesso;
    char *test3 = processo.nome;

    // Se o quadro pertence ao processo, acessa o mesmo
    if (processo.paginas[idPagina].idQuadro != -1 && strcmp(quadros[processo.paginas[idPagina].idQuadro]->nomeProcesso, processo.nome) == 0)
    {
        if (leitura)
        {
            printf("\nProcesso %s: Lendo endereço %d da página %d -> quadro %d", processo.nome, endereco, idPagina, processo.paginas[idPagina].idQuadro);
            *chamadas = atualizaChamadas(processo.paginas[idPagina].idQuadro, numChamadas, *chamadas);
        }
        else
        {
            printf("\nProcesso %s: Escrevendo no endereço %d da página %d -> quadro %d", processo.nome, endereco, idPagina, processo.paginas[idPagina].idQuadro);
            *chamadas = atualizaChamadas(processo.paginas[idPagina].idQuadro, numChamadas, *chamadas);
        }
    }
    else
    {
        printf("\nPage fault");
        int idQuadro = executarLru(quadros, quantidadeDeQuadros, *chamadas, numChamadas);
        processo.paginas[idPagina].idQuadro = idQuadro;
        quadros[idQuadro]->setNomeProcesso(processo.nome);

        printf("\nProcesso %s: Swap in da página %d para o quadro %d", processo.nome, idPagina, idQuadro);

        if (leitura)
        {
            printf("\nProcesso %s: Lendo endereço %d da página %d -> quadro %d", processo.nome, endereco, idPagina, idQuadro);
            *chamadas = atualizaChamadas(idQuadro, numChamadas, *chamadas);
        }
        else
        {
            printf("\nProcesso %s: Escrevendo no endereço %d da página %d -> quadro %d", processo.nome, endereco, idPagina, idQuadro);
            *chamadas = atualizaChamadas(idQuadro, numChamadas, *chamadas);
        }
    }
}

/* Função usada para obter um processo de um array pelo seu nome */
Processo *obterProcessoPeloNome(char *nome, Processo **processos, int numProcessos)
{
    for (int i = 0; i < numProcessos; i++)
    {
        char *test = processos[i]->nome;

        if (!strcmp(processos[i]->nome, nome))
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

    Quadro **quadros = (Quadro **)malloc(quantidadeDeQuadros * sizeof(Quadro));

    // Inicialização dos quadros da memória principal
    for (int i = 0; i < quantidadeDeQuadros; i++)
    {
        Quadro *quadro = new Quadro();
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

    Processo **processos = NULL;
    int numProcessos = 0;
    int noLinha = 0;

    int *chamadas = NULL;
    int numChamadas = 0;

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

                Processo *novoProcesso = new Processo(nome, tamanho, quantidadeDePaginas);
                novoProcesso = alocarPaginas(novoProcesso, tamanhoPagina, quantidadeDePaginas, tamanho);

                if (numProcessos == 0)
                {
                    processos = (Processo **)malloc(sizeof(Processo));
                }
                else
                {
                    processos = (Processo **)realloc(processos, (numProcessos + 1) * sizeof(Processo));
                }

                processos[numProcessos] = novoProcesso;

                int quantidadeAlocada = alocarQuadros(processos[numProcessos], quadros, quantidadeDeQuadros, tamanho, tamanhoPagina, &chamadas, numChamadas);
                numChamadas = numChamadas + quantidadeAlocada;
                numProcessos++;

                break;
            }
            case 'R':
            {
                int endereco = obterEndereco(linha);

                char *test = (*processos)[1].nome;

                Processo *processo = obterProcessoPeloNome(nome, processos, numProcessos);

                realizarLeituraOuEscrita(1, quadros, *processo, tamanhoPagina, endereco, quantidadeDeQuadros, &chamadas, numChamadas);
                numChamadas = numChamadas + 1;

                break;
            }
            case 'W':
            {
                int endereco = obterEndereco(linha);
                Processo *processo = obterProcessoPeloNome(nome, processos, numProcessos);

                realizarLeituraOuEscrita(0, quadros, *processo, tamanhoPagina, endereco, quantidadeDeQuadros, &chamadas, numChamadas);
                numChamadas = numChamadas + 1;

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

        sleep(1);

        noLinha++;
    }

    fclose(arq);

    printf("\n\n");
}
