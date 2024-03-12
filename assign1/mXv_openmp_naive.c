/*
 * Programmer(s) : [k213006 mohib]
 * Date: 
 * Desc: Sequential version of matrix vector multiplication. 
 * Note: you will have to make similar files for other 4 version as explained in assign1.md and a batch file to get running times of all 5 programs.
 */
#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<omp.h>


//dynamic memory allocation function
double **allocateMatrix(int size)
{
  double **matrix = (double **)malloc(size * sizeof(double *));
    if (matrix == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < size; i++) {
        matrix[i] = (double *)malloc(size * sizeof(double));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    return matrix;


}


double *allocateVector(int size) {
    double *vector = (double *)malloc(size * sizeof(double));
    if (vector == NULL) {
        fprintf(stderr, "Memory allocation of vector cannot be performed\n");
        exit(EXIT_FAILURE);
    }
    return vector;
}

void freeMatrix(double **matrix,int size){
  for (int i=0;i<size;i++){
    free(matrix[i]);

  }
  free(matrix);
}

void freeVector(double *vector) {
    free(vector);
}

void fillMatrixRandom(double **matrix,int size){
for (int i=0;i<size;i++){
  for (int j=0;j<size;j++){
    matrix[i][j]=((double) rand()/RAND_MAX)*1000;

  }
}

}




void fillVectorRandom(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = ((double)rand() / RAND_MAX)*1000; 
    }
}

double *matrixVectorMultiply(double **matrix, double *vector, int size) {
    double *result = allocateVector(size);
    #pragma omp parallel for

    for (int i = 0; i < size; i++) {
        result[i] = 0.0;
        for (int j = 0; j < size; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
    return result;
}

void displayVector(double *vector, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf(", %f ", vector[i]);
    }
        printf(" ]");

    printf("\n");
}



int main(int argc, char* argv[]){

if (argc!=2){
  fprintf(stderr,"Usage: %s <matrix_size>\n",argv[0]);
  return EXIT_FAILURE;
}


int size=atoi(argv[1]); //get the size from cli
if (size<=0){
  fprintf(stderr,"Invalid size");
  return EXIT_FAILURE;

}

double **matrix = allocateMatrix(size);
double *vector = allocateVector(size);

fillMatrixRandom(matrix, size);
fillVectorRandom(vector, size);

  double *result = matrixVectorMultiply(matrix, vector, size);

    printf("Resultant vector:\n");
    displayVector(result, size);

    freeMatrix(matrix, size);
    freeVector(vector);
    freeVector(result);

    return EXIT_SUCCESS;



}

