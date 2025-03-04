#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_HEX 256
// Definicao da estrutura dos bytes
typedef struct dado {
  int ordem;
  int qtdBytes;
  int *bytes;
  int qtdTiposbytes;
  char **codigoHuffman;
} Dado;

// Definição do nó da árvore de prefixos
typedef struct no {
  // frequencia
  int frequencia;
  // código do símbolo
  int simbolo;
  // nó direito e esquerdo
  struct no *direita, *esquerda;
} No;

// Definição da fila de prioridades que será utilizada
typedef struct filaPraMin {
  No **heap;
  int tamanho;
} FilaPMin;

// Definição do histograma que será usado para construir a fila 
typedef struct histograma {
  int frequencia;
  int numero;
} Histograma;

typedef struct huff {
  int contador;
  int flag;
  float porcentagemCompressao;
  uint64_t dadoComprimido;
} Huff;

// Ler os dados do arquivo de entrada
void readInput(FILE *input, Dado *novoDado) {
  fscanf(input, "%d", &novoDado->qtdBytes);
  novoDado->bytes = (malloc(novoDado->qtdBytes * (sizeof(char*))));

  for (int i = 0; i < novoDado->qtdBytes; i++) {
    fscanf(input, "%x", &novoDado->bytes[i]);
  }
  novoDado->codigoHuffman = NULL;
}

Histograma *contarFrequenciaBytes(Dado *dado) {
  int n = dado->qtdBytes;
  int frequencias[MAX_HEX] = {0};
  int index = 0;
  dado->qtdTiposbytes = 0;

  Histograma *histograma = malloc(MAX_HEX * sizeof(Histograma));

  for (int i = 0; i < n; i++) {
    frequencias[dado->bytes[i]] += 1;
  }

  for (int i = 0; i < MAX_HEX; i++) {
    if (frequencias[i]) {
      histograma[index].frequencia = frequencias[i];
      dado->qtdTiposbytes++;
      histograma[index].numero = i;
      index++;
    }
  }

  return histograma;
}

// Inicializa a fila de prioridades
FilaPMin *inicializarFila() {
  FilaPMin *fila = (FilaPMin *)malloc(sizeof(FilaPMin));

    fila->tamanho = 0;
    fila->heap = (No**)malloc(MAX_HEX * sizeof(No*));

  return fila;
}

// Inicializa um novo nó
No *criarNo(int numero, int frequencia, No *direita, No *esquerda) {
  No *novoNo = malloc(sizeof(No));
  
    novoNo->frequencia = frequencia;
    novoNo->simbolo = numero;
    novoNo->direita = direita;
    novoNo->esquerda = esquerda;
  
  return novoNo;
}

// heapify
void heapify(FilaPMin *fila, int index) {


  if (index == 0) return;

  int pai = (index - 1)/2;

  if (fila->heap[pai]->frequencia > fila->heap[index]->frequencia) {
    No *temp = fila->heap[pai];
    fila->heap[pai] = fila->heap[index];
    fila->heap[index] = temp;
    heapify(fila, pai);
  }
}

// Insere um novo nó na fila de prioridade
void inserir(FilaPMin *fila, No *novoNo) {

  int i = fila->tamanho;
  fila->heap[i] = novoNo;
  heapify(fila, fila->tamanho);
  fila->tamanho++;

}

// Gera a fila de prioridades com base nas frequencias do histograma
FilaPMin *criarFilaPrioridade(Histograma *histograma) {

  FilaPMin *fila = inicializarFila();

  for (int i = 0; i < MAX_HEX; i++) {
    if (histograma[i].frequencia > 0) {
      No *novoNo = criarNo(histograma[i].numero, histograma[i].frequencia, NULL, NULL);
      inserir(fila, novoNo);
    }
  }
    return fila;
}

// Heapify min
void heapifyMin(FilaPMin *fila, int i) {

      int esquerda = 2 * i + 1;
      int direita = 2 * i + 2;
      int menorIndex = i;

      if (esquerda < fila->tamanho && fila->heap[esquerda]->frequencia < fila->heap[menorIndex]->frequencia) {
        menorIndex = esquerda;
      }

      if (direita < fila->tamanho && fila->heap[direita]->frequencia < fila->heap[menorIndex]->frequencia) {
        menorIndex = direita;
      }

      if (menorIndex != i) {
        No *temp = fila->heap[i];
        fila->heap[i] = fila->heap[menorIndex];
        fila->heap[menorIndex] = temp;

        heapifyMin(fila, menorIndex);
      }
}
 
// Retorna o nó com o menor valor da fila
No *extrairMin(FilaPMin *fila) {
  
  if (fila->tamanho == 0) {
    return NULL;
  }

    No *menor = fila->heap[0];
    fila->heap[0] = fila->heap[fila->tamanho - 1];
    fila->tamanho--;
  
  heapifyMin(fila, 0);
  return menor;
}

// Constrói a árvore de Huffman
No *construirArvore(Histograma *histograma) {
  FilaPMin *fila = criarFilaPrioridade(histograma);

    while (fila->tamanho > 1) {
      No *x = extrairMin(fila);
      No *y = extrairMin(fila);
      No *novoNo = criarNo('\0', x->frequencia + y->frequencia, x, y);
      inserir(fila, novoNo);
    }
  return extrairMin(fila);
}

