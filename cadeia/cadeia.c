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

void calcularTabela(int *k, char *p) {
  int m = strlen(p);
  k[0] = -1;
  for (int i = 1, j = -1; i < m; i++) {
    while(j >= 0 && p[j+1] != p[i]) {
      j = k[j];
    } 
    if (p[j+1] == p[i]) {
      j++;
    }
    k[i] = j;
  }
}

int contarCorrespondencias(int tamanhoSubcadeia, char *dna, char *gene) {
  int n = strlen(dna), m = strlen(gene);
  int contador = 0;
  int i = 0, j = 0;

  while (i < m && j < n) {
    int acc = 0;

    while (j < n && dna[j] == gene[i]) {
      acc++;
      j++;
      i++;
    }

    if (acc >= tamanhoSubcadeia) {
      contador += acc;
    }
    j++;
    i++;
  }
  return contador;
}

int contarOcorrencias(char *dna, char *gene, int tamanhoSubcadeia) {
  int tamanho_gene = strlen(gene);
  int tamanho_dna = strlen(dna);
  int correspondencias = 0;
  int contador = 0;
  
  int k[tamanho_dna];
  int r[tamanho_gene];
  
   correspondencias = contarCorrespondencias(tamanhoSubcadeia, dna, gene);
  printf("Correspondencias: %d\n", correspondencias);
  return correspondencias;
}


double calcularProbabilidade(char **genes, int qtdGenes, char *dna, int tamanhoSubcadeia) {
  int genesValidos = 0;

  for (int i = 0; i < qtdGenes; i++) {
    int caracteresCorresp = contarOcorrencias(dna, genes[i], tamanhoSubcadeia);
    int tamanhoGene = strlen(genes[i]);

    double percent = (double) caracteresCorresp / tamanhoGene * 100;

    if (percent >= 90.0) {
      genesValidos++;
    }
  }

  double probabilidade = (double) genesValidos / qtdGenes * 100;
  return probabilidade;
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

  // Lendo os dados do arquivo de entrada
  for (int i = 0; i < qtd_doencas; i++) {
    readInput(input, i, &doencas[i]);
  }

  for (int i = 0; i < qtd_doencas; i++) {
    double percentual = calcularProbabilidade(doencas[i].genes, doencas[i].qtd_genes, dna, tam_cadeia);



    printf("Doenca: %s -> Percentual: %.2f\n\n\n", doencas[i].nome, percentual);
  }
}
