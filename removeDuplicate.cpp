/* 
 * Given a sorted array, remove the duplicates in place such that each element appear only once 
 *                                                        and return the new length.
 *
 * Do not allocate extra space for another array, you must do this in place with constant memory.
 *
 * For example,
 * Given input array nums = [1,1,2],
 *
 * Your function should return length = 2, with the first two elements of nums being 1 and 2 respectively.
 *          It doesn't matter what you leave beyond the new length.
 *
 * Subscribe to see which companies asked this question
 *
 */

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

int removeDuplicateFromSortedArray01(int *nums,int size)
{
	int begin = 0;
	for(int i = 1; i < size; i++)
	{
		if(nums[begin] != nums[i])
			nums[++begin] = nums[i];
	}
	return begin+1;
}

int removeDuplicateFromSortedArray02(int *nums,int size)
{
	return distance(nums,unique(nums,nums+size));
}
