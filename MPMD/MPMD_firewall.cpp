#include <mpi.h>
#include <omp.h>	
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <netinet/ip_icmp.h>	//Provides declarations for icmp header
#include <netinet/udp.h>	//Provides declarations for udp header
#include <netinet/tcp.h>	//Provides declarations for tcp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <sys/socket.h>
#include <arpa/inet.h>
#include "MPMD_template1.cpp"

#define MAX_ARR 10000
#define FN_SIZE 20
#define NUM_FN 5
#define Q_CAP 90
#define NUM_THREADS 4
using namespace std;
#include <map>
//typedef int (*generic_fp)(void);


int check_source_address(void* hdr1,int j)
{
	ip_packet* hdr = ((ip_packet*)hdr1);
	char** tokens;
	char int_ip[]="192.168.10.0";
	char ext_ip[]="172.16.51.50";
	char *ch;
	int ip1[4];
	int ip2[4];
	int ip3[4];
	ch = strtok(int_ip, ".");
	int i=0;
	while (ch != NULL) {
		ip1[i++]=atoi(ch);
		ch = strtok(NULL, ".");
	}
	ch = strtok(ext_ip, ".");
	i=0;
	while (ch != NULL) {
		ip2[i++]=atoi(ch);
		ch = strtok(NULL, ".");
	}
	ch = strtok(hdr[j].source, ".");
	i=0;
	while (ch != NULL) {
		ip3[i++]=atoi(ch);
		ch = strtok(NULL, ".");
	}
	for(i=0;i<4;i++)
	{
		if(ip1[i]!=ip2[i])
		{
			if(ip3[i]>ip2[i]&&ip3[i]<ip1[i]||ip3[i]<ip2[i]&&ip3[i]>ip1[i])
			{
				break;
			}
		}
	}
	if(i==4)
		return 1;
	else
		return 0;

}

int check_destination_address(void* hdr1,int j)
{
	ip_packet* hdr = (ip_packet*)hdr1;
	char** tokens;
	char int_ip[]="192.168.10.0";
	char ext_ip[]="172.16.51.50";
	char *ch;
	int ip1[4];
	int ip2[4];
	int ip3[4];
	ch = strtok(int_ip, ".");
	int i=0;
	while (ch != NULL) {
		ip1[i++]=atoi(ch);
		ch = strtok(NULL, ".");
	}
	ch = strtok(ext_ip, ".");
	i=0;
	while (ch != NULL) {
		ip2[i++]=atoi(ch);
		ch = strtok(NULL, ".");
	}
	ch = strtok(hdr[j].dest, ".");
	i=0;
	while (ch != NULL) {
		ip3[i++]=atoi(ch);
		ch = strtok(NULL, ".");
	}
	for(i=0;i<4;i++)
	{
		if(ip1[i]!=ip2[i])
		{
			if(ip3[i]>ip2[i]&&ip3[i]<ip1[i]||ip3[i]<ip2[i]&&ip3[i]>ip1[i])
			{
				break;
			}
		}
	}
	if(i==4)
		return 1;
	else
		return 0;

}
int check_source_and_destination(void* hdr1,int j)
{
	int x=check_source_address(hdr1,j);
	int y=check_destination_address(hdr1,j);
	return x&&y;
}
int check_header_len(void* hdr1,int i)
{
	ip_packet* hdr = (ip_packet*)hdr1;
	if(hdr[i].ihl!=5)
		return 1;
	return 0;
}
int check_protocol(void* hdr1,int i)
{
	ip_packet* hdr = (ip_packet*)hdr1;
	if(hdr[i].protocol!=6)
		return 1;
	else
		return 0;
}
int check_tos(void* hdr1,int i)
{
	ip_packet* hdr = (ip_packet*)hdr1;
	if(hdr[i].tos!=0)
		return 1;
	else
		return 0;
}

int get_id(void* hdr1,int i)
{
	ip_packet* hdr = (ip_packet*)hdr1;
	return hdr[i].id;
}

int read_ip_packets(FILE *fp,ip_packet* iph)
{
	int i;
	//if(iph==NULL)

	///	printf("fddddddddddddd\n");
	if(fscanf(fp,"%d\n",&iph->version)==EOF)return -1;
	fscanf(fp,"%d\n",&iph->ihl);
	fscanf(fp,"%d\n",&iph->tos);
	fscanf(fp,"%d\n",&iph->tot_len);
	fscanf(fp,"%d\n",&iph->id);
	fscanf(fp,"%d\n",&iph->ttl);
	fscanf(fp,"%d\n",&iph->protocol);
	fscanf(fp,"%d\n",&iph->check);
	//printf("gggggggg\n");
	fscanf(fp,"%s\n",(iph->source));
	fscanf(fp,"%s\n",(iph->dest));
	//printf("whyyyyyyyyyyyyyyyy\n");
	return 1;
}
int (*fn_arr[])(void*,int)={check_source_and_destination,check_header_len,check_protocol,check_tos};

int main(int argc,char **argv)
{
	//printf("nananna\n\n");
	MPI_Init(&argc,&argv);
	//printf("111111111nananna11111111111111\n\n");
	int rank,num;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	ip_packet *buffer = (ip_packet*)malloc(MAX_ARR*sizeof(ip_packet));
	MPI_Datatype ip_MPI;
	MPI_Datatype entryType[3] = { MPI_INT, MPI_CHAR, MPI_CHAR};
	int count[3] = { 8 ,20,20};
	MPI_Aint extent1,extent2;
	MPI_Type_extent(MPI_INT,&extent1);
	MPI_Type_extent(MPI_CHAR,&extent2);
	MPI_Aint addr[3]      = { 0 , 8*extent1,20*extent2};
	MPI_Type_struct(1,count,addr,entryType,&ip_MPI);
	MPI_Type_commit(&ip_MPI);
	int size;
	
	ip_packet arr[MAX_ARR];
	int i=0;
	FILE* fp;
	fp=fopen(argv[1],"r");
	//printf("here1\n");
	for(i=0;i<MAX_ARR;i++)
	{
		if(read_ip_packets(fp,&arr[i])==-1)break;
	}
	size=i;
	//printf("%d SIZE\n",size );
	fclose(fp);
	queue<qnode> q;
	int no_of_fn =4; 
	if(rank==0)	
	{
		int i,x,j;
		int MAX=4;
		if(no_of_fn>num)
		{
			x = size/MAX;
		}
		else
		{
			MAX=ceil(num/no_of_fn);
			x = (size/MAX);
		}
		int initial=0;
		for(j=0;j<MAX;j++)
		{
			if(j<MAX-1){
				for(i=0;i<no_of_fn;i++)
				{
					qnode qn;
					qn.fn_no = i;
					qn.initial=initial;
					qn.end=initial+x;
					q.push(qn);

				}
			}
			else
			{
				for(i=0;i<no_of_fn;i++)
				{
					qnode qn;
					qn.fn_no = i;
					qn.initial=initial;
					qn.end=size;
					q.push(qn);

				}
			}
			initial=initial+x;
		}
	}
	MPMD_template(q,arr,ip_MPI,size,fn_arr);	
	//printf("%d meeeeeeeee\n",rank );
	/*if(rank==0)
	{
	    MPI_Abort(MPI_COMM_WORLD,MPI_SUCCESS);
	}*/
	//printf("After abort\n");
	//MPI_Barrier(MPI_COMM_WORLD);
	//MPMD_template();
	MPI_Finalize();
}