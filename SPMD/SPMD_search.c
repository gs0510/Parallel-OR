#include <mpi.h>
#include <omp.h>
#include <stdio.h>

#include "SPMD_template.c"

#define MAX_SIZE 1234567

int buffer[MAX_SIZE];
int number_to_search;

int search(long int start,long int end)
{
	//printf("%d ID\n",id );
	//printf("%d ----- %d ------> %d\n",start,end,number_to_search );
	printf("\n");
	long int i;
	for(i=start;i<end;i++)
	{
		if(buffer[i]==number_to_search)
		{
			printf("Number found at the index %ld.\n",i );
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
	int size;
	number_to_search = atoi(argv[2]);
	FILE* fp;
	if(rank==0)
	{
		
		fp=fopen(argv[1],"r");
	}
	while(1){
		if(rank==0){
			int num_values=0;
		//fscanf(fp,"%d",&num_values);
			int i=0;
			while(fscanf(fp,"%d",&buffer[i++])!=EOF&&i<MAX_SIZE)
			{
				num_values++;
			}
			//printf("%d whyyyyyyyyyyy %d\n",num_values,buffer[0] );
			if(num_values==0)
			{
				for(i=1;i<num;i++)
				{
					size=-1;
					MPI_Send(&size,1,MPI_INT,i,1,MPI_COMM_WORLD);

				}
				break;
			}
			if(i<MAX_SIZE)
			{
				size=num_values;
			}
			else{
				size=num_values+1;
			}
			for(i=1;i<num;i++)
			{
				MPI_Send(&size,1,MPI_INT,i,1,MPI_COMM_WORLD);	
				MPI_Send(buffer,size,MPI_INT,i,1,MPI_COMM_WORLD);
			}
			
		}
		else{
			MPI_Status st;
			MPI_Recv(&size,1,MPI_INT,0,1,MPI_COMM_WORLD,&st);
			if(size==-1)
				break;
			MPI_Recv(buffer,size,MPI_INT,0,1,MPI_COMM_WORLD,&st);
		}
		SPMD_template(size,0,search);
		MPI_Barrier(MPI_COMM_WORLD);
	}
	if(rank==0)
	{
		printf("Number not found.\n");
	}	
	MPI_Finalize();
}