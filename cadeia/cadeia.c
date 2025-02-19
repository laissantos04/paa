#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct doenca {
  char *nome;
  char **genes;
  char **subcadeias;
  int ordem;
  int qtd_genes;
  int qtd_subcadeias;
} Doenca;

void readInput(FILE *input, int ordem, Doenca *doenca) {
    doenca->nome = malloc(11 * sizeof(char));
    fscanf(input, "%s %d", doenca->nome, &doenca->qtd_genes);

    doenca->genes = malloc(doenca->qtd_genes * sizeof(char *)); 

    for (int j = 0; j < doenca->qtd_genes; j++) {
      doenca->genes[j] = malloc (100 * sizeof(char));
      fscanf(input, "%s", doenca->genes[j]);
    }
    doenca->ordem = ordem;
    doenca->qtd_subcadeias = 0;
}

char** quebrarCadeia(char *gene, int tam_subcadeia, int *qtd_subc) {
  int tam_gene = strlen(gene);
  int qtd_subcadeias = tam_gene - tam_subcadeia + 1;

  if (qtd_subcadeias <= 0) {
    qtd_subcadeias = 0;
    return NULL;
  }

  char **subcadeias = (char**)malloc(qtd_subcadeias * sizeof(char*));
  
  for (int i = 0; i < qtd_subcadeias; i++) {
    subcadeias[i] = (char*)malloc((tam_subcadeia + 1) * sizeof(char));
    strncpy(subcadeias[i], gene+i, tam_subcadeia);
    subcadeias[i][tam_subcadeia] = '\0';
  }
  *qtd_subc += qtd_subcadeias;
  return subcadeias;
}

void inserir(int32_t *V, int32_t i, int32_t *c) {
  V[*c] = i;
  (*c)++;
}

void calcularTabela(int32_t *k, char *P) {
  // i = sufixo, j = prefixo
  for (int32_t i = 1, j = -1; i < strlen(P); i++) {
    while (j >= 0 && P[j+1] != P[i])
      j = k[j];
    if (P[j+1] == P[i])
      j++;
    k[i] = j;
  }
}

void KMP(int32_t *k, int32_t *R, char *T, char *P) {
  // pré-processamento
  int32_t n = strlen(T), m = strlen(P), c = 0;
  calcularTabela(k, P);

  for (int32_t i = 0, j = -1; i < n; i++) {
    while(j >= 0 && P[j+1] != T[i]) 
      j = k[j];
    if (P[j+1] == T[i])
      j++;
    if (j == m-1) {
      inserir(R, i-m+1, &c);
      j = k[j];
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
  
  int tam_cadeia;
  fscanf(input, "%d", &tam_cadeia);
  char dna[1024];
  fscanf(input, "%s", dna);
  int tam_dna = strlen(dna);
  
  int qtd_doencas;
  fscanf(input, "%d", &qtd_doencas);
  Doenca doencas[qtd_doencas];

  for (int i = 0; i < qtd_doencas; i++) {
    readInput(input, i, &doencas[i]);
  }

  for (int i = 0; i < qtd_doencas; i++) {
    int total_subcadeias = 0;

    for (int j = 0; j < doencas[i].qtd_genes; j++) {
      int qtd_sub;
      doencas[i].subcadeias = quebrarCadeia(doencas[i].genes[j], tam_cadeia, &qtd_sub);
      total_subcadeias += qtd_sub;
    }

    doencas[i].subcadeias = malloc(total_subcadeias * sizeof(char *));
    doencas[i].qtd_subcadeias = 0;

    for (int j = 0; j < doencas[i].qtd_genes; j++) {
      int qtd_sub = 0;
      char **subcadeias = quebrarCadeia(doencas[i].genes[j], tam_cadeia, &qtd_sub);

      for (int k = 0; k < qtd_sub; k++) {
        doencas[i].subcadeias[doencas[i].qtd_subcadeias++] = subcadeias[k];
      }
      free(subcadeias);
    }
  }

  for (int i = 0; i < qtd_doencas; i++) {
    for (int j = 0; j < doencas[i].qtd_subcadeias; j++) {
      int32_t tabela[tam_cadeia];
      int32_t indices[tam_dna];

      KMP(tabela, indices, dna, doencas[i].subcadeias[j]);
      for (int k = 0; k < tam_dna; k++) {
        printf("%d ", indices[k]);
      }
      printf("\n###########\n");
    }
  }

}
