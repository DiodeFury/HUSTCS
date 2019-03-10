#include "basic.h"
#include "selectvar.h"


inline void deleteElem(int target, intListHead * head) {
	--head->length;
}

inline void recoverElem(int target, intListHead * head) {
	++head->length;
}

DecisionTreeNode * addAssumption(int assumption, BOOL valid, DecisionTreeHead * head) {
	DecisionTreeNode * tempNode = head->firstNode;
	if (tempNode == nullptr) {
		// ��ʱ��û���κ�assumption
		head->firstNode = (DecisionTreeNode *)malloc(sizeof(DecisionTreeNode));

		assert(head->firstNode != nullptr);

		head->firstNode->back = nullptr;
		head->firstNode->next = nullptr;
		head->firstNode->assumption = assumption;
		head->firstNode->valid = valid;
		return head->firstNode;
	}
	while (tempNode->next)
		tempNode = tempNode->next;
	// �ҵ����һ���ڵ�
	tempNode->next = (DecisionTreeNode *)malloc(sizeof(DecisionTreeNode));

	assert(tempNode->next != nullptr);

	tempNode->next->back = tempNode;
	tempNode->next->next = nullptr;
	tempNode->next->assumption = assumption;
	tempNode->next->valid = valid;
	return tempNode->next;
}

bool Process(DecisionTreeNode * node, CNF * cnf) {
	bool flag = TRUE; //��־�Ƿ���ֿ��Ӿ�,TRUEΪû�г��֣�FALSEΪ������

	int assumption = node->assumption;
	int currentClauseIndex = 0;
	intListNode * positiveClauseIndexPtr = nullptr;
	intListNode * negativeClauseIndexPtr = nullptr;



	if (assumption > 0) {
		positiveClauseIndexPtr = cnf->variables[assumption].positive->first;
		negativeClauseIndexPtr = cnf->variables[assumption].negative->first;
		cnf->variables[assumption].handled = TRUE;
	}
	else {
		negativeClauseIndexPtr = cnf->variables[-assumption].positive->first;
		positiveClauseIndexPtr = cnf->variables[-assumption].negative->first;
		cnf->variables[-assumption].handled = TRUE;
	}


	// ��ɾ���Ӿ䡢��ɾ�����֣����ܳ�������ͬʱ������ͬһ���Ӿ�����

	while (positiveClauseIndexPtr)
	{
		currentClauseIndex = positiveClauseIndexPtr->value;
		// ȡ�������Ӿ�ɾ��
		if (!cnf->clauses[currentClauseIndex].hidden) {
			cnf->clauses[currentClauseIndex].hidden = assumption;
		}
		positiveClauseIndexPtr = positiveClauseIndexPtr->next;
	}
	currentClauseIndex = 0;
	while (negativeClauseIndexPtr)
	{
		currentClauseIndex = negativeClauseIndexPtr->value;
		// ȡ�������Ӿ��еĶ�Ӧ����ɾ��
		if (!cnf->clauses[currentClauseIndex].hidden) {
			assert(cnf->clauses[currentClauseIndex].variables->length >= 0);
			deleteElem(-assumption, cnf->clauses[currentClauseIndex].variables);
			--cnf->clauses[currentClauseIndex].length; // �Ӿ䳤�ȼ���
			if (0 == cnf->clauses[currentClauseIndex].length) {
				// ˵�������˿��Ӿ�
				flag = FALSE;
			}
		}
		negativeClauseIndexPtr = negativeClauseIndexPtr->next;
	}
	return flag;
}

void Recover(DecisionTreeNode * node, CNF * cnf) {
	int assumption = node->assumption;
	int currentClauseIndex = 0;
	intListNode * positiveClauseIndexPtr = nullptr;
	intListNode * negativeClauseIndexPtr = nullptr;
	if (assumption > 0) {
		positiveClauseIndexPtr = cnf->variables[assumption].positive->first;
		negativeClauseIndexPtr = cnf->variables[assumption].negative->first;
		cnf->variables[assumption].handled = FALSE; // �ָ�״̬
	}
	else {
		negativeClauseIndexPtr = cnf->variables[-assumption].positive->first;
		positiveClauseIndexPtr = cnf->variables[-assumption].negative->first;
		cnf->variables[-assumption].handled = FALSE;
	}


	// �� Process�෴���Ȼָ����֣��ٻָ�ɾ��

	while (negativeClauseIndexPtr)
	{
		currentClauseIndex = negativeClauseIndexPtr->value;
		// ȡ�������Ӿ��еĶ�Ӧ���ָֻ�
		if (!cnf->clauses[currentClauseIndex].hidden) {
			recoverElem(-assumption, cnf->clauses[currentClauseIndex].variables);
			++cnf->clauses[currentClauseIndex].length;// �Ӿ䳤�Ȼָ�

		}
		negativeClauseIndexPtr = negativeClauseIndexPtr->next;
	}

	while (positiveClauseIndexPtr)
	{
		// ȡ��,����Ӿ��ɾ�����ɵ�ǰassumption���µģ���ָ�
		currentClauseIndex = positiveClauseIndexPtr->value;
		if (cnf->clauses[currentClauseIndex].hidden == assumption)
			cnf->clauses[currentClauseIndex].hidden = 0;

		positiveClauseIndexPtr = positiveClauseIndexPtr->next;
	}

}

