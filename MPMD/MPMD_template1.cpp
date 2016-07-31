#include <mpi.h>
#include <omp.h>	
#include <stdio.h>
#include <map>
#include <stdlib.h>
#include <math.h>
#include <queue>
#include "MPMD1.h"
#define NUM_THREADS 4
typedef struct qNode
{
	int fn_no;
	int initial;
	int end;
}qnode;

using namespace std;
void MPMD_template(queue<qnode> que, void* arr, MPI_Datatype typ,int size,int (*fn_arr[])(void*,int))
{
	int rank,num;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	if(rank==0)
	{
		//printf("%d que sizewee\n",que.size() );
		while(!que.empty())
		{
			int num_procs=num-1;
			int i;
			for(i=1;i<num&&!que.empty();i++)
			{
				qnode qn1 = que.front();
				que.pop();
				MPI_Send(&qn1,1,typ,i,1,MPI_COMM_WORLD);	
				//printf("kyonnnnnnnnn11111111111\n");
			}
			//printf("ddddddddddddonnnnnnnn\n");
			while(1)
			{
				int ans=555,id_no;
				MPI_Status st;
				//if(MPI_Recv(&ans,1,MPI_INT,MPI_ANY_SOURCE,3,MPI_COMM_WORLD,&st)!=MPI_SUCCESS)printf("error in recv 1\n");;
				//printf("------------   %d\n",st.MPI_SOURCE);
				if(MPI_Recv(&id_no,1,MPI_INT,MPI_ANY_SOURCE,3,MPI_COMM_WORLD,&st)!=MPI_SUCCESS)printf("error in recv 2\n");		
				//printf("%d IN id_no %d\n",ans,id_no );
				if(id_no!=-1)
				{
					printf("Rules are not followed by datagram with id %d\n",id_no );
					/*for(i=1;i<num;i++)
					{
						MPI_Status st1;
						MPI_Request req;
						MPI_Send(&id_no,1,MPI_INT,i,2,MPI_COMM_WORLD);
						//MPI_Wait(&req,&st1);	
					}*/
					
				}
				
				else
				{
					//printf("IN -1\n");
					int process_no = st.MPI_SOURCE;
					if(!que.empty())
					{
						//printf("%d///////////////\n",process_no);
						qnode qn1 = que.front();
						que.pop();
						MPI_Send(&qn1,1,typ,process_no,1,MPI_COMM_WORLD);	
						//printf("33333333\n");
					}
					else{
						qnode qn1;
						qn1.fn_no=-1;
						MPI_Send(&qn1,1,typ,process_no,1,MPI_COMM_WORLD);
						num_procs--;
						if(num_procs==0)break;
					}
				}
			}
		}
	}
	else{
		MPI_Status st;
		int fn_no;
		while(1)
		{
			qnode qn2;
			MPI_Recv(&qn2,1,typ,0,1,MPI_COMM_WORLD,&st);
			//printf("ffffffffffff111111111\n");
			if(qn2.fn_no==-1)
				break;
			int i,id;
			fn_no=qn2.fn_no;
			int sz = qn2.end-qn2.initial;
			int div=sz/(NUM_THREADS);
			    #pragma omp parallel num_threads(NUM_THREADS) shared(fn_no,arr,div,sz,qn2,size) private(id,i)
				{   
				    id=omp_get_thread_num();
				    //printf("%d------id\n",id);
				    
				    int initial =(id)*div+qn2.initial;
				    int end=(id+1)*div+qn2.initial;
				    if(id==NUM_THREADS-1)
				        end=qn2.end;
				  
				    for(i=initial;i<end&&i<size;i++)
				    {
					    int uu = (*fn_arr[fn_no])(arr,i);
					    //printf("%duuuuuuuu\n",uu );
					    if(uu==1)
					    {
						    int to_send = get_id(arr,i);
						    //mp[to_send]=1;
						    //if(MPI_Send(&uu,1,MPI_INT,0,3,MPI_COMM_WORLD)!=MPI_SUCCESS)printf("error in send 1\n");
						    if(MPI_Send(&to_send,1,MPI_INT,0,3,MPI_COMM_WORLD)!=MPI_SUCCESS)printf("error in send 2\n");
					    }
					
				    }
			    	#pragma omp barrier
			    }
			    //thread

			//printf("%d---------rank-\n",rank );
			int ab=-1;
			//MPI_Send(&ab,1,MPI_INT,0,3,MPI_COMM_WORLD);
			MPI_Send(&ab,1,MPI_INT,0,3,MPI_COMM_WORLD);	
			//printf("%d**********SENDING -1\n",rank );	
		}
		
	}
}
