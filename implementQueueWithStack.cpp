
/*
	Implement Queue using Stacks

	Implement the following operations of a queue using stacks.

	1、push(x) -- Push element x to the back of queue.
	2、pop() -- Removes the element from in front of queue.
	3、peek() -- Get the front element.
	4、empty() -- Return whether the queue is empty.

	Notes:
	You must use only standard operations of a stack -- which means only push to top, peek/pop from top, size, and is empty operations are valid.
	Depending on your language, stack may not be supported natively. You may simulate a stack by using a list or deque (double-ended queue), as long as you use only standard operations of a stack.
	You may assume that all operations are valid (for example, no pop or peek operations will be called on an empty queue).
	Subscribe to see which companies asked this question

*/
#include <stack>
#include <iostream>

using namespace std;

class Queue 
{
public:
	// Push element x to the back of queue.
	void push(int x) 
	{
		stack_input.push(x);
	}
	// Removes the element from in front of queue.
	void pop(void) 
	{
		peek();
		stack_output.pop();
	}
	// Get the front element.
	int peek(void) 
	{
		if (stack_output.empty())
		{
			while (stack_input.size())
			{
				stack_output.push(stack_input.top());
				stack_input.pop();
			}
		}
		return stack_output.top();
	}

	// Return whether the queue is empty.
	bool empty(void) 
	{
		return stack_input.empty() && stack_output.empty();
	}
private:
	stack<int> stack_input;
	stack<int> stack_output;
};


int main()
{
	return 0;
}
