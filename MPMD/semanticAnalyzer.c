/*
		BATCH 10
		Gargi Sharma 2013A7PS161P
		Ayushi Agarwal 2013A7PS065P

*/
#include "symbolTableDef.h"
#include "parserDef.h"
//#include "typechecker.c"
#include <string.h>
#include <stdio.h>



char currentFn[30];

void checkifDeclared(treeNode* ast, SPtr spt, FPtr fglobal,int *flag)
{
	treeNode* temp=ast;
	int i;
	FPtr f=spt->fp;
	if(strcmp(ast->NodeSymbol,"<funCallStmt>")==0)
	{
		temp=ast->child;
		while(strcmp(temp->NodeSymbol,"TK_FUNID")){
			temp =temp->next;
		}
		//printf("%s %s 11\n", temp->lexeCurNode,currentFn);
		if(strcmp(temp->lexeCurNode,currentFn)==0){
			printf("Recursion not allowed. Error at lineno %d.\n",temp->lineno );
			*flag=1;
			return;
		}
		for(i=0;i<f->size;i++)
		{
			if(strcmp((f+i)->functionName,temp->lexeCurNode)==0)
			{
				if((f+i)->lineno> temp->lineno)
				{
					printf("%s called before being declared\n",(f+i)->functionName );
					*flag=1;
				}
				return;
			}
		}
		printf("%s function in line no %d called but is not defined\n", temp->lexeCurNode,temp->lineno);
		*flag=1;
		return;
	}
	else if(strcmp(ast->NodeSymbol,"<function>")==0||strcmp(ast->NodeSymbol,"<mainFunction>")==0)
	{
		strcpy(currentFn,ast->child->lexeCurNode);
	}	
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkifDeclared(child,spt,fglobal,flag);
		child=child->next;
	}
}

int rule1(void* data2,int a)
{
	struct data *data1 = (struct data*)data2;
	int var=0;
	treeNode* ast = (data1+a)->ast->root;
	SPtr spt = (data1+a)->symPtr;
	FPtr fglobal = (data1+a)->fglobal;
	checkifDeclared(ast,spt,fglobal,&var);
	return var;
}

void checkInput(treeNode* ast,SPtr spt,FPtr fglobal,char fname[],int* cin,int* num,int *lno,int *flag){
	FPtr f = spt->fp;
	int i,j;
	//printf("%s hiii\n",ast->lexeCurNode );
	if(strcmp(ast->NodeSymbol,"TK_ID")==0){
		*lno = ast->lineno;
		//printf("%s 11\n",ast->lexeCurNode );
		for(i=0;i<f->size;i++)
		{
			if(strcmp((f+i)->functionName,fname)==0)
			{
					//printf("inside if\n");
					*num=(f+i)->inparam;
					VPtr v = (f+i)->v;
					for(j=0;j<v->size;j++){
						if((v+j)->inparam ==*cin){
							if(ast->SymPtr!=NULL&&strcmp((v+j)->type,ast->SymPtr->type)){
								*flag=1;
								printf(" Signature not matching. Type of input parameter should be %s. Error at lineno %d\n", (v+j)->type,ast->lineno);
							}
							break;
						}
					}
					if(j==v->size)
					{
						*flag=1;
						printf("You have given more parameters. Required Number is %d. Error in line no %d\n",(f+i)->inparam,ast->lineno);
					}
			}
		}
		(*cin)++;
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkInput(child,spt,fglobal,fname,cin,num,lno,flag);
		child=child->next;
	}
	//printf("returning\n");
}