DecisionTreeNode * BackTrack(DecisionTreeNode * node, CNF * cnf) {
	while (node->valid && node->back)
	{
		DecisionTreeNode * tempPtr = node;
		Recover(node, cnf);
		node = node->back;
		node->next = nullptr;
		free(tempPtr);
		tempPtr = nullptr;
	}
	if (!node->valid) {
		// ��ʱ����ΪvalidΪFALSE������ѭ����˵���ҵ��˼����
		Recover(node, cnf);
		node->assumption = -node->assumption;
		node->valid = TRUE;
		return node;
	}
	else {
		// ��ʱ����ʹ���ݵ��ʼҲ�޷�ʹCNFΪ�棬����NULL
		return nullptr;
	}
}

bool Satisfied(CNF * cnf) {
	// ���CNF�Ƿ��Ѿ�����
	int flag = TRUE;
	// ������ڻ�δ��ɾ�����Ӿ���˵��û������
	for (int i = cnf->clauseNum; i > 0; --i) {
		if (!cnf->clauses[i].hidden) {
			flag = FALSE;
			break;
		}
	}
	return flag;
}

int SingleRule(CNF * cnf) {
	int result = 0;
	intListNode * tempNode = nullptr;
	for (int i = cnf->clauseNum; i > 0; --i) {
		if (1 == cnf->clauses[i].length && !cnf->clauses[i].hidden) {
			tempNode = cnf->clauses[i].variables->first;
			while (cnf->variables[abs(tempNode->value)].handled)
				tempNode = tempNode->next;
			result = tempNode->value;
			break;
		}
	}
	return result;
}

DecisionTreeHead * DPLL(CNF * cnf) {
	DecisionTreeHead * Head = (DecisionTreeHead *)malloc(sizeof(DecisionTreeHead));

	assert(Head != nullptr);

	DecisionTreeNode * currentTreeNode = nullptr;
	Head->firstNode = nullptr;
	bool flag = TRUE;
	int singleRuleResult = 0;

	int assumption = 0;

	while (!Satisfied(cnf))
	{
		while (flag && (singleRuleResult = SingleRule(cnf))) {
			currentTreeNode = addAssumption(singleRuleResult, TRUE, Head);
			//clock_t start = clock();
			flag = Process(currentTreeNode, cnf);
			//clock_t end = clock();
			//printf("Processtime: %f\n", (double)(end - start));
			// ����
			//printAssumptions(Head);

			//printf("proccess the single rule @@ var = %d\n", singleRuleResult);

			if (!flag)
				break;
		}
		if (flag) {
			// ���ڲ����е��Ӿ���˳�
			//clock_t start = clock();
			assumption = SelectVar(cnf);
			//clock_t end = clock();
			//printf("SelectVar : %f\n ", (double)(end - start));
			if (assumption == 0)
				return Head;
			currentTreeNode = addAssumption(assumption, FALSE, Head);
			//printf("assumption var = %d\n", assumption);

			// ����


			//start = clock();
			flag = Process(currentTreeNode, cnf);
			//end = clock();
			//printf("Processtime: %f\n", (double)(end - start));
			//printAssumptions(Head);
			//printf("process the single rule\n");
		}
		else {
			//  ���ڴ��ڳ�ͻ���˳�
			//printf("BAKETRACK @@ var = %d\n", currentTreeNode->assumption);
			//clock_t start = clock();
			currentTreeNode = BackTrack(currentTreeNode, cnf);
			if (nullptr == currentTreeNode)
				return nullptr;
			//clock_t end = clock();
			//printf("BackTrackTime: %f\n", (double)(end - start));
			//printf("AFTER backtrack @@ var = %d\n", currentTreeNode->assumption);
			//start = clock();
			flag = Process(currentTreeNode, cnf);
			//end = clock();
			//printf("Processtime: %f\n", (double)(end - start));

			// ����
			//printAssumptions(Head);

			//printf("process the single rule\n");
		}
	}
	return Head;
}