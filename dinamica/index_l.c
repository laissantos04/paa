#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // Biblioteca para medir o tempo de execução

// Definição da struct do pacote
typedef struct package
{
    char code[20];
    int weight;
    int volume;
    double value;
    int selected;
    int index;
} Package;

// Definição da struct do veículo
typedef struct vehicle
{
    char plateNumber[8];
    int weight;
    int volume;
    int count;
    int totalVolume;
    double totalValue;
    int totalWeight;
    Package *selected;
} Vehicle;

typedef enum
{
    VEHICLE,
    PACKAGE
} DataType;

void readInput(void *array, FILE *input, int n, DataType type)
{
    for (int i = 0; i < n; i++)
    {
        if (type == VEHICLE)
        {
            Vehicle *vehicles = (Vehicle *)array;
            fscanf(input, "%s %d %d", vehicles[i].plateNumber, &vehicles[i].weight, &vehicles[i].volume);
            vehicles[i].selected = NULL;
            vehicles[i].count = 0;
            vehicles[i].totalValue = 0;
            vehicles[i].totalVolume = 0;
            vehicles[i].totalWeight = 0;
        }
        else if (type == PACKAGE)
        {
            Package *packages = (Package *)array;
            fscanf(input, "%s %lf %d %d", packages[i].code, &packages[i].value, &packages[i].weight, &packages[i].volume);
            packages[i].selected = 0;
            packages[i].index = i;
        }
    }
}

double max(double a, double b)
{
    return (a > b) ? a : b;
}

void printRemainingPackages(Package *packages, int qtdPackages, FILE *output)
{
    fprintf(output, "PENDENTE:");
    int totalW = 0, totalV = 0;
    double totalValue = 0;

    for (int i = 0; i < qtdPackages; i++)
    {
        totalValue += packages[i].value;
        totalV += packages[i].volume;
        totalW += packages[i].weight;
    }

    fprintf(output, "R$%.2f,%dKG,%dL", totalValue, totalW, totalV);

    for (int i = 0; i < qtdPackages; i++)
    {
        if (i == 0)
            fprintf(output, "->");

        if (i + 1 < qtdPackages)
        {
            fprintf(output, ",");
        }

        fprintf(output, "%s", packages[i].code);
    }

    fprintf(output, "\n");
}

void addPackage(Vehicle *vehicle, Package *packages, int index)
{
    vehicle->selected = realloc(vehicle->selected, (vehicle->count + 1) * sizeof(Package));

    vehicle->selected[vehicle->count] = packages[index];
    vehicle->count++;
    vehicle->totalWeight += packages[index].weight;
    vehicle->totalVolume += packages[index].volume;
    vehicle->totalValue += packages[index].value;

    packages[index].selected = 1;
}

void removePackage(Package *packages, int *qtdPackages)
{
    int rest = 0;
    for (int i = 0; i < (*qtdPackages); i++)
    {
        if (packages[i].selected)
        {
            if (i + 1 < (*qtdPackages))
            {
                packages[i] = packages[*qtdPackages - 1];
            }

            rest += 1;
        }
    }
    (*qtdPackages) -= rest;
}

void checkPackage(Vehicle *vehicle, Package *packages, int *qtdPackages)
{
    int V = vehicle->volume, W = vehicle->weight;
    double ***matriz = (double ***)malloc(((*qtdPackages) + 1) * sizeof(double **));

    for (int i = 0; i <= (*qtdPackages); i++)
    {
        matriz[i] = (double **)malloc((W + 1) * sizeof(double *));
        for (int j = 0; j <= W; j++)
        {
            matriz[i][j] = (double *)calloc(V + 1, sizeof(double));
        }
    }

    for (int i = 1; i <= (*qtdPackages); i++)
    {
        if (packages[i - 1].selected == 1)
            continue;

        for (int j = 0; j <= W; j++)
        {
            for (int k = 0; k <= V; k++)
            {
                if (packages[i - 1].weight > j || packages[i - 1].volume > k)
                {
                    matriz[i][j][k] = matriz[i - 1][j][k];
                }
                else
                {
                    matriz[i][j][k] = max(matriz[i - 1][j][k],
                                          matriz[i - 1][j - packages[i - 1].weight][k - packages[i - 1].volume] + packages[i - 1].value);
                }
            }
        }
    }

    int j = W, k = V;
    for (int i = (*qtdPackages); i > 0; i--)
    {
        if (packages[i - 1].selected == 1)
            continue;
        if (matriz[i][j][k] != matriz[i - 1][j][k])
        {
            addPackage(vehicle, packages, i - 1);
            j -= packages[i - 1].weight;
            k -= packages[i - 1].volume;
        }
    }

    removePackage(packages, qtdPackages);

    for (int i = 0; i <= (*qtdPackages); i++)
    {
        for (int j = 0; j <= W; j++)
        {
            free(matriz[i][j]);
        }
        free(matriz[i]);
    }
    free(matriz);
}

void sortElements(Package *packages, int n)
{
    for (int i = 0; i < n; i++)
    {
        int smaller = i;

        for (int j = i + 1; j < n; j++)
        {
            if (packages[smaller].index > packages[j].index)
            {
                smaller = j;
            }
        }

        Package aux = packages[i];
        packages[i] = packages[smaller];
        packages[smaller] = aux;
    }
}

int main(int argc, char *argv[])
{

    clock_t start = clock();
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");

    if (input == NULL || output == NULL)
    {
        fprintf(stderr, "Erro ao abrir os arquivos.\n");
        return 1;
    }

    int qtdVehicle;
    fscanf(input, "%d", &qtdVehicle);
    Vehicle *vehicles = malloc(qtdVehicle * sizeof(Vehicle));
    readInput(vehicles, input, qtdVehicle, VEHICLE);

    int qtdPackages;
    fscanf(input, "%d", &qtdPackages);
    Package *packages = malloc(qtdPackages * sizeof(Package));
    readInput(packages, input, qtdPackages, PACKAGE);

    for (int i = 0; i < qtdVehicle; i++)
    {
        checkPackage(&vehicles[i], packages, &qtdPackages);
        int weightPercent = (vehicles[i].totalWeight * 100) / vehicles[i].weight;
        int volumePercent = (vehicles[i].totalVolume * 100) / vehicles[i].volume;

        fprintf(output, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->", vehicles[i].plateNumber, vehicles[i].totalValue, vehicles[i].totalWeight, weightPercent, vehicles[i].totalVolume, volumePercent);
        if (vehicles[i].count > 1)
        {
            sortElements(vehicles[i].selected, vehicles[i].count);
        }

        for (int j = 0; j < vehicles[i].count; j++)
        {
            fprintf(output, "%s", vehicles[i].selected[j].code);

            if (j + 1 < vehicles[i].count)
                fprintf(output, ",");
        }

        fprintf(output, "\n");
    }
    printRemainingPackages(packages, qtdPackages, output);

    free(vehicles);
    free(packages);

    fclose(input);
    fclose(output);

    clock_t end = clock();
    double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Tempo de execução: %.3f segundos\n", elapsed_time);

    return 0;
}