void checkOutput(treeNode* ast,SPtr spt,FPtr fglobal,char fname[],int* cin,int* num,int *lno,int *flag){
	FPtr f = spt->fp;
	int i,j;
	//printf("%s output hiii\n",ast->lexeCurNode );
	if(strcmp(ast->NodeSymbol,"TK_ID")==0){
		*lno = ast->lineno;
		//printf("%s output 11\n",ast->lexeCurNode );
		for(i=0;i<f->size;i++)
		{
			if(strcmp((f+i)->functionName,fname)==0)
			{
					//printf("inside output if\n");
					*num=(f+i)->outparam;
					//printf("%s func\n",(f+i)->functionName );
					VPtr v1 = (f+i)->v;

					//printf("%d-----\n",*cin );
					for(j=0;j<v1->size;j++){
						if((v1+j)->outparam ==*cin){
							//printf("====\n");
							if(ast->SymPtr!=NULL&&strcmp((v1+j)->type,ast->SymPtr->type)){
								*flag=1;
								printf("Signature not matching. Type of output parameter should be %s It is %s Error at lineno %d\n", (v1+j)->type,ast->SymPtr->type,ast->lineno);
							}
							break;
						}
					}
					if(j==v1->size)
					{
						*flag=1;
						printf("You have given more parameters. Required Number is %d. Error in line no %d\n",(f+i)->outparam,ast->lineno);
					}
			}
		}
		(*cin)++;
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkOutput(child,spt,fglobal,fname,cin,num,lno,flag);
		child=child->next;
	}
	//printf("returning\n");
}

void checkInputParameters(treeNode* ast, SPtr spt, FPtr fglobal,int *flag)
{
	treeNode* temp=ast;
	int i;
	FPtr f=spt->fp;		
	char fname[40];

	if(strcmp(ast->NodeSymbol,"<funCallStmt>")==0){
		temp = ast->child;
		treeNode* tempout =temp;
		while(strcmp(temp->NodeSymbol,"TK_FUNID")){
			temp=temp->next;
		}
		strcpy(fname,temp->lexeCurNode);
		
		temp=temp->next;
		//temp=temp->child;
		int cin=1,num=0,lno;
		
		cin=1;
		if(temp->child!=NULL)
			checkInput(temp->child,spt,fglobal,fname,&cin,&num,&lno,flag);
		else
			checkInput(temp,spt,fglobal,fname,&cin,&num,&lno,flag);
		if(num>cin-1){

			if(temp->child!=NULL){
				*flag=1;
				printf(" Signature not matching. The function requires %d input parameters. Error at lineno %d\n", num,lno);
			}
			else
			{
				*flag=1;
				printf(" Signature not matching. The function requires %d input parameters. Error at lineno %d\n", num,lno);
			}
		}
		cin=1;
		if(strcmp(tempout->NodeSymbol,"<outputParameters>")==0){
			if(tempout->child!=NULL)
				checkOutput(tempout->child,spt,fglobal,fname,&cin,&num,&lno,flag);
			else
				checkOutput(tempout,spt,fglobal,fname,&cin,&num,&lno,flag);
		//	printf("%d num cin %d\n", num, cin);
			if(num>cin-1)
			{
				if(tempout->child!=NULL){
					printf("Signature not matching. The function requires %d output parameters. Error at lineno %d\n", num,lno);
					*flag=1;
				}
				else
				{
					*flag=1;
					printf("Signature not matching. The function requires %d output parameters. Error at lineno %d\n", num,lno);
				}	
			}
		}
		else{
			while(strcmp(tempout->NodeSymbol,"TK_FUNID")){
				tempout=tempout->next;
			}
			for(i=0;i<f->size;i++)
			{
				if(strcmp((f+i)->functionName,fname)==0)
				{
						if((f+i)->outparam>0){
							*flag=1;
							printf("Error at line %d. Required number of outputs %d.\n",tempout->lineno,(f+i)->outparam);
						}
				}
			}
		}
		return;
	}

	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkInputParameters(child,spt,fglobal,flag);
		child=child->next;
	}
}

int rule2(void* data2,int a)
{
	struct data* data1 = (struct data*)data2;
	int var=0;
	treeNode* ast = (data1+a)->ast->root;
	SPtr spt = (data1+a)->symPtr;
	FPtr fglobal = (data1+a)->fglobal;
	checkInputParameters(ast,spt,fglobal,&var);
	return var;
}

int findNoOfParams(FPtr f, char fname[]){
	int size=f->size,i;
	for(i=0;i<size;i++){
		if(strcmp((f+i)->functionName,fname)==0){
			return (f+i)->outparam;
		}
	}
	return 0;
}


char* findType(FPtr f, FPtr fglobal,char var[],int noOut){
	int i=0,size=f->size,j;
	while(i<size){
		if(strcmp((f+i)->functionName,var)==0){
			VPtr vp = (f+i)->v;
			int j;
			while(j<size){
				if((vp+j)->outparam==noOut){
					return (vp+j)->type;
				}
				j++;
			}
			break;
		}
		i++;
	}
}

