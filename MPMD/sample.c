#include <mpi.h>
#include <omp.h>
#include <stdio.h>

#define filename "abc.txt"
int main(int argc, char *argv[]){
  int count;
  FILE *a1;
  int a;
  int num;
int numOfProc,my_rank;
  a1 = fopen(filename,"r");


  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numOfProc);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

  if(my_rank == 0){     
    fscanf(a1,"%d",&num);
    a = num;
    printf("a = %d",a);
  }

  MPI_Finalize();
}