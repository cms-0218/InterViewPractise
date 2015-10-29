

void BubleSort(int *nums,int size)
{
	int i,j;
	for(i = 0; i < size; i++)
	{
		for(j = i+1; j < size; j++)
		{
			if(nums[j] < nums[i])
			{
				int tmp = nums[i];
				nums[i] = nums[j];
				nums[j] = tmp;
			}

		}

	}

}