void checkReturn(treeNode* ast, SPtr spt, FPtr fglobal,char fname[],int *cin,int *flag){
	FPtr f = spt->fp;
	int i,j;
	if(strcmp(ast->NodeSymbol,"TK_ID")==0){
		for(i=0;i<f->size;i++)
		{
			if(strcmp((f+i)->functionName,fname)==0)
			{
				VPtr v1 = (f+i)->v;
				for(j=0;j<v1->size;j++){
					if((v1+j)->outparam ==*cin){
						if(ast->SymPtr!=NULL&&strcmp((v1+j)->type,ast->SymPtr->type)){
							*flag=1;
							printf("Signature not matching. Type of output parameter should be %s It is %s Error at lineno %d\n", (v1+j)->type,ast->SymPtr->type,ast->lineno);
						}
						break;
					}
				}
				if(j==v1->size)
				{
					*flag=1;
					printf("You have given more parameters. Required Number is %d. Error in line no %d\n",(f+i)->outparam,ast->lineno);
				}
			}
		}
		(*cin)++;
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkReturn(child,spt,fglobal,fname,cin,flag);
		child=child->next;
	}
}


void checkReturnValues(treeNode* ast, SPtr spt, FPtr fglobal,int * reqNo,char* fname,int *flag){
	treeNode* temp=ast;
	int i;
	FPtr f=spt->fp;		
	if(strcmp(ast->NodeSymbol,"<function>")==0||strcmp(ast->NodeSymbol,"<mainFunction>")==0){
		temp=ast->child;
		strcpy(fname,temp->lexeCurNode);
		//printf("%s\n",fname );
		*reqNo = findNoOfParams(f,fname);
		//printf("%d required pars\n",*reqNo );
	}
	if(strcmp(ast->NodeSymbol,"<returnStmt>")==0){
		//printf("in rettt\n");
		if(ast->child->next==NULL && *reqNo!=0){
			//figure out lineno
			//printf("%s\n",ast->child->lexeCurNode );
			*flag=1;
			printf("%d variables are required to be returned in function %s. Error at line no.\n",*reqNo,fname);
		}
		else if(strcmp(ast->child->next->NodeSymbol,"TK_ID")==0){
			//printf("insiddddddddd %d req no\n", *reqNo);
			if(*reqNo!=1){
				*flag=1;
				printf("%d variables are required to be returned in function %s. Error at line no %d.\n",*reqNo,fname,ast->child->next->lineno );
			}
			else{
				char type1[256];
				char error[256];
				perror("");	
				strcpy(type1,findType(f,fglobal,fname,1));
				//printf("%s\n",type1 );
				if(ast->child->next->SymPtr!=NULL&&strcmp(type1,ast->child->next->SymPtr->type)!=0){
					*flag=1;
					printf("Return type should have been %s. Error at line no %d.\n",type1,ast->child->next->lineno );
				}
			}
		}	
		else{
			int c =1;
			//printf("hfffffffijfj\n");
			checkReturn(ast->child->next->child,spt,fglobal,fname,&c,flag);
		}
	}

	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkReturnValues(child,spt,fglobal,reqNo,fname,flag);
		child=child->next;
	}	
}

void setAssigned(treeNode *ast){
	if(strcmp(ast->NodeSymbol,"TK_ID")==0){
		if(ast->SymPtr!=NULL)ast->SymPtr->ifassigned=1;
	}

	treeNode* child=ast->child;
	while(child!=NULL)
	{
		setAssigned(child);
		child=child->next;
	}
}

