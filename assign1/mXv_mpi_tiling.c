#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TILE_SIZE 16

// Function to allocate memory for a matrix
double **allocateMatrix(int rows, int cols) {
    double **matrix = (double **)malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double *)malloc(cols * sizeof(double));
    }
    return matrix;
}

// Function to allocate memory for a vector
double *allocateVector(int size) {
    return (double *)malloc(size * sizeof(double));
}

// Function to free memory allocated for a matrix
void freeMatrix(double **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to free memory allocated for a vector
void freeVector(double *vector) {
    free(vector);
}

// Function to fill a matrix with random values
void fillMatrixRandom(double **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = ((double)rand() / RAND_MAX) * 1000;
        }
    }
}

// Function to fill a vector with random values
void fillVectorRandom(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = ((double)rand() / RAND_MAX) * 1000;
    }
}

// Function to perform matrix-vector multiplication with tiling
void matrixVectorMultiply(double **matrix, double *vector, double *result, int rows, int cols, int rank, int size) {
    int chunk_size = (rows + size - 1) / size;
    int start_row = rank * chunk_size;
    int end_row = (rank + 1) * chunk_size;
    if (end_row > rows) end_row = rows;

    for (int i = start_row; i < end_row; i++) {
        result[i] = 0.0;
        for (int j = 0; j < cols; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int rows, cols;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Parse command line arguments
    if (argc != 3) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <rows> <cols>\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    rows = atoi(argv[1]);
    cols = atoi(argv[2]);

    // Allocate memory for matrix, vector, and result
    double **matrix = allocateMatrix(rows, cols);
    double *vector = allocateVector(cols);
    double *result = allocateVector(rows);

    // Fill matrix and vector with random values
    if (rank == 0) {
        fillMatrixRandom(matrix, rows, cols);
        fillVectorRandom(vector, cols);
    }

    // Broadcast vector data to all processes
    MPI_Bcast(vector, cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Perform matrix-vector multiplication
    matrixVectorMultiply(matrix, vector, result, rows, cols, rank, size);

    // Allocate memory for the receive buffer
    double *recv_buffer = NULL;
    if (rank == 0) {
        recv_buffer = malloc(rows * sizeof(double));
    }

    // Gather results from all processes
    MPI_Gather(result, rows / size, MPI_DOUBLE, recv_buffer, rows / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Print result
    if (rank == 0) {
        printf("Resultant vector:\n");
        for (int i = 0; i < rows; i++) {
            printf("%f ", recv_buffer[i]);
        }
        printf("\n");
    }

    // Free allocated memory
    free(recv_buffer);
    freeMatrix(matrix, rows);
    freeVector(vector);
    freeVector(result);

    // Finalize MPI
    MPI_Finalize();

    return EXIT_SUCCESS;
}
