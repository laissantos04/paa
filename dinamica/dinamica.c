#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Definição da struct do pacote
typedef struct package {
  char *code;
  int weight;
  int volume;
  double value;
} Package;

// Definição da struct do veículo
typedef struct vehicle {
  char *plateNumber;
  int weight;
  int volume;
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
void addPackage(Vehicle *vehicle, Package newPackage) {
  vehicle->selected = realloc(vehicle->selected, vehicle->count * sizeof(Package));
  vehicle->selected[vehicle->count] = newPackage;
  vehicle->count++;
}

// Retona o valor máximo 
double max(double a, double b) {
  return (a > b) ? a : b;
}
 
// Verifica quais pacotes devem ser incluídos no veículo e retorna a lista de pacotes que foram aceitos
Package* checkPackage(Vehicle vehicle, Package *packages, int N) {

  int V = vehicle.volume/2, W = vehicle.weight/2;
  // Inicializa a matriz com 3D
  double matriz[N + 1][W + 1][V + 1];

  // Inicializa todos os valores da matriz como zero
  for (int i = 0; i <= N; i++) {
    for (int j = 0; j <= W; j++) {
      for (int k = 0; k <= V; k++) {
        matriz[i][j][k] = 0;
      }
    }
  }

  // Verifica quais os produtos que serão incluídos
  for (int i = 1; i <= N; i++) {
    for (int j = 0; j <= W; j++) {
      for (int k = 0; k <= V; k++) {
        if (packages[i].weight > j || packages[i].volume > k) {
         matriz[i][j][k] = matriz[i-1][j][k];
        } else {
          matriz[i][j][k] = max(matriz[i-1][j][k], matriz[i-1][j - packages[i].weight][k - packages[i].volume] + packages[i].value);
        }
      }
    }
  }

  int j = W, k = V;
  // Localiza os pacotes que fazem parte da solução final e adiciona num vetor
  for (int i = N; i > 0; i--) {
    if (matriz[i][j][k] != matriz[i-1][j][k]) {
      addPackage(&vehicle, packages[i]);
      j -= packages[i-1].weight;
      k -= packages[i-1].volume;
    }
  }

  for (int i = 0; i < vehicle.count; i++) {
    printf("%s %.1f \n", vehicle.selected[i].code, vehicle.selected[i].value);
  }

  return vehicle.selected;

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
    checkPackage(vehicles[i], packages, qtdPackages);
  }

}
