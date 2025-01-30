#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <time.h>

typedef struct container {
  char code[25], cnpj[25], error[25];
  int weight_diff;
  int index, weight;
} Container;

// Função para comparar os conteineres para identificar divergencias de CNPJ ou peso
int compareContainer(Container c1, Container c2) {
  if (strcmp(c1.cnpj, c2.cnpj) != 0) {
     return 1; // CNPJ diferente
  }
    int v1 = c1.weight;
    int v2 = c2.weight;

    if (fabs((double)v1 - v2) >= 0.1 * fabs((v1 + v2) / 2.0)) {
      return -1; // Peso diferente
    }
  return 0; // Sem diferenças
}

// Função para ler e processar o arquivo de input, salvando cada container dentro do vetor de containersv
void readInput(FILE *input, Container *containers, int size) {

  for (int j = 0; j < size; j++) {
    fscanf(input, "%s %s %d", containers[j].code, containers[j].cnpj, &containers[j].weight);
    memset(containers[j].error, 0, sizeof(containers[j].error));
    containers[j].weight_diff = 0;
    containers[j].index = j;
  }
}

// Função para realizar a busca binária de um conteiner pelo código
int search(Container *v, int size, const char *searchCode) {
  int i = 0, j = size - 1;

  while (i <= j) {
    int m = i + (j - i) / 2;
    int cmp = strcmp(v[m].code, searchCode);

    if (cmp == 0) {
      return m; // Se encontrar o código, retorna o índice
    } else if (cmp > 0) {
      j = m - 1; // Procura na metade esquerda
    } else {
      i = m + 1; // Procura na metade direita
    }
  }
  return -1; // Se não encontrar o código
}

// Função para processar as diferenças entre os vetores, de acordo com o resultado da comparação.
// De acordo com as diferenças de cada elemento, atribui ao vetor correspondente
void processDiff(Container *allContainers, int qtd_all, Container *selectedContainers, 
                 int qtd_selected, Container *div_cnpj, Container *div_weight, 
                 int *iCnpj, int *iWeight) {
    
  for (int i = 0; i < qtd_selected; i++) {
   
    // Realiza a busca binária do conteiner
    int j = search(allContainers, qtd_all, selectedContainers[i].code); 
      if(j != -1) {
      // Compara se existe diferenca entre o CNPJ ou peso
        int cmp = compareContainer(allContainers[j], selectedContainers[i]);
     
      // Se houver diferenca de CNPJ
        if (cmp == 1) {
        // Copia o container diferente para o vetor de diferencas do CNPJ
          div_cnpj[*iCnpj] = allContainers[j]; 
          strcpy(div_cnpj[*iCnpj].error, selectedContainers[i].cnpj);
          (*iCnpj)++;
        // Se houver diferenca de peso
        } else if (cmp == -1) {
       
        // Calculo para verificar se a diferenca é maior do que 10%
          int v1 = allContainers[j].weight;
          int v2 = selectedContainers[i].weight;
          int diff = abs(v1 - v2);
          float p10 = ((float)v1 * 10) / 100;
          float perc = (diff * 10) / p10;
          int perc_int = (int)round(perc);
       
          if (perc_int > 10) {
          // Copia o container diferente para o vetor de diferencas do peso
            div_weight[*iWeight] = allContainers[j];
            div_weight[*iWeight].weight_diff = perc_int;
            sprintf(div_weight[*iWeight].error, "%d", diff);
            (*iWeight)++; 
          }
        }
      }
  }
} 

// Copia os dados do vetor temporario para o vetor final
void copiar(Container *v1, Container *v2, int index) {
  for (int i = 0; i < index; i++) {
    v1[i] = v2[i];
  }
}

// Intercala os dados das diferencas entre CNPJ
void intercalarCnpj(Container *v, Container *temp, int i, int m, int j) {
  int c1 = i, c2 = m + 1; // definição dos índices iniciais de conteineres
  int k = i;

  while (c1 <= m && c2 <= j) {

    if (v[c1].index < v[c2].index) {
      temp[k++] = v[c1++];
    } else {
      temp[k++] = v[c2++];
    }
  }

  if (c1 > m) {
    copiar(&temp[k], &v[c2], j - c2 + 1);
  } else {
    copiar(&temp[k], &v[c1], m - c1 + 1);
  }
  copiar(&v[i], &temp[i], j - i + 1);
}