void setIfAssigned(treeNode* ast, SPtr spt, FPtr fglobal)
{
	if(strcmp(ast->NodeSymbol,"TK_ASSIGNOP")==0)
	{
		if(strcmp(ast->prev->NodeSymbol,"TK_ID")==0){
			//printf("%s kya\n",ast->prev->lexeCurNode );
			//if(ast->prev->SymPtr->ifassigned==0)printf("nananana\n");
			if(ast->prev->SymPtr!=NULL)ast->prev->SymPtr->ifassigned=1;
		}
		else{
			if(ast->prev->child->SymPtr!=NULL)ast->prev->child->SymPtr->ifassigned=1;
		}
	}
	else if(strcmp(ast->NodeSymbol,"<ioStmt>")==0){
		if(strcmp(ast->child->NodeSymbol,"TK_READ")==0)
		{
			if(strcmp(ast->child->next->NodeSymbol,"TK_ID")==0){
				if(ast->child->next->SymPtr!=NULL)ast->child->next->SymPtr->ifassigned=1;
			}
			else{
				if(ast->child->next->child->SymPtr!=NULL)ast->child->next->child->SymPtr->ifassigned=1;
			}	
		}
	}
	else if(strcmp(ast->NodeSymbol,"<outputParameters>")==0){
		setAssigned(ast->child);
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		setIfAssigned(child,spt,fglobal);
		child=child->next;
	}
}

void checkAssigned(treeNode* ast,int *flag){
	if(strcmp(ast->NodeSymbol,"TK_ID")==0){
		if(ast->SymPtr!=NULL&&ast->SymPtr->ifassigned==0){
			*flag=1;
			printf("Value should be assigned to the variable %s. Error at line no %d.\n",ast->lexeCurNode,ast->lineno);
		}
	}

	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkAssigned(child,flag);
		child=child->next;
	}	
}

void checkIfAssigned(treeNode* ast, SPtr spt, FPtr fglobal,int *flag)
{
	if(strcmp(ast->NodeSymbol,"<returnStmt>")==0){
		if(ast->child->next!=NULL)
		{
			if(strcmp(ast->child->next->NodeSymbol,"TK_ID")==0){
				if(ast->child->next->SymPtr!=NULL&&ast->child->next->SymPtr->ifassigned==0){
					*flag=1;
					printf("Value should be assigned to the variable %s. Error at line no %d.\n",ast->child->next->lexeCurNode,ast->child->next->lineno);
				}
			}
			else{
				//printf("%s 11\n",ast->child->next->lexeCurNode );
				checkAssigned(ast->child->next->child,flag);
			}
		}	
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkIfAssigned(child,spt,fglobal,flag);
		child=child->next;
	}
}

int rule3(void* data2,int a)
{
	struct data* data1 = (struct data*)data2;
	int var=0;
	treeNode* ast = (data1+a)->ast->root;
	SPtr spt = (data1+a)->symPtr;
	FPtr fglobal = (data1+a)->fglobal;
	checkIfAssigned(ast,spt,fglobal,&var);
	return var;
}


int checkwhilest(treeNode* ast,char identifier[]){
	if(strcmp(ast->NodeSymbol,"TK_ID")==0){
		if(!strcmp(ast->lexeCurNode,identifier))
		{
			return 1;
		}
	
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		int x=checkwhilest(child,identifier);
		if(x==0)
			child=child->next;
		else
			return x;
	}	
}
int checkwhile(treeNode* ast,char identifier[])
{
	int condition = 0;

	if(strcmp(ast->NodeSymbol,"TK_ASSIGNOP")==0)
	{
		if(strcmp(ast->prev->NodeSymbol,"TK_ID")==0)
		{
				//printf("noooo\n");
				if(!strcmp(ast->prev->lexeCurNode,identifier))
					return 1;	
		}
		else{
			if(!strcmp(ast->prev->child->lexeCurNode,identifier))
					return 1;
		}
	}
	else if(strcmp(ast->NodeSymbol,"<ioStmt>")==0){
		//printf("in ioStmtsssssss\n");
		if(strcmp(ast->child->NodeSymbol,"TK_READ")==0)
		{
			if(strcmp(ast->child->next->NodeSymbol,"TK_ID")==0){
				//printf("%s---------%s\n", ast->child->next->lexeCurNode,identifier);
				if(!strcmp(ast->child->next->lexeCurNode,identifier)){
					//printf("lalalalla\n");
					return 1;
				}
			}
			else{
				if(!strcmp(ast->child->next->child->lexeCurNode,identifier))
					return 1;
			}	
		}
	}
	else if(strcmp(ast->NodeSymbol,"<outputParameters>")==0){
		//printf("outputttt %s\n",identifier);
		if(checkwhilest(ast->child,identifier))
			return 1;

	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		int x=checkwhile(child,identifier);
		if(x==0)
			child=child->next;
		else
			return x;
	}
	return 0;
}

