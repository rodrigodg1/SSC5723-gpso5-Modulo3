

## Simulador de Memória Virtual Com Paginação

Com a técnica de memória virtual, o espaço de memória disponível ao programa pode exceder o tamanho da memória principal e um processo pode executar sem ter todas as instruções e dados dentro da memória principal. Uma das maneiras de implementar essa técnica é por meio da paginação, onde a memória é particionada em blocos/quadros de tamanho fixo. Cada processo é dividido em páginas de tamanho igual ao bloco. Assim,uma página de um processo pode ser carregado em um bloco de memória.
	
 - Essa abstração de página é representada pela classe *pagina* e as características do processo assim como a tabela de páginas é caracterizada pela classe *processo*.

Quando ocorre uma falta de página **( *page fault*)**, é necessário um algoritmo para realizar uma substituição de pagina na memória. Para o projeto, utilizamos o **LRU** (Menos Utilizado Recentemente).

## Como Compilar ?
```bash
g++ main.cpp
```
## Como Executar ?

```bash
./a.out [tamanhoDaPágina] [tamanhoEndereçoLógico] [tamanhoMemóriaFísica] [tamanhoMemóriaSecundária]
```


***Exemplo***  

    ./a.out  100 10000 100000 1000000