// Intercala os dados das diferencas de peso
void intercalarWeight(Container *v, Container *temp, int i, int m, int j) {
  int c1 = i, c2 = m + 1;
  int k = i;

  while (c1 <= m && c2 <= j) {
    if (v[c1].weight_diff > v[c2].weight_diff) {
      temp[k++] = v[c1++];
    } else if (v[c1].weight_diff < v[c2].weight_diff) {
      temp[k++] = v[c2++];
    } else if (v[c1].weight_diff == v[c2].weight_diff) {
      if (v[c1].index < v[c2].index) {
        temp[k++] = v[c1++];
      } else {
        temp[k++] = v[c2++];  
      }
    }
  }

  if (c1 > m) {
    copiar(&temp[k], &v[c2], j - c2 + 1);
  } else {
    copiar(&temp[k], &v[c1], m - c1 + 1);
  }
  copiar(&v[i], &temp[i], j - i + 1);
}

// Função para intercalar o vetor original
void intercalar(Container *v, Container *temp, int i, int m, int j) {
  int c1 = i, c2 = m + 1, k = i;

  while (c1 <= m && c2 <= j) {
// Função para comparar com base no código
    if (strcmp(v[c1].code, v[c2].code) < 0) { // se c1 < c2
      temp[k++] = v[c1++];
    } else {
      temp[k++] = v[c2++];
    }
  }

  if (c1 > m) {
    copiar(&temp[k], &v[c2], j - c2 + 1);
  } else {
    copiar(&temp[k], &v[c1], m - c1 + 1);
  }
  copiar(&v[i], &temp[i], j - i + 1);
}

// Metodo mergesort para realizar a ordenacao
void merge(Container *v, Container *temp, int i, int j, int c) { // c = critério para saber se é CNPJ ou peso
  if (i < j) {
    int m = i + (j - i) / 2;

    merge(v, temp, i, m, c);
    merge(v, temp, m+1, j, c);
    if (c == 1) {
      intercalarCnpj(v, temp, i, m, j);
    } else if (c == 2) {
      intercalarWeight(v, temp, i, m, j);
    } else {
      intercalar(v, temp, i, m, j);
    }
  }
}

// Printar os dados no arquivo de saida
void print(FILE *output, Container *v, int size, int c) {
  if (c == 1) {
    for (int i = 0; i < size; i++) {
      fprintf(output, "%s:%s<->%s\n", v[i].code, v[i].cnpj, v[i].error);
    }
  } else if (c == 2) {
    for (int i = 0; i < size; i++) {
      fprintf(output, "%s:%skg(%d%%)\n", v[i].code, v[i].error, v[i].weight_diff);
    }
  }
}

// Main
int main(int argc, char* argv[]) {
clock_t inicio = clock();
 if (argc < 2) {
    printf("Necessario informar o input e o output.\n");
    return 1;
  }

  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");
  if (input == NULL || output == NULL) {
    printf("Problema ao abrir os arquivos.\n");
    return 1;
  }

  int qtd, qtd_selected;

  fscanf(input, "%d", &qtd);
  Container *allContainers = malloc(qtd * sizeof(Container));
  // Ler os dados de entrada adicionando no vetor original
  readInput(input, allContainers, qtd);

  Container *auxTemp = malloc(qtd * sizeof(Container));
  // Ordena o vetor original pela ordem lexicográfica
  merge(allContainers, auxTemp, 0, qtd - 1, 0); 

  fscanf(input, "%d", &qtd_selected);
  // Ler os dados de entrada adicionando no vetor de conteineres selecionados
  Container *selectedContainers = malloc(qtd * sizeof(Container));
  readInput(input, selectedContainers, qtd_selected);

  Container *diffCnpj = malloc(qtd_selected * sizeof(Container));
  Container *diffWeight = malloc(qtd_selected * sizeof(Container));
  int iCnpj = 0, iWeight = 0, init = 0;
  
  // Verifica se existem divergencias
  processDiff(allContainers, qtd, selectedContainers, qtd_selected, diffCnpj, diffWeight, &iCnpj, &iWeight);

  // Ordena o vetor de divergentes por CNPJ
  Container *temp = malloc(iCnpj * sizeof(Container));
  merge(diffCnpj, temp, init, iCnpj - 1, 1);
  
  // Ordena o vetor de divergentes por peso
  Container *tempWeight = malloc(iWeight * sizeof(Container));
  merge(diffWeight, tempWeight, init, iWeight - 1, 2);

  // Printa os dois vetores no arquivo de saida
  /*for (int i = 0; i < iCnpj; i++) {
    printf("Code: %s, CNPJ: %s, Error: %s\n", diffCnpj[i].code, diffCnpj[i].cnpj, diffCnpj[i].error);
  }*/
  print(output, diffCnpj, iCnpj, 1);
  print(output, diffWeight, iWeight, 2);

  free(tempWeight);
  free(temp);
  free(diffWeight);
  free(diffCnpj);
  free(selectedContainers);
  free(allContainers);
  free(auxTemp);

  fclose(input);
  fclose(output);

  clock_t fim = clock(); 
    double tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("O código levou %.6f segundos para executar.\n", tempo_gasto);
  return 0;

}
