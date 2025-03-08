#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_HEX 256
// Definicao da estrutura dos bytes
typedef struct dado {
  int ordem, qtdBytes, qtdTiposbytes;
  int *bytes;
  char **codigoHuffman;
} Dado;

// Definição do nó da árvore de prefixos
typedef struct no {
  int frequencia, simbolo;
  struct no *direita, *esquerda;
} No;

// Definição da fila de prioridade
typedef struct filaPraMin {
  No **heap;
  int tamanho;
} FilaPMin;

// Definição do histograma
typedef struct histograma {
  int frequencia, numero;
} Histograma;

// Definição dos dados comprimidos Huffman e RLE
typedef struct huff {
  int contador, flag;
  float porcentagemCompressao;
  char* dadoComprimido;
} Huff;

typedef struct rle {
  float porcentagemCompressao;
  int *dadoComprimido;
  int i;
} RLE;

// Ler os dados do arquivo de entrada
void readInput(FILE *input, Dado *novoDado) {
  fscanf(input, "%d", &novoDado->qtdBytes);
  novoDado->bytes = (malloc(novoDado->qtdBytes * (sizeof(int))));

  for (int i = 0; i < novoDado->qtdBytes; i++) {
    fscanf(input, "%x", &novoDado->bytes[i]);
  }
}

Histograma *contarFrequenciaBytes(Dado *dado) {
  int frequencias[MAX_HEX] = {0};
  dado->qtdTiposbytes = 0;
  Histograma *histograma = malloc(MAX_HEX * sizeof(Histograma));

  // Inicializando o histograma com frequências 0
  for (int i = 0; i < MAX_HEX; i++) {
    histograma[i].frequencia = 0;
    histograma[i].numero = i;  // Opcional: inicializar número, dependendo da lógica
  }

  for (int i = 0; i < dado->qtdBytes; i++) {
    frequencias[dado->bytes[i]] += 1;
  }

    for (int i = 0, index = 0; i < MAX_HEX; i++) {
      if (frequencias[i]) {
        histograma[index].frequencia = frequencias[i];
        histograma[index].numero = i;
        dado->qtdTiposbytes++;
        index++;
      }
    }
  return histograma;
}

