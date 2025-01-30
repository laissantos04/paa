#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct var {
  int value, priority;
  char name[3];
} Var;

// Ler os dados de entrada e aloca nos vetores
void readInput(FILE *input, int *v, int qtd) {
    for (int j = 0; j < qtd; j++) {
      fscanf(input, "%d", &v[j]);
    }
}

// Trocar os elementos 
void trocar(int *x, int *y, int *c) {
  int temp = *x;
  *x = *y;
  *y = temp;
  ++(*c);
} 

// Método Hoare Padrão
int hoare(int *v, int i, int j, int *c) {
  int p = v[i];
  int x = i - 1, y = j + 1;

  while(1) {
    do {
      --y;
    } while(v[y] > p); do {
      ++x;
    } while(v[x] < p);

    if (x < y) {
      trocar(&v[x], &v[y], c);
    } else 
      return y;
  }
}

// Método Lomuto Padrão
int lomuto(int *v, int i, int j, int *c) {
  int p = v[j];
  int x = i - 1, y;

  for (y = i; y < j; y++) {
    if (v[y] <= p) {
      trocar(&v[++x], &v[y], c);
    }
  }
  trocar(&v[x+1], &v[j], c);
  return x + 1;
} 


// Encontrar o pivô aleatório
int randP(int *v, int i, int j, int op, int *c) {
  int n = j - i + 1;
  int r = i + (abs(v[i]) % n);
  
  trocar(&v[r], (op == 4) ? &v[i] : &v[j], c);
  
  if (op == 4) {
    return hoare(v, i, j, c);
  } else {
    return lomuto(v, i, j, c);
  }  
}

// Determinar o pivô como a mediana de 3
int mediana(int *v, int i, int j, int op, int *counter) {

  int a = i + (j - i + 1) / 4;
  int b = i + (j - i + 1) / 2;
  int c = i + 3 * (j - i + 1)/4;
  int m;

  if ((v[a] >= v[b] && v[a] <= v[c]) || (v[a] <= v[b] && v[a] >= v[c])) {
    m = a;
  } else if ((v[b] >= v[a] && v[b] <= v[c]) || (v[b] <= v[a] && v[b] >= v[c])) {
    m = b;
  } else {
    m = c;
  }
 
  trocar(&v[m], (op == 2) ? &v[i] : &v[j], counter);

  if (op == 2) {
    return hoare(v, i, j, counter);
  } else {
    return lomuto(v, i, j, counter);
  }
}

// Função quicksort
void quicksort(int *v, int i, int j, int op, int *c) {

  ++(*c);
  if (i < j) {
    int p;
    if (op == 0) {
      p = hoare(v, i, j, c);
      quicksort(v, i, p, op, c);
      quicksort(v, p+1, j, op, c);
      
    } else if (op == 1) {
      p = lomuto(v, i, j, c);
      quicksort(v, i, p-1, op, c);
      quicksort(v, p+1, j, op, c);
    
    } else if (op == 2 || op == 3) {
      p = mediana(v, i, j, op, c);
      quicksort(v, i, (op == 2) ? p : p-1, op, c);
      quicksort(v, p+1, j, op, c);
   
    } else if (op == 4 || op == 5) {
      p = randP(v, i, j, op, c);
      quicksort(v, i, (op == 4) ? p : p-1, op, c);
      quicksort(v, p+1, j, op, c);
    }
  }
}

void callQs(int *copy, int *v, int i, int n, int op, int *var) {
  memcpy(copy, v, n * sizeof(int));
  quicksort(copy, i, n - 1, op, var);
}

void sortVar(Var *allVar) {
  for (int i = 0; i < 7; i++) {
    int index = i;
    
    for (int j = i + 1; j < 7; j++) {
      if (allVar[j].value < allVar[index].value) {
       index = j;
      } else if (allVar[j].value == allVar[index].value) { // Caso os valores sejam iguais, faz o desempate pela prioridade
        if (allVar[j].priority < allVar[index].priority) { 
          index = j;
        }
      }
    }

    Var aux = allVar[i];
    allVar[i] = allVar[index];
    allVar[index] = aux;
  }
}

// Main
int main(int argc, char* argv[]) {
  
    if (argc != 3) {
      printf("Necessário especificar os dois arquivos de entrada.\n");
    return 1;
  }

  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");

  if (input != NULL) {

    int qtd_v;
    fscanf(input, "%d", &qtd_v);

    for (int i = 0; i < qtd_v; i++) {
      int qtd_items;
      Var allVar[7];

      fscanf(input, "%d", &qtd_items);
      int v[qtd_items];
      int copy[qtd_items]; // vetor temporário
      readInput(input, v, qtd_items);
     
      // Inicialização dos valores das variáveis de controle
      // Inicia os valores como zero a cada chamada
      // A prioridade é dada em ordem de menor para maior, ou seja, o menor valor tem maior prioridade
      allVar[0].value = qtd_items;
      allVar[1].value = 0;
      strcpy(allVar[0].name, "N");

      // TODOS Lomuto
      allVar[2].value = 0;      
      callQs(copy, v, 0, qtd_items, 1, &allVar[2].value); // chamando qs para o lomuto padrão
      strcpy(allVar[2].name, "LP");
      allVar[2].priority = 0;

      allVar[4].value = 0;      
      callQs(copy, v, 0, qtd_items, 3, &allVar[4].value); // chamando qs para o lomuto mediana
      strcpy(allVar[4].name, "LM");
      allVar[4].priority = 1;

      allVar[6].value = 0;      
      callQs(copy, v, 0, qtd_items, 5, &allVar[6].value); // chamando qs para o lomuto random
      strcpy(allVar[6].name, "LA");
      allVar[6].priority = 2;

      // TODOS HOARE

      callQs(copy, v, 0, qtd_items, 0, &allVar[1].value); // chamando qs para o hoare padrão
      strcpy(allVar[1].name, "HP");
      allVar[1].priority = 3;

      allVar[3].value = 0;     
      callQs(copy, v, 0, qtd_items, 2, &allVar[3].value); // chamando qs para o hoare mediana
      strcpy(allVar[3].name, "HM");
      allVar[3].priority = 4;

      allVar[5].value = 0;      
      callQs(copy, v, 0, qtd_items, 4, &allVar[5].value); // chamando qs para o hoare random
      strcpy(allVar[5].name, "HA");
      allVar[5].priority = 5;
     
      sortVar(allVar);
        fprintf(output, "%d:N(%d),%s(%d),%s(%d),%s(%d),%s(%d),%s(%d),%s(%d)\n",
               i,allVar[0].value,allVar[1].name,allVar[1].value,allVar[2].name,allVar[2].value,
               allVar[3].name,allVar[3].value,allVar[4].name,allVar[4].value,allVar[5].name,
               allVar[5].value, allVar[6].name,allVar[6].value);
    }
  }

  fclose(input);
  fclose(output);
  return 0;
}
