#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void matrixVectorMultiplyOpenMPTiling(int *matrix, int *vector, int *result, int size, int tile_size) {
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        result[i] = 0;
        for (int j = 0; j < size; j++) {
            result[i] += matrix[i * size + j] * vector[j];
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <size>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);

    int *matrix = (int *)malloc(size * size * sizeof(int));
    int *vector = (int *)malloc(size * sizeof(int));
    int *result = (int *)malloc(size * sizeof(int));

    srand(time(NULL));

    // Fill matrix and vector with random values
    for (int i = 0; i < size * size; i++) {
        matrix[i] = rand() % 10;
    }

    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 10; 
    }

    int tile_size = 16; // tile size 

    // Apply tiling to the matrix
    #pragma omp parallel for
    for (int i = 0; i < size; i += tile_size) {
        for (int j = 0; j < size; j += tile_size) {
            matrixVectorMultiplyOpenMPTiling(matrix, vector, result, size, tile_size);
        }
    }

    // Print the result
    printf("Resultant vector:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    // Free allocated memory
    free(matrix);
    free(vector);
    free(result);

    return 0;
}
