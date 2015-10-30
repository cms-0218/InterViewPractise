#include <string.h>
#include <iostream>
#include <algorithm>
using namespace std;

void BubbleSort(int *nums,int size)
{
	if(nums == NULL || size < 2)
		return;
	int tmp;
	for(int i = size - 1; i >= 0; i--)
	{
		for(int j = i; j >= 0; j--)
		{
			if( nums[i] < nums[j] )
			{
				tmp = nums[i];
				nums[i] = nums[j];
				nums[j] = tmp;
			}
		}
	}
}

void SelectSort(int *nums,int size)
{
	if(nums == NULL || size < 2)
		return;
	
	int minPos;
	int tmp;
	for(int i = 0; i < size; i++)
	{
		minPos = i;
		int tmp = nums[i];
		for(int j = i+1; j < size; j++)
			if( nums[j] < nums[minPos] )
				minPos = j;
		nums[i] = nums[minPos];
		nums[minPos] = tmp;
	}
}

void InsertSort(int *nums,int size)
{
	if(nums == NULL || size < 2)
		return;
}


