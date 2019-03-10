#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef _BASIC_H
#define _BASIC_H

#define DISCARD_BUFFER_SIZE 100
#define TRUE 1
#define FALSE 0
#define BOOL int


struct intListNode {
	int value;
	intListNode * next;
};

struct intListHead
{
	intListNode * first;
	int length = 0;
};

struct Var
{
	//int index; // ��Ԫ��ţ�û������
	int positiveCount; // ��������
	int negativeCount; // ��������
	intListHead * positive;
	intListHead * negative;
	BOOL handled; // ��ʾ�ñ�Ԫ�Ƿ��Ѿ�����ֵ�ˣ�����FindMaxʱ�ų�����
};

struct Clause
{
	int length;
	intListHead * variables; // �������Ӿ������б�Ԫ��λ��
	int hidden;  // ָʾ�Ƿ�ɾ�����Լ���ɾ����ԭ��
};

struct CNF {
	int varNum;
	int clauseNum;
	Var * variables;
	Clause * clauses;
};

struct DecisionTreeNode
{
	int assumption;
	BOOL valid;
	DecisionTreeNode * next;
	DecisionTreeNode * back;
};

struct DecisionTreeHead
{
	DecisionTreeNode * firstNode;
};


void addElem(int target, intListHead * head);
void InitIntListHead(CNF * cnf);
int LoadFile(const char * filename, CNF * cnf);
int * TurnToArray(DecisionTreeHead * result, CNF * cnf);
void CheckFinalResult(int * resultArray, CNF * cnf);
void DestroyList(intListHead * head);
void DestroyCNF(CNF * cnf);


#endif 


