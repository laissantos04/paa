#include <stdio.h>
#include <stdlib.h>

#define MAX_HEX 256
// Definicao da estrutura dos bytes
typedef struct dado {
  int qtdBytes;
  int *bytes;
  int qtdTiposbytes;
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

// Ler os dados do arquivo de entrada
void readInput(FILE *input, Dado *novoDado) {
  fscanf(input, "%d", &novoDado->qtdBytes);
  novoDado->bytes = (malloc(novoDado->qtdBytes * (sizeof(char*))));

  for (int i = 0; i < novoDado->qtdBytes; i++) {
    fscanf(input, "%x", &novoDado->bytes[i]);
  }
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

// Insere um novo nó na fila de prioridade
void inserir(FilaPMin *fila, No *novoNo) {

  int i = fila->tamanho;
  fila->heap[i] = novoNo;
  fila->tamanho++;

    while(i > 0) {
      int pai = (i-1)/2;
    
      if (fila->heap[i]->frequencia >= fila->heap[pai]->frequencia) {
        break;
      }

      No *temp = fila->heap[i];
      fila->heap[i] = fila->heap[pai];
      fila->heap[pai] = temp;

      i = pai;
    }
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

// Retorna o nó com o menor valor da fila
No *extrairMin(FilaPMin *fila) {
  
  if (fila->tamanho == 0) {
    return NULL;
  }

    No *menor = fila->heap[0];
    fila->heap[0] = fila->heap[fila->tamanho - 1];
    fila->tamanho--;

    int i = 0;
    while (1) {
      int esquerda = 2 * i + 1;
      int direita = 2 * i + 2;
      int menorIndex = i;

      if (esquerda < fila->tamanho && fila->heap[esquerda]->frequencia < fila->heap[menorIndex]->frequencia) {
        menorIndex = esquerda;
      }

      if (direita < fila->tamanho && fila->heap[direita]->frequencia < fila->heap[menorIndex]->frequencia) {
        menorIndex = direita;
      }

      if (menorIndex == i) {
        break;
      }

      No *temp = fila->heap[i];
      fila->heap[i] = fila->heap[menorIndex];
      fila->heap[menorIndex] = temp;

      i = menorIndex;
    }

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

void huffman(Dado *dados) {
  // Constrói o histograma
  Histograma *histograma = contarFrequenciaBytes(dados);
  // Cria uma fila de prioridade mínima usando o histograma
  FilaPMin *fila = criarFilaPrioridade(histograma); 
  // Constrói a árvore de Huffman
  No *topo = construirArvore(histograma);
  // TODO: Gerar os códigos de Huffman

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
    readInput(input, todosDados[i]);
  }

  // Processa cada conjunto de dados individualmente
  for (int i = 0; i < qtdDados; i++) {
    huffman(todosDados[i]);

  }


}
