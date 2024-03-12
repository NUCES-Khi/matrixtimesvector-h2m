
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void matrixVectorMultiplyOpenMPTiling(int *matrix, int *vector, int *result, int rows, int cols, int tile_size) {
    #pragma omp parallel for
    for (int i = 0; i < rows; i++) {
        result[i] = 0;
        for (int j = 0; j < cols; j++) {
            result[i] += matrix[i * cols + j] * vector[j];
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <matrix_size> <vector_size>\n", argv[0]);
        return 1;
    }

    int matrix_size = atoi(argv[1]);
    int vector_size = atoi(argv[2]);

    int *matrix = (int *)malloc(matrix_size * vector_size * sizeof(int));
    int *vector = (int *)malloc(vector_size * sizeof(int));
    int *result = (int *)malloc(matrix_size * sizeof(int));

    srand(time(NULL));

    // Fill matrix and vector with random values
    for (int i = 0; i < matrix_size * vector_size; i++) {
        matrix[i] = rand() % 10;
    }

    for (int i = 0; i < vector_size; i++) {
        vector[i] = rand() % 10; 
    }

    int tile_size = 16; // tile size 

    // Apply tiling to the matrix
    #pragma omp parallel for
    for (int i = 0; i < matrix_size; i += tile_size) {
        for (int j = 0; j < vector_size; j += tile_size) {
            matrixVectorMultiplyOpenMPTiling(matrix, vector, result, matrix_size, vector_size, tile_size);
        }
    }

    // Print the result
    printf("Resultant vector:\n");
    for (int i = 0; i < matrix_size; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    // Free allocated memory
    free(matrix);
    free(vector);
    free(result);

    return 0;
}
