#ifndef int_stack_H
#define int_stack_H

typedef struct int_stack
{
	int *v;
	int size;	// max size of stack
	int next;	// next available position
} int_stack;

int_stack * create_stack();

int size(int_stack *ptr);

#endif // int_stack_H
