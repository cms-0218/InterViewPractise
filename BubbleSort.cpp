
void BubbleSort(int *nums,int size)
{
	if(nums == NULL || size < 2)
		return;
	int i,j;
	int tmp;
	for( i = size - 1; i >= 0; i--)
	{
		for( j = i; j >= 0; j--)
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
