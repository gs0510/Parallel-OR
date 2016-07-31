#include <mpi.h>
#include <omp.h>	
#include <stdio.h>
#include <math.h>
#define MAX_SIZE 1234567
#include "SPMD_template.c"

long int start_index,end_index;

int calculate_prime(long int num)
{
	if(num==0||num==1)return 0;
	long int i=0;
	for(i=2;i<=sqrt(num);i++)
	{
		if(num%i==0)
		{
			return 0;
		}
	}
	return 1;
}

int search(long int start,long int end)
{
	int i;
	for(i=start_index+start;i<start_index+end;i++)
	{
		if(calculate_prime(i)){
			printf("%d the number that is prime\n",i );
			return 1;
		}
	}
	return 0;
}


void main(int argc, char* argv[]){
	MPI_Init(&argc,&argv);
	int rank,num;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	long int size;
	if(rank==0){
		FILE* fp;
		fp=fopen(argv[1],"r");
		fscanf(fp,"%ld",&start_index);
		fscanf(fp,"%ld",&end_index);
		int i=0;
		size= end_index-start_index;
		for(i=1;i<num;i++)
		{
			MPI_Send(&start_index,1,MPI_LONG,i,1,MPI_COMM_WORLD);
			MPI_Send(&size,1,MPI_LONG,i,1,MPI_COMM_WORLD);	
		}
		fclose(fp);
		
	}
	else{
		MPI_Status st;
		MPI_Recv(&start_index,1,MPI_LONG,0,1,MPI_COMM_WORLD,&st);
		MPI_Recv(&size,1,MPI_LONG,0,1,MPI_COMM_WORLD,&st);
	}
	SPMD_template(size,0,search);
	if(rank==0)
	{
		printf("No prime number in the given range.\n");
	}
	printf("%d\n",rank );
	MPI_Finalize();
}