#include <string.h>
#include <iostream>
#include <algorithm>
#include <cstdlib>
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
	
	int tmp;
	for(int i = 0; i < size; i++)
	{
		int index = i;
		tmp = nums[i];
		for(int j = i-1; j >= 0 ; j--)
		{
			if( nums[j] > nums[index])
			{
				nums[index] = nums[j];
				nums[j] = tmp;
			}
			index--;
		}
	}
}


int main()
{
	//============生成测试数据===========
	const int size = 50;
	int nums[size];
	for(int i = 0; i < size; i++)
		nums[i] = rand();
	
	cout << "系统随机生成的测试数据如下：" << endl;
	for(int i = 0; i < size; i++)
		cout << nums[i] << "   ";
	cout << endl;
	
	cout << "冒泡排序测试结果如下：" << endl;
	BubbleSort(nums,size);
	for(int i = 0; i < size; i++)
		cout << nums[i] << "   ";
	cout << "============================================" << endl;
	
	cout << "选择排序测试结果如下：" << endl;
	SelectSort(nums,size);
	for(int i = 0; i < size; i++)
		cout << nums[i] << "   ";
	cout << "============================================" << endl;
	
	cout << "插入排序测试结果如下：" << endl;
	InsertSort(nums,size);
	for(int i = 0; i < size; i++)
		cout << nums[i] << "   ";
	cout << "============================================" << endl;
	
	cout << "堆排序测试结果如下：" << endl;
	for(int i = 0; i < size; i++)
		cout << nums[i] << "   ";
	cout << "============================================" << endl;
	
	cout << "快速排序测试结果如下：" << endl;
	for(int i = 0; i < size; i++)
		cout << nums[i] << "   ";
	cout << "============================================" << endl;
	
	cout << "归并排序测试结果如下：" << endl;
	for(int i = 0; i < size; i++)
		cout << nums[i] << "   ";
	cout << "============================================" << endl;
	
	
	cout << "希尔排序测试结果如下：" << endl;
	for(int i = 0; i < size; i++)
		cout << nums[i] << "   ";
	cout << "============================================" << endl;
	
	
	return 0;
}


