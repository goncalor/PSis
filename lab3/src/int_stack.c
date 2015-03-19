#include "int_stack.h"
#include <stdlib.h>

int_stack * create_stack()
{
	int_stack *s;

	s = malloc(sizeof(int_stack));

	s->v = malloc(5*sizeof(int));
	s->size = 5;
	s->next = 0;

	return s;
}

int size(int_stack *ptr)
{
	return ptr->next;
}

void push (int_stack *s, int value){

	if(s->next < s->size)
	{
		s->v[s->next] = value;
		s->next++;
	}
}

// returns element on top of the stack
int top (int_stack *s){

	int x = -1; 

	if(s->next > 0)
		x = s->v[s->next-1];

	return x;
}

int pop (int_stack *s){

	int value;

	if(s->next > 0)
	{
		value = s->v[s->next] ;
		s->next--;
	}

	return value;
}
