#include <math.h>
#include <stdio.h>
#include <omp.h>
#include <mpi.h>
#define NUM_THREADS 3

int skew_count(int size,int num)
{	
	int a = pow(2,num-1);
	int b = log(((size*a)/(a-1)))*(1.0)/log(2);
	return b;
}

void without_data_skew(long int size,int num,int pid)
{
	int count=size/(num-1);
	int initial=(pid-1)*count;
	long int end;
	int id,ans;
	if(pid==(num-1))
	{
		end=size;
	}
	else
	{
		end=initial+count;
	}
	
	#pragma omp parallel num_threads(NUM_THREADS) shared(initial,end) private(id,ans)
	{
		id=omp_get_thread_num();
		
		int total=end-initial;
		int to_find=ceil(total*(1.0)/4);
		//printf("%d to_find\n",to_find );
		if(id==NUM_THREADS-1)
		{
			//printf("%d-------------%d 11\n",initial+(id)*to_find,end );
			ans=search(initial+(id)*to_find,end);
			//printf("%d nas\n",ans );
		}
		else
		{
			ans=search(initial+(id)*to_find,initial+(id+1)*to_find);
			//printf("%d nas\n",ans );
		}
		if(MPI_Send(&ans,1,MPI_INT,0,1,MPI_COMM_WORLD)==MPI_SUCCESS)
		{
			printf("SUCCESS----------------%d-----%d\n",id,pid);
		}
		//printf("%d %d ------ sending %d\n",pid, id ,ans);
	}
}

void with_data_skew(long int size, int num,int pid)
{	
	int x = skew_count(size,num);
	long int initial;
	long int end=size;
	int i;
	for(i=1;i<pid;i++)
	{
		end -=pow(2,(x-((num-1)-(i-1))));
	}
	initial = end - pow(2,(x-((num-1)-(pid-1))));
	int id,ans;
	if(pid==num-1)
	{
		initial=0;
	}
	
	#pragma omp parallel num_threads(NUM_THREADS) shared(initial,end) private(id,ans)
	{
		id=omp_get_thread_num();
		
		long int total=end-initial;
		long int to_find=ceil(total*(1.0)/4);
		if(id==NUM_THREADS-1)
		{
			ans=search(initial+(id)*to_find,end);
		}
		else
		{
			ans=search(initial+(id)*to_find,initial+(id+1)*to_find);
			//printf("%d nas\n",ans );
		}
		if(MPI_Send(&ans,1,MPI_INT,0,1,MPI_COMM_WORLD)==MPI_SUCCESS)
		{
			//printf("SUCCESS\n");
		}
		//printf("%d %d ------ sending %d\n",pid, id ,ans);
	}	
}



void SPMD_template(long int size,int data_skew,int (*search)(long int,long int))
{
	//printf("hhooo\n");
	//MPI_Init(0,NULL);
	int pid, num;
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	MPI_Status st;
	if(pid==0)
	{
		//printf("11111\n");
		int ans;

		int count=0;
		while(count< NUM_THREADS*(num-1))
		{
			//printf("%d countttttt\n",count );
			MPI_Status status;
			MPI_Recv(&ans,1,MPI_INT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,&status);
			//printf("%d received from %d\n",ans,status.MPI_SOURCE);
			if(ans==1)
			{
				printf("Operation performed successfully.\n" );
				MPI_Abort(MPI_COMM_WORLD,MPI_SUCCESS);
				//printf("INSIDE ANSWER\n");
				//break;
			}
			printf("%d co\n",count );
			count++;
		}
		/*if(count < 4*(num-1))
		{
			printf("dskcnskjnck\n" );
			MPI_Finalize();-
			exit(0);
		}*/
		//printf("yyyyyyyyyyyaaaaaaa\n");
	}
	else
	{
		//printf("eslslse\n");
		if(data_skew==0)
		{
			without_data_skew(size,num,pid);	
		}
		else{
			with_data_skew(size,num,pid);	
		}
	}
	//MPI_Barrier(MPI_COMM_WORLD);
	//MPI_Finalize();
}