void preencherTabela(No *raiz, char **tabela, char *caminho, int profundidade) {
  
  if (raiz == NULL) return;

  if (raiz->esquerda == NULL && raiz->direita == NULL) {
    if (profundidade == 0) {
      caminho[0] = '0';
      caminho[1] = '\0';
    } else {
      caminho[profundidade] = '\0';
    }
    tabela[raiz->simbolo] = strdup(caminho);
    return;
  }

  caminho[profundidade] = '1';
  preencherTabela(raiz->esquerda, tabela, caminho, profundidade + 1);

  caminho[profundidade] = '0';
  preencherTabela(raiz->direita, tabela, caminho, profundidade + 1);
}

char **gerarTabelaHuff(No *raiz) {
  char **tabelaHuff = (char **)calloc(MAX_HEX, sizeof(char *));
  char caminho[MAX_HEX];

  if (raiz == NULL) return tabelaHuff;

  preencherTabela(raiz, tabelaHuff, caminho, 0);
  return tabelaHuff;
}

void gerarCodigosHuff(int *bytes, char **tabelaHuff, char **codigos, int qtdBytes) {
   for (int i = 0; i < qtdBytes; i++) {
    codigos[i] = malloc(MAX_HEX * sizeof(char));
    strcpy(codigos[i], tabelaHuff[bytes[i]]);
  }
}

int calcularProfundidade(No *raiz) {
  if (raiz == NULL) return 0;
  if (raiz->esquerda == NULL && raiz->direita == NULL) return 1;

    int profundidadeEsq = calcularProfundidade(raiz->esquerda);
    int profundidadeDir = calcularProfundidade(raiz->direita);
    
  return 1 + ((profundidadeEsq > profundidadeDir) ? profundidadeEsq : profundidadeDir);
}

void compactar(Dado *dados, char **tabelaHuff, Huff *huffmanCompactado, int profundidadeArvore) {
  if (profundidadeArvore == 1) {
    huffmanCompactado->flag = 1;
    int qtd = dados->qtdBytes;
    int contador = 0;

    while (qtd > 0) {
      contador++;
      qtd -= 8;
    }

    float porcentagem = ((float)(contador * 2)/(dados->qtdBytes * 2)) * 100;
    huffmanCompactado->porcentagemCompressao = porcentagem;
    huffmanCompactado->contador = contador;
  } else {
    huffmanCompactado->flag = 0;
    char *c = malloc((dados->qtdBytes * 8 + 1) * sizeof(char));
    c[0] = '\0';

    for (int i = 0; i < dados->qtdBytes; i++) {
      strcat(c, tabelaHuff[dados->bytes[i]]);
    }

    while (strlen(c) % 8 != 0) {
      strcat(c, "0");
    }

    float porcentagem = ((float)strlen(c)/ 4 / (dados->qtdBytes * 2)) * 100;
    huffmanCompactado->porcentagemCompressao = porcentagem;

    uint64_t num = strtoull(c, NULL, 2);
    huffmanCompactado->dadoComprimido = num;
    huffmanCompactado->contador = (int)(strlen(c)/4);
    free(c);

  }
}

Huff *huffman(Dado *dados) {
  // Constrói o histograma
  Histograma *histograma = contarFrequenciaBytes(dados);
  // Cria uma fila de prioridade mínima usando o histograma
  FilaPMin *fila = criarFilaPrioridade(histograma); 
  // Constrói a árvore de Huffman
  No *topo = construirArvore(histograma);
  // Gera a tabela de correspondências de Huffman
  char **tabelaHuff = gerarTabelaHuff(topo);
  // Gera os códigos de cada byte
  char **codigos = malloc(dados->qtdBytes * sizeof(char *));
  gerarCodigosHuff(dados->bytes, tabelaHuff, codigos, dados->qtdBytes);
  // Compacta os códigos Huffman gerados
  Huff *huffmanCompactado = malloc(sizeof(Huff));
  int profundidadeArvore = calcularProfundidade(topo);
  compactar(dados, tabelaHuff, huffmanCompactado, profundidadeArvore);

  return huffmanCompactado;
}

void printHuffman(Huff *huffman, Dado *dado, FILE *output) {
  fprintf(output, "%d->HUF(%.2f%%)=", dado->ordem, huffman->porcentagemCompressao);
  
  if (huffman->flag) {
    for (int i = 0; i < huffman->contador; i++) {
      fprintf(output, "00");
    }
    fprintf(output, "\n");
  } else {
    fprintf(output, "%0*lX\n", huffman->contador,huffman->dadoComprimido);
  }
}

int main (int argc, char *argv[]) {
        
  if (argc != 3) {
      printf("Necessário especificar os dois arquivos de entrada.\n");
    return 1;
  }

  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");

  if (input == NULL) {
    printf("Erro ao abrir o arquivo de entrada.\n");
    return 1;
  }

  int qtdDados;
  fscanf(input, "%d", &qtdDados);
  Dado **todosDados = malloc(qtdDados * sizeof(Dado*));
  
  // Ler os dados e armazenar num vetor
  for (int i = 0; i < qtdDados; i++) {
    todosDados[i] = malloc(sizeof(Dado));
    todosDados[i]->ordem = i;
    readInput(input, todosDados[i]);
  }

  // Processa cada conjunto de dados individualmente
  for (int i = 0; i < qtdDados; i++) {
    Huff *huff = huffman(todosDados[i]);
    printHuffman(huff, todosDados[i], output);
  }


}
