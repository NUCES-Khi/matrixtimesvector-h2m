
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Dynamic memory allocation function
double **allocateMatrix(int size)
{
    double **matrix = (double **)malloc(size * sizeof(double *));
    if (matrix == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < size; i++)
    {
        matrix[i] = (double *)malloc(size * sizeof(double));
        if (matrix[i] == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    return matrix;
}

double *allocateVector(int size)
{
    double *vector = (double *)malloc(size * sizeof(double));
    if (vector == NULL)
    {
        fprintf(stderr, "Memory allocation of vector cannot be performed\n");
        exit(EXIT_FAILURE);
    }
    return vector;
}

void freeMatrix(double **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

void freeVector(double *vector)
{
    free(vector);
}

void fillMatrixRandom(double **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = ((double)rand() / RAND_MAX) * 1000;
        }
    }
}

void fillVectorRandom(double *vector, int size)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = ((double)rand() / RAND_MAX) * 1000;
    }
}

void matrixVectorMultiply(double **matrix, double *vector, double *result, int size, int start, int end)
{
    for (int i = start; i < end; i++)
    {
        result[i] = 0.0;
        for (int j = 0; j < size; j++)
        {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

void displayVector(double *vector, int size)
{
    printf("[");
    for (int i = 0; i < size; i++)
    {
        printf(", %f ", vector[i]);
    }
    printf(" ]");
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <matrix_size>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int size = atoi(argv[1]); // Get the size from command line
    if (size <= 0)
    {
        fprintf(stderr, "Invalid size\n");
        return EXIT_FAILURE;
    }

    int rank, num_procs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Calculate the number of rows per process
    int rows_per_process = size / num_procs;
    int extra_rows = size % num_procs;

    // Allocate memory for matrix, vector, and result
    double **matrix;
    double *vector;
    double *result = allocateVector(size);

    if (rank == 0)
    {
        matrix = allocateMatrix(size);
        vector = allocateVector(size);
        fillMatrixRandom(matrix, size);
        fillVectorRandom(vector, size);
    }

    // Broadcast vector to all processes
    MPI_Bcast(vector, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Scatter matrix to all processes
    double **local_matrix = allocateMatrix(rows_per_process + (rank < extra_rows ? 1 : 0));
    MPI_Scatter(matrix[0], (rows_per_process + (rank < extra_rows ? 1 : 0)) * size, MPI_DOUBLE, local_matrix[0], (rows_per_process + (rank < extra_rows ? 1 : 0)) * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Perform local matrix-vector multiplication
    matrixVectorMultiply(local_matrix, vector, result, size, 0, rows_per_process + (rank < extra_rows ? 1 : 0));

    // Gather partial results from all processes to the root process
    MPI_Gather(rank == 0 ? MPI_IN_PLACE : result, rows_per_process + (rank < extra_rows ? 1 : 0), MPI_DOUBLE, result, rows_per_process + (rank < extra_rows ? 1 : 0), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Display result in the root process
    if (rank == 0)
    {
        printf("Resultant vector:\n");
        displayVector(result, size);
    }

    // Free allocated memory
    if (rank == 0)
    {
        freeMatrix(matrix, size);
        free(vector);
    }
    freeMatrix(local_matrix, rows_per_process + (rank < extra_rows ? 1 : 0));
    freeVector(result);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
