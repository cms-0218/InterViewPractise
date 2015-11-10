#include "BTree.h"
#include "Tool.h"

BTree::BTree()
{
	pRoot = new struct BTNode;
}

void BTree::createBTree(BTNode *&pRoot)
{
	char data;
	cin >> data;
	if(data == '#' )
		pRoot = NULL;
	else
	{
		pRoot = new BTNode;
		if(pRoot == NULL)
			return;
		pRoot->val = data;
		createBTree(pRoot->pLeft);
		createBTree(pRoot->pRight);
	}
}

bool checkContains(struct BTNode *p,struct BTNode *t2)
{
	if (t2 == NULL)
		return true;
	if (p == NULL || p->val != t2->val)
		return false;
	return checkContains(p->pLeft,t2->pLeft) && checkContains(p->pRight,t2->pRight);
}

bool BTree::subTreeIsContains(struct BTNode *t1,struct BTNode *t2)
{
	return checkContains(t1,t2) || checkContains(t1->pLeft,t2) || checkContains(t2->pRight,t2); 
}

string BTree::serialByPreOrder(BTNode *pRoot)
{
	if(pRoot == NULL)
	{
		return "#!";
	}
	string strRes = pRoot->val + "!";
	strRes += serialByPreOrder(pRoot->pLeft);
	strRes += serialByPreOrder(pRoot->pRight);
	return strRes;
}

BTNode* createBTreeFromStr(vector<string> vecStr)
{
	BTNode *head = new BTNode;
	string str = vecStr.front();
	vecStr.erase(vecStr.begin());
	if (str == "#")
	{
		head = NULL;
		return head;
	}
	head->val = str;
	head->pLeft = createBTreeFromStr(vecStr);
	head->pRight = createBTreeFromStr(vecStr);
	return head;
}

BTNode* BTree::deserialByPre(string strTree)
{
	BTNode *tmp ;
	vector<string> vecStr = Tool::strSplit(strTree,"!");
	tmp = createBTreeFromStr(vecStr);
	return tmp;
}

void BTree::PreOrderBTree(BTNode *pRoot)
{
	if (pRoot == NULL)
		return;
	BTNode *p = pRoot;
	stack<BTNode *> stkBTNode;

	while(!stkBTNode.empty() || p != NULL)
	{
		while(p != NULL)
		{
			stkBTNode.push(p);
			cout << " " << p->val;
			p = p->pLeft;
		}
		BTNode *tmp = stkBTNode.top();
		stkBTNode.pop();
		p = tmp->pRight;
	}
	cout << endl;
}


void BTree::MidOrderBTree(BTNode *pRoot)
{
	if (pRoot == NULL)
		return;
	BTNode *p = pRoot;
	stack<BTNode *> stkBTNode;

	while(!stkBTNode.empty() || p != NULL)
	{
		while(p != NULL)
		{
			stkBTNode.push(p);
			p = p->pLeft;
		}
		p = stkBTNode.top();
		cout << " " << p->val;
		p = p->pRight;
		stkBTNode.pop();
	}
	cout << endl;
}

void BTree::PostOrderBTree(BTNode *pRoot)
{
	if (pRoot == NULL)
		return;
	BTNode *p = pRoot;
	stack<BTNode *> stkBTNode;
	BTNode *tmp = NULL;
	while(!stkBTNode.empty() || p != NULL)
	{
		while(p != NULL)
		{
			stkBTNode.push(p);
			p = p->pLeft;
		}
		p = stkBTNode.top();
		if(p->pRight == NULL || tmp == p)
		{
			cout << ' ' << p->val;
			stkBTNode.pop();
			p = NULL;
		}
		else
		{
			tmp = p;
			p = p->pRight;
		}
	}
	cout << endl;
}


bool BTree::isBalancedofBTree(BTNode *pRoot, int &height)
{
	int leftHeight = 0;
	int rightHeight = 0;
	if (pRoot == NULL)
		height = 0;
	isBalancedofBTree(pRoot->pLeft,leftHeight) + 1;	
	isBalancedofBTree(pRoot->pRight,rightHeight) + 1;




	return true;

}