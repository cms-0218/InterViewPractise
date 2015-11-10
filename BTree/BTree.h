#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <stack>
using namespace std;

/*
**
**  
**
**
**
**
**
**
*/

struct BTNode
{
	struct BTNode *pLeft;
	struct BTNode *pRight;
	string val;
};


class BTree
{
public:
	struct BTNode *pRoot;
	BTree();
	~BTree();
	void createBTree(BTNode *&pRoot);
	bool subTreeIsContains(struct BTNode *t1,struct BTNode *t2);
	string serialByPreOrder(BTNode *pRoot);
	BTNode* deserialByPre(string strTree);
	void PreOrderBTree(BTNode *pRoot);
	void MidOrderBTree(BTNode *pRoot); 
	void PostOrderBTree(BTNode *pRoot);
	bool isBalancedofBTree(BTNode *pRoot);

};