// Inicializa a fila de prioridades
FilaPMin *inicializarFila() {
  FilaPMin *fila = (FilaPMin *)malloc(sizeof(FilaPMin));
  fila->heap = (No**)malloc(MAX_HEX * sizeof(No*));
  fila->tamanho = 0;
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

  int pai = (index - 1)/2;

  if (index > 0 && fila->heap[pai]->frequencia > fila->heap[index]->frequencia) {
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

  for (int i = 0; i < MAX_HEX && histograma[i].frequencia; i++) {
      inserir(fila, criarNo(histograma[i].numero, histograma[i].frequencia, NULL, NULL));
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
  
  if (!raiz) return;

  if (!raiz->esquerda && !raiz->direita) {
    caminho[profundidade] = '\0';
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

  preencherTabela(raiz, tabelaHuff, caminho, 0);
  return tabelaHuff;
}

void gerarCodigosHuff(int *bytes, char **tabelaHuff, char **codigos, int qtdBytes) {
   for (int i = 0; i < qtdBytes; i++) {
    codigos[i] = strdup(tabelaHuff[bytes[i]]);
  }
}

int calcularProfundidade(No *raiz) {
  if (!raiz) return 0;
  if (!raiz->esquerda && !raiz->direita) return 1;

    int profundidadeEsq = calcularProfundidade(raiz->esquerda);
    int profundidadeDir = calcularProfundidade(raiz->direita);
    
  return 1 + ((profundidadeEsq > profundidadeDir) ? profundidadeEsq : profundidadeDir);
}

float calcularPorcentagemCompressao(float tamanhoCompactado, int qtd) {
  return ((float)tamanhoCompactado/(qtd * 2)) * 100;
}

void gerarString(Dado *dado, char **tabelaHuff, char **c) {
  int tamanho = dado->qtdBytes * 8 + 1;
  *c = malloc(tamanho * sizeof(char));
  (*c)[0] = '\0';

  for (int i = 0; i < dado->qtdBytes; i++) {
    strcat(*c, tabelaHuff[dado->bytes[i]]);
  }

  while(strlen(*c) % 8 != 0) {
    strcat(*c, "0");
  }

//  printf("String binária gerada: %s\n", *c);  // Debug aqui
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

    huffmanCompactado->porcentagemCompressao = calcularPorcentagemCompressao(contador*2, dados->qtdBytes);
    huffmanCompactado->contador = contador;
  } else {
    huffmanCompactado->flag = 0;
    gerarString(dados, tabelaHuff, &huffmanCompactado->dadoComprimido);
    //bin_to_hex(huffmanCompactado->dadoComprimido);

    float tamanho = (float)strlen(huffmanCompactado->dadoComprimido)/4;
    huffmanCompactado->porcentagemCompressao = calcularPorcentagemCompressao(tamanho, dados->qtdBytes);
    //huffmanCompactado->dadoComprimido = strtoull(c, NULL, 2);
    //printf("Valor convertido: %llu\n", huffmanCompactado->dadoComprimido);
    //huffmanCompactado->contador = (int)(strlen(huffmanCompactado->dadoComprimido)/4);
  //  free(c);
  }
}

Huff *huffman(Dado *dados) {
  Histograma *histograma = contarFrequenciaBytes(dados); 
  No *topo = construirArvore(histograma);
  char **tabelaHuff = gerarTabelaHuff(topo);
  char **codigos = malloc(dados->qtdBytes * sizeof(char *));
  gerarCodigosHuff(dados->bytes, tabelaHuff, codigos, dados->qtdBytes);
  Huff *huffmanCompactado = malloc(sizeof(Huff));
  compactar(dados, tabelaHuff, huffmanCompactado, calcularProfundidade(topo));

  return huffmanCompactado;
}

void bin_to_hex(const char *bin) {
    int len = strlen(bin);
    int num_bytes = (len + 7) / 8; // Arredonda para cima se não for múltiplo de 8
    unsigned char *bytes = malloc(num_bytes);

    if (!bytes) {
        printf("Erro ao alocar memória.\n");
        return;
    }

    memset(bytes, 0, num_bytes);

    for (int i = 0; i < len; i++) {
        int byte_pos = i / 8;
        bytes[byte_pos] = (bytes[byte_pos] << 1) | (bin[i] - '0');
    }

   // printf("Hexadecimal: ");
    for (int i = 0; i < num_bytes; i++) {
        printf("%02X", bytes[i]);
    }
    printf("\n");

    free(bytes);
}


void printHuffman(FILE *output, int ordem, Huff *huffman) {
  fprintf(output, "%d->HUF(%.2f%%)=", ordem, huffman->porcentagemCompressao);
  
  int tamanhoBinario = strlen(huffman->dadoComprimido);
  int numBytes = (tamanhoBinario + 7) / 8;
  unsigned char *bytes = malloc(numBytes);

  memset(bytes, 0, numBytes);

  for (int i = 0; i < tamanhoBinario; i++) {
    int posicaoByte = i/8;
    bytes[posicaoByte] = (bytes[posicaoByte] << 1 | huffman->dadoComprimido[i] - '0');
  }

  for (int i = 0; i < numBytes; i++) {
    fprintf(output, "%02X", bytes[i]);
  }
  fprintf(output, "\n");
  
  free(bytes);
}

RLE *contarBytesRLE(Dado *dados) {
  
  RLE *rle = malloc(sizeof(RLE));
  rle->dadoComprimido = malloc((dados->qtdBytes * 2) * sizeof(int));

  int qtd = dados->qtdBytes;
  int index = 0;

  for (int i = 0; i < qtd; i++) {
    int contador = 1;
    int atual = dados->bytes[i];

    while (i + 1 < qtd && atual == dados->bytes[i + 1]) {
      contador++;
      i++;
    }
    
    rle->dadoComprimido[index] = contador;
    rle->dadoComprimido[index + 1] = dados->bytes[i];
    index += 2;
    contador = 1;
  }

  rle->porcentagemCompressao = calcularPorcentagemCompressao(index * 2, qtd);
  rle->i = index;

  return rle;
}

void printRLE(FILE *output, Dado *dado, RLE *rle) {
  fprintf(output, "%d->RLE(%.2f%%)=", dado->ordem, rle->porcentagemCompressao);
  
  for (int i = 0; i < rle->i; i+=2) {
    fprintf(output, "%02X", rle->dadoComprimido[i]);
    fprintf(output, "%02X", rle->dadoComprimido[i + 1]);
  }

  fprintf(output, "\n");

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

  // Ler os dados e armazenar num vetor
  for (int i = 0; i < qtdDados; i++) {
    Dado *dado = malloc(sizeof(Dado));
    readInput(input, dado);
    dado->ordem = i;

    Huff *huff = huffman(dado);
    RLE *rle = contarBytesRLE(dado);
    if (huff->porcentagemCompressao == rle->porcentagemCompressao) {
      printHuffman(output, i, huff);
      printRLE(output, dado, rle);
    } else if (huff->porcentagemCompressao > rle->porcentagemCompressao) {
      printRLE(output, dado, rle);
    } else {
      printHuffman(output, i, huff);
    } 
  
    free(dado);
    free(huff);
    free(rle);
  }

  fclose(input);
  fclose(output);
  return 0;
}
