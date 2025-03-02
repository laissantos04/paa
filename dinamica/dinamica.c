#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Definição da struct do pacote
typedef struct package {
  char *code;
  int weight;
  int volume;
  double value;
  int selected;
} Package;

// Definição da struct do veículo
typedef struct vehicle {
  char *plateNumber;
  int weight;
  int volume;
  int totalVolume;
  double totalValue;
  int totalWeight;
  int count;
  Package *selected;
} Vehicle;

typedef enum {VEHICLE, PACKAGE} DataType;

// Ler os dados do arquivo de entrada e salvar no vetor de veículos
void readInput(void *array, FILE *input, int n, DataType type) {
  for (int i = 0; i < n; i++) {
    if (type == VEHICLE) {
      Vehicle *vehicles = (Vehicle *)array;
      vehicles[i].plateNumber = malloc(8 * sizeof(char));
      vehicles[i].selected = NULL;
      vehicles[i].count = 0;
      fscanf(input, "%s %d %d", vehicles[i].plateNumber, &vehicles[i].weight, &vehicles[i].volume);
    } else if (type == PACKAGE) {
        Package *packages = (Package *)array;
        packages[i].code = malloc(20 * sizeof(char));
        fscanf(input, "%s %lf %d %d", packages[i].code, &packages[i].value, &packages[i].weight, &packages[i].volume);
    }
  } 
}

// Adicionar um pacote na lista de pacotes selecionados
void addPackage(Vehicle *vehicle, Package *packages, int index) {
    vehicle->selected = realloc(vehicle->selected, (vehicle->count + 1) * sizeof(Package));
    vehicle->selected[vehicle->count] = packages[index];
    vehicle->count++;
    vehicle->totalWeight += packages[index].weight;
    vehicle->totalVolume += packages[index].volume;
    vehicle->totalValue += packages[index].value;

    packages[index].selected = 1;
}

// Retona o valor máximo 
double max(double a, double b) {
  return (a > b) ? a : b;
}

void removePackage(Package *packages, int *qtdPackages) {
    int rest = 0;
    for (int i = 0; i < (*qtdPackages); i++) {
        if (packages[i].selected) {
            if (i + 1 < (*qtdPackages)) {
                packages[i] = packages[*qtdPackages - 1];
            }
            rest += 1;
        }
    }
    (*qtdPackages) -= rest;
}
 
// Verifica quais pacotes devem ser incluídos no veículo e retorna a lista de pacotes que foram aceitos
void checkPackage(Vehicle *vehicle, Package *packages, int *N) {

  int V = vehicle->volume, W = vehicle->weight;
  // Inicializa a matriz com 3D
  double ***matriz = (double ***)malloc(((*N) + 1) * sizeof(double**));

  for (int i = 0; i <= (*N); i++) {
    matriz[i] = (double **)malloc((W + 1) * sizeof(double *));

    for (int j = 0; j <= W; j++) {
      matriz[i][j] = (double *)calloc(V+1, sizeof(double));
    }
  }

  for (int i = 1; i <= (*N); i++) {
    if (packages[i-1].selected == 1) 
      continue;

    for (int j = 0; j <= W; j++) {
      for (int k = 0; k <= V; k++) {
        if (packages[i-1].weight > j || packages[i-1].volume > k) {
          matriz[i][j][k] = matriz[i-1][j][k];
        } else {
          matriz[i][j][k] = max(matriz[i-1][j][k],
                                matriz[i-1][j - packages[i-1].weight][k - packages[i-1].volume + packages[i-1].value);
        }
      }
    }
  }

  int j = W, k = V;

  for (int i = (*N); i > 0; i--) {
    if (packages[i-1].selected == 1)
      continue;
    if (matriz[i][j][k] != matriz[i-1][j][k]) {
      addPackage(vehicle, packages, i - 1);
      j -= packages[i-1].weight;
      k -= packages[i-1].volume;
    }
  }

  removePackage(packages, N);

    for (int i = 0; i <= (*N); i++) {
        for (int j = 0; j <= W; j++) {
            free(matriz[i][j]);
        }
        free(matriz[i]);
    }
    free(matriz);
} // fim 

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
  
  // Carrega os dados dos veículos
  int qtdVehicle;
  fscanf(input, "%d", &qtdVehicle);
  Vehicle vehicles[qtdVehicle];
  readInput(vehicles, input, qtdVehicle, VEHICLE);
  
  // Carrega os dados dos pacotes
  int qtdPackages;
  fscanf(input, "%d", &qtdPackages);
  Package packages[qtdPackages];
  readInput(packages, input, qtdPackages, PACKAGE);
  
  for (int i = 0; i < qtdVehicle; i++) {
    checkPackage(&vehicles[i], packages, &qtdPackages);
  }

}
