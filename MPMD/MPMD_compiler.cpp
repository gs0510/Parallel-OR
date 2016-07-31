#include <mpi.h>
#include <omp.h>	
#include <stdio.h>
#include <math.h>
#include <queue>
#include "typechecker.c"
#include "semanticAnalyzer.c"
#include <stdlib.h>
#include <string.h>
#include "lexer.c"
#include "parser.c"
#include "lexerDef.h"
#include "symbolTableDef.h"
#include "parserDef.h"
#include "MPMD_template1.cpp"
#include "symbolTable.c"
#define MAX_SIZE_ARR 1234567
using namespace std;

int lineno;

int (*fn_arr[])(void*,int)={rule1,rule2,rule3,rule4,rule5,rule6,rule7};

int get_id(void* hdr1,int i)
{
	struct data* hdr = (struct data*)hdr1;
	return (hdr[i].ast)->id;
}

int main(int argc,char* argv[])
{
	MPI_Init(&argc,&argv);
	int rank,num;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	struct data arr[1000];
	int size=0;
	char filename[512];
	char f2[512];
	strcpy(f2,"grammar.txt");
	//printf("hello\n");
	popGram(f2);
	popFirsts();
	popFollows();
	createParseTable(ru);
	populatesyncset();
	//printf("wjwjwj\n");
	FILE* fp;
	fp=fopen(argv[1],"r");
	while(fscanf(fp,"%s",filename)!=EOF)
	{
		//printf("%s---\n",filename );
		lineno=1;
		tokenInfo * temp = ABC(filename,256);
		int correctParseTree=0;
		tree* ast=parseInputSourceCode(temp,&correctParseTree,size);
		arr[size].ast=ast;
		if(correctParseTree)
		{
			createAST(ast);
			//astnum=printParseTree(ast);
			SPtr symPtr=initializeSymbolTable();
			arr[size].symPtr=symPtr;
			FPtr fglobal=globalTable(symPtr);
			arr[size++].fglobal=fglobal;
			createSymbolTable(ast->root,symPtr,fglobal);
		}
		//printf("kdsmkmsk\n");
	}
	printf("AST's generated\n");
	MPI_Datatype ast_MPI;
	MPI_Datatype entryType[3] = { MPI_INT, MPI_INT, MPI_INT};
	int count[3] = { 1 ,1,1};
	MPI_Aint extent1,extent2;
	MPI_Type_extent(MPI_INT,&extent1);
	MPI_Aint addr[3]      = { 0 , extent1,extent1};
	MPI_Type_struct(1,count,addr,entryType,&ast_MPI);
	MPI_Type_commit(&ast_MPI);

	queue<qnode> q;
	if(rank==0)	
	{
		int no_of_fn=7;
		int i,x,j;
		int MAX=2;
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
	MPMD_template(q,arr,ast_MPI,size,fn_arr);
	MPI_Finalize();
}