void checkWhile(treeNode* ast,int *flag)
{
	int whilst,lineno,flag11;
	char var11[256];
	treeNode* temp,*temp1;
	if(!strcmp("<iterativeStmt>",ast->NodeSymbol))
	{			
		whilst = 0;
		temp = ast;
		lineno =ast->child->lineno;
		ast = ast->child->next;
		while(1)
		{
			if(!(strcmp(ast->NodeSymbol,"<ioStmt>")!=0 && strcmp(ast->NodeSymbol,"<iterativeStmt>")!=0 && strcmp(ast->NodeSymbol,"<conditionalStmt>")!=0 && strcmp(ast->NodeSymbol,"TK_ASSIGNOP")!=0 && strcmp(ast->NodeSymbol,"<funCallStmt>")!=0 && strcmp(ast->NodeSymbol,"<stmt>")!=0))break;
			flag11 = 0;
			if(ast->child == NULL)
			{
				while(1)
				{
					if(flag11==1)break;
					//printf("haaloo\n");
					if(strcmp("TK_ID",ast->NodeSymbol)==0)
					{	
					//printf("%s-----\n",ast->lexeCurNode );
						temp1 = ast;
						strcpy(var11,ast->lexeCurNode);
						//printf("1212\n");
						//printf("var11 = %s\n",var11);
						ast = temp;
						//printf("symbol = %s %s\n",ast->NodeSymbol,var11);
						if(checkwhile(ast,var11)){
							whilst = 1;
							//printf("%s in checkWhile\n",var11 );
						}
						if(whilst==1)
							break;
						ast= temp1;
					}
					if(ast->next!=NULL)
					{
						//printf("inda nakdnsndna\n");
						ast=ast->next;
						flag11 = 1;
						//printf("inda nakdnsndna %d\n",flag11);
					}
					else
						ast = ast->parent;
					}
				}
				else
				{
					//printf("inda nakdnsndna \n");
					ast = ast->child;
				}
			}
			if(whilst==0)
			{
				*flag=1;
				printf("None of the variables in the while loop condition gets updated.Error at line number %d.\n",lineno);
			}
			
		}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkWhile(child,flag);
		child=child->next;
	}
}

int rule4(void* data2,int a)
{
	struct data* data1 = (struct data*)data2;
	int var=0;
	treeNode* ast = (data1+a)->ast->root;
	checkWhile(ast,&var);
	return var;
}


