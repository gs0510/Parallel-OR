/*
		BATCH 10
		Gargi Sharma 2013A7PS161P
		Ayushi Agarwal 2013A7PS065P

*/
#ifndef _symbolTableDef_h
#define _symbolTableDef_h
typedef struct records1 *RPtr;
struct records1{
	char lexName[256];
	char type[256];
	int lineno;
	int offset;
	int status;
};

typedef struct variables1 *VPtr;
struct variables1{
	RPtr r;
	char lexName[256];
	int lineno;
	int size;
	int offset;
	int num_fields;
	char type[256];
	int status;
	int inparam;
	int outparam;
	int ifassigned;
};

typedef struct functions1 *FPtr;
struct functions1{
	VPtr v;
	char functionName[256];
	int lineno;
	int size;
	int inparam;
	int outparam;
	int status;
};

typedef struct symbolTable *SPtr;
struct symbolTable{
	FPtr fp;
	int size;
};

#endif
