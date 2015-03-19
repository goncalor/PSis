#include "unity.h"
#include "int_stack.h"

int_stack * s;

void setUp(void)
{
	s = create_stack();
}

void tearDown(void)
{
}

void test_task_creation(){
	TEST_ASSERT_NOT_NULL(s);
	int n = size(s);
	TEST_ASSERT_EQUAL_INT(0,n);

}

void test_push(){	
	TEST_ASSERT_NOT_NULL(s);

	push(s, 8);

	int n = size(s);
	TEST_ASSERT_EQUAL_INT(1, n);

	int sze = s->size;
	int i;

	for(i=0; i<sze+10; i++)
		push(s, i);

	n = size(s);
	TEST_ASSERT_EQUAL_INT(sze, n);
}

void test_top(){	
	TEST_ASSERT_NOT_NULL(s);

	int sze = s->size;
	int i;

	for(i=0; i<sze; i++)
	{
		push(s, i);
		TEST_ASSERT_EQUAL_INT(top(s), i);
	}

}


void test_pop(){	
	TEST_ASSERT_NOT_NULL(s);

	int nxt, n, i, sze = s->size;

	for(i=0; i<sze; i++)
		push(s, i);

	nxt = s->next;

	for(i=0; i<sze+2; i++)
	{
		pop(s);
		nxt--;
		if (nxt>-1)
		{
			TEST_ASSERT_EQUAL_INT(size(s), nxt);
		}
		else
			TEST_ASSERT_EQUAL_INT(size(s), 0);
	}
}

