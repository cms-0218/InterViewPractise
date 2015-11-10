#include <stdio.h>
#include <iostream>
using namespace std;

#include "BTree.h"
#include "Tool.h"



int lengthOfLIS(int* nums, int numsSize) 
{
	int tmpLen;
	int MaxLen = 1;
	for(int i = 0; i < numsSize; i++)
	{
		tmpLen = 1;
		int tmp = nums[i];
		for(int j = i+1; j < numsSize; j++)
		{
			if(nums[j] > tmp)
			{
				tmpLen++;
				tmp = nums[j];
				if(MaxLen < tmpLen)
					MaxLen = tmpLen;
			}
		}
	}
	return MaxLen;
}

int majorityElement(int *nums,int numSize)
{
	int count = 0;
	int major = 0;
	for (int i = 0; i < numSize; i++)
	{
		if (count == 0)
		{
			major = nums[i];
		}
		if (major == nums[i])
		{
			count += 1;
		}
		else
			count -= 1;
	}
	return major;
}



int main()
{

	//string strTest = "2#3#567";
	//vector<string> vecStr = Tool::strSplit(strTest,"#");
	//for (vector<string>::iterator i = vecStr.begin(); i != vecStr.end(); i++)
	//{
	//	cout << *i << endl;
	//}
	//cout << endl;
	BTree *btree = new BTree;
	struct BTNode *t1 = new struct BTNode;
	//struct BTNode *t2 = new struct BTNode;
	
	//cout << "please input the node of the first Tree:" << endl;
	//btree->createBTree(t1);

	//string strRes = btree->serialByPreOrder(t1);

	//BTNode *ret = btree->deserialByPre(strRes);
	
	//cout << strRes << endl;

	//cout << "please input the node of the second Tree" << endl;
	//btree->createBTree(t2);
	//bool isContains = btree->subTreeIsContains(t1,t2);

	//if (isContains)
	//	cout << "contains" << endl;
	//else
	//	cout << "not contains" << endl;

	//btree->PreOrderBTree(t1);
	//cout << endl;
	//btree->MidOrderBTree(t1);
//	cout << endl;
	
//	btree->PostOrderBTree(t1);
//	cout << endl;

	const int numsSize = 8; 
	//int nums[numsSize] = {10,9,2,5,3,7,101,18};
	//int iret = lengthOfLIS(nums,numsSize);

	int nums[numsSize] = {1,2,1,1,2,1,2,1};
	int iret = majorityElement(nums,numsSize);

	return 0;
}