void boolCheck(treeNode* ast,FPtr fglobal,int *flag)
{
	if(strcmp(ast->NodeSymbol,"TK_NOT")==0){
		if(strcmp(ast->next->NodeSymbol,"<booleanExpression>")){
			*flag=1;
			printf("LINENO %d Boolean expression expected to the right of TK_NOT\n",ast->lineno );
		}
	}
	if(strcmp(ast->NodeSymbol,"TK_AND")==0||strcmp(ast->NodeSymbol,"TK_OR")==0){
		if(strcmp(ast->prev->NodeSymbol,"<booleanExpression>")!=0){
			*flag=1;
			printf("LINENO %d Boolean Expression expected to the left of %s .\n",ast->lineno,ast->NodeSymbol );
		}
		if(strcmp(ast->next->NodeSymbol,"<booleanExpression>")!=0){
			*flag=1;
			printf("LINENO %d Boolean Expression expected to the right of %s .\n",ast->lineno,ast->NodeSymbol );	
		}
	}
	if(strcmp("<allVar>",ast->NodeSymbol)==0&&ast->next!=NULL)
	{
		//printf("%s bbabab\n",ast->child->child->lexeCurNode );
		char typ1[256];
		char typ2[256];
		strcpy(typ1,getfieldtype(ast->child->child->SymPtr->type,ast->child->child->next->lexeCurNode,fglobal));
		//printf("%s tyo1\n",typ1 );
		treeNode* temp=ast->next->next;
		//if(temp==NULL)printf("haiii\n");
		if(strcmp("TK_RNUM",temp->NodeSymbol)==0 || strcmp("TK_NUM",temp->NodeSymbol)==0 || strcmp("TK_ID",temp->NodeSymbol)==0)
		{
			if(strcmp("TK_RNUM",temp->NodeSymbol)==0 )
			{
				strcpy(typ2,"REAL");
			}
			else if(strcmp("TK_NUM",temp->NodeSymbol)==0)
			{
				strcpy(typ2,"INT");
			}
			else if(temp->SymPtr==NULL){
				strcpy(typ2,"UNKNOWN");
				//printf("%s %s 33\n",temp->lexeCurNode,temp->NodeSymbol );
			}
			else{
				strcpy(typ2,temp->SymPtr->type);
			}
			//printf("%s 111\n",typ2 );
		}
		else
		{
			strcpy(typ2,getfieldtype(temp->child->child->SymPtr->type,temp->child->child->next->lexeCurNode,fglobal));
		}
		//printf("%s %s\n",typ1,typ2 );
		//if(strcmp(typ1,""))
		if(strcmp(typ1,typ2)!=0){
			*flag=1;
			printf("LINENO %d Error. Type on both sides should be same. \n",ast->child->child->lineno);
		}
		if(strcmp(ast->next->NodeSymbol,"TK_LT")&&strcmp(ast->next->NodeSymbol,"TK_LE")==0&&strcmp(ast->next->NodeSymbol,"TK_GT")==0&&strcmp(ast->next->NodeSymbol,"TK_GE")==0&&strcmp(ast->next->NodeSymbol,"TK_LT")==0&&strcmp(ast->next->NodeSymbol,"TK_NE")==0&&strcmp(ast->next->NodeSymbol,"TK_EQ")==0){
			*flag=1;
			printf("LINENO %d Error. Expected relational Operator instead of %s.\n",ast->lineno, ast->next->NodeSymbol);
		}
	}
	else if((strcmp("TK_RNUM",ast->NodeSymbol)==0 || strcmp("TK_NUM",ast->NodeSymbol)==0 || strcmp("TK_ID",ast->NodeSymbol)==0)&&ast->next!=NULL&&ast->next->next!=NULL){
		//treeNode* temp22;
		//printf("hellloooooooo2222 %s\n",ast->lexeCurNode);
		char typ1[256];
		char typ2[256];
		if(strcmp("TK_RNUM",ast->NodeSymbol)==0 )
			{
				strcpy(typ1,"REAL");
			}
			else if(strcmp("TK_NUM",ast->NodeSymbol)==0)
			{
				strcpy(typ1,"INT");
			}
			else if(ast->SymPtr==NULL){
				strcpy(typ1,"UNKNOWN");
				//printf("%s %s 33\n",temp->lexeCurNode,temp->NodeSymbol );
			}
			else{
				strcpy(typ1,ast->SymPtr->type);
			}
		treeNode* temp22=ast->next->next;
		if(strcmp("TK_RNUM",temp22->NodeSymbol)==0 || strcmp("TK_NUM",temp22->NodeSymbol)==0 || strcmp("TK_ID",temp22->NodeSymbol)==0)
		{
			if(strcmp("TK_RNUM",temp22->NodeSymbol)==0 )
			{
				strcpy(typ2,"REAL");
			}
			else if(strcmp("TK_NUM",temp22->NodeSymbol)==0)
			{
				strcpy(typ2,"INT");
			}
			else if(temp22->SymPtr==NULL){
				strcpy(typ2,"UNKNOWN");
				//printf("%s %s 33\n",temp->lexeCurNode,temp->NodeSymbol );
			}
			else{
				strcpy(typ2,temp22->SymPtr->type);
			}
		}
		else
		{
			strcpy(typ2,getfieldtype(temp22->child->child->SymPtr->type,temp22->child->child->next->lexeCurNode,fglobal));
		}
		//printf("%s %s\n",typ1,typ2 );
		//if(strcmp(typ1,""))
		if(strcmp(typ1,typ2)!=0){
			printf("LINENO %d Error. Type on both sides should be same. \n",ast->lineno);
		}
		if(strcmp(ast->next->NodeSymbol,"TK_LT")&&strcmp(ast->next->NodeSymbol,"TK_LE")==0&&strcmp(ast->next->NodeSymbol,"TK_GT")==0&&strcmp(ast->next->NodeSymbol,"TK_GE")==0&&strcmp(ast->next->NodeSymbol,"TK_LT")==0&&strcmp(ast->next->NodeSymbol,"TK_NE")==0&&strcmp(ast->next->NodeSymbol,"TK_EQ")==0){
			printf("LINENO %d Error. Expected relational Operator instead of %s.\n",ast->lineno, ast->next->NodeSymbol);
		}
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		boolCheck(child,fglobal,flag);
		child=child->next;
	} 
}

