#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pack {
  char **allPacks;
  int index;
  int size;
} Pack;

// Ler os dados do arquivo e salvar no vetor
void readInput(Pack *packages, FILE *input, int index) {
  char buffer[100];

  fscanf(input, "%d %d", &packages[index].index, &packages[index].size);
  packages[index].allPacks = malloc(packages[index].size * sizeof(char *));

  for (int j = 0; j < packages[index].size; j++) {
    fscanf(input, "%s", buffer);
    packages[index].allPacks[j] = malloc(strlen(buffer) + 1);
    strcpy(packages[index].allPacks[j], buffer);
  }
}

void trocar(Pack *v, int i, int j) {
  Pack temp = v[i];
  v[i] = v[j];
  v[j] = temp;
}

void heapify(Pack *v, int size, int index) {
  int p = index, e = 2 * index + 1, d = 2 * index + 2;

  if (e < size && v[e].index < v[p].index) p = e;
  if (d < size && v[d].index < v[p].index) p = d;

  if (p != index) {
    trocar(v, p, index);
    heapify(v, size, p);
  }
}

void heapsort(Pack *v, int n) {
  for (int i = n/2 - 1; i >= 0; i--) heapify(v, n, i);
}

void print(Pack packages, FILE *output) {
 //TODO: Ajustar para ser impresso no arquivo de saída 
    fprintf(output, "|");
      for (int j = 0; j < packages.size; j++) {
        fprintf(output, "%s", packages.allPacks[j]);
        if (j + 1 != packages.size) {
          fprintf(output, ",");
        }
      }
}

void freeMem(Pack *packages, int n) {
    // Liberando a memória alocada
  for (int i = 0; i < n; i++) {
    if (packages[i].allPacks != NULL) {
        for (int j = 0; j < packages[i].size; j++) {
            free(packages[i].allPacks[j]);
        }
        free(packages[i].allPacks);
    }
  }
}

// Main
int main(int argc, char *argv[]) {

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

  int n, qtd;
  fscanf(input, "%d %d", &n, &qtd);
  Pack packages[n];

  int index = 0;
  int current_index = 0;
  int flag = 0;
  
  // Lendo os pacotes
  for (int i = 0; i < n; i++) {   
    int total = 0;
    for (int i = 0; i < qtd; i++) {
      readInput(packages, input, index);
      index++;
      total++;
    }
      heapsort(packages, index); // Realiza a ordenação parcial a cada bloco de pacotes lidos
      
      for (int i = index - 1; i >= 0; i--) {
        if (packages[0].index == current_index) {
          print(packages[0], output);
          trocar(packages, 0, index - 1);
          current_index++;
          index--;
          heapify(packages, index, 0);
          flag = 1;
      }
    }
    if (flag && total != 1) {
      fprintf(output, "|\n");
      flag = 0;
    }
  }
  freeMem(packages, n);
  fclose(input);
  return 0;
}
