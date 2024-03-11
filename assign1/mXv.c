/*
 * Programmer(s) : [k213006 mohib]
 * Date: 
 * Desc: Sequential version of matrix vector multiplication. 
 * Note: you will have to make similar files for other 4 version as explained in assign1.md and a batch file to get running times of all 5 programs.
 */
#include<stdio.h>
#include<stdlib.h>

//dynamic memory allocation function
double **allocateMatrix(int rows,int cols)
{
  double ** matrix=(double **)malloc(rows* sizeof(double *));
  if (matrix==NULL){
    fprintf(stderr,"Memory allocation failed\n");
    exit(EXIT_FAILURE);

  }
  for (int i = 0; i < rows; i++) {
        matrix[i] = (double *)malloc(cols * sizeof(double));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }

    return matrix;


}

void freeMemory(double **matrix,int rows){
  for (int i=0;i<rows;i++){
    free(matrix[i]);

  }
  free(matrix)
}

int main(int argc, char* argv){
  //enter you code here...
}//end main