void booleanCheck(treeNode* ast,FPtr fglobal,int *flag){

	if(strcmp(ast->NodeSymbol,"<iterativeStmt>")==0||strcmp(ast->NodeSymbol,"<conditionalStmt>")==0)
	{
		boolCheck(ast->child->next,fglobal,flag);
	}	
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		booleanCheck(child,fglobal,flag);
		child=child->next;
	}	
}

int rule5(void* data2,int a)
{
	struct data* data1 = (struct data*)data2;
	int var=0;
	treeNode* ast = (data1+a)->ast->root;
	FPtr fglobal = (data1+a)->fglobal;
	booleanCheck(ast,fglobal,&var);
	return var;
}

void recordCheck(treeNode *ast,FPtr fglobal,int *flag){
	if(strcmp(ast->NodeSymbol,"TK_ID")==0)
	{
		if(ast->SymPtr!=NULL&&!(!strcmp(ast->SymPtr->type,"INT")||!strcmp(ast->SymPtr->type,"REAL")||!strcmp(ast->SymPtr->type,"TK_NUM")||!strcmp(ast->SymPtr->type,"TK_RNUM")))
		{
			//printf("%s ast\n",ast->SymPtr->type );
			*flag=1;
			printf("ERROR at line no %d. Records cannot be multiplied or divided.\n",ast->lineno );
		}
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		recordCheck(child,fglobal,flag);
		child=child->next;
	}	
}


void recCheck(treeNode *ast,FPtr fglobal,int *flag){
	if(strcmp(ast->NodeSymbol,"<arithmeticExpression3>")==0){
		recordCheck(ast->child->next,fglobal,flag);
	}

	treeNode* child=ast->child;
	while(child!=NULL)
	{
		recCheck(child,fglobal,flag);
		child=child->next;
	}
}


void readCheck(treeNode *ast,FPtr fglobal,int* flag){
	if(strcmp(ast->NodeSymbol,"TK_READ")==0){
		if(strcmp(ast->next->NodeSymbol,"TK_ID")==0){
			if(strcmp(ast->next->SymPtr->type,"INT")&&strcmp(ast->next->SymPtr->type,"REAL")){
				*flag=1;
				printf("CANNOT READ RECORD. ERROR AT LINENO %d\n",ast->next->lineno );
			}
		}	
	}

	treeNode* child=ast->child;
	while(child!=NULL)
	{
		readCheck(child,fglobal,flag);
		child=child->next;
	}
}

int rule6(void* data2,int a)
{
	struct data* data1 = (struct data*)data2;
	int var=0;
	treeNode* ast = (data1+a)->ast->root;
	SPtr spt = (data1+a)->symPtr;
	FPtr fglobal = (data1+a)->fglobal;
	readCheck(ast,fglobal,&var);
	return var;
}


void checkCon(treeNode *ast,FPtr fglobal,int* flag)
{
	if(!strcmp(ast->NodeSymbol,"TK_ID")){
		if(strcmp(ast->SymPtr->type,"INT")&&strcmp(ast->SymPtr->type,"REAL")){
			*flag=1;
			printf("RECORDS %s cannot be compared. Error at lineno %d\n",ast->lexeCurNode,ast->lineno );
		}
	}
	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkCon(child,fglobal,flag);
		child=child->next;
	}	
}

void checkConditional(treeNode *ast,FPtr fglobal,int* flag){
	if(strcmp(ast->NodeSymbol,"<booleanExpression>")==0){
		checkCon(ast,fglobal,flag);
	}

	treeNode* child=ast->child;
	while(child!=NULL)
	{
		checkConditional(child,fglobal,flag);
		child=child->next;
	}	
}

int rule7(void* data2,int a)
{
	struct data* data1 = (struct data*)data2;
	int var=0;
	treeNode* ast = (data1+a)->ast->root;
	SPtr spt = (data1+a)->symPtr;
	FPtr fglobal = (data1+a)->fglobal;
	checkConditional(ast,fglobal,&var);
	return var;
}