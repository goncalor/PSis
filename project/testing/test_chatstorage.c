#include "unity.h"
#include "chatstorage.h"
#include <stdio.h>

chatdb * cdb;

void setUp(void)
{
	cdb = CSinit();
}

void tearDown(void)
{
	CSdestroy(cdb);
}

void test_db_creation()
{
	TEST_ASSERT_NOT_NULL(cdb);
	int n = CSsize(cdb);
	TEST_ASSERT_EQUAL_INT(0,n);
	
	puts("Passed - Creation of Chat Data Base.");

}

void test_add_msg()
{	
	CSstore(cdb, "test_message");
	int n = CSsize(cdb);
	TEST_ASSERT_EQUAL_INT(1, n);

	puts("Passed - Add a message.");
}

void test_single_query()
{
	CSstore(cdb, "test_message");
	char **messages = CSquery(cdb, 0, 0);	
	TEST_ASSERT_EQUAL_STRING("test_message", messages[0]);

	puts("Passed - Query a single message.");
}


void test_limit()
{
	char message[5];
	int i;	

	for(i = 0; i < 1500; i++)
	{
		sprintf(message, "%d", i);
		CSstore(cdb, message);
	}

	int n = CSsize(cdb);
	TEST_ASSERT_EQUAL_INT(1500, n);	

	puts("Passed - No limit number of messages.");	

}

void test_multiple_query()
{
	char msg[5];
	int i;	

	for(i = 0; i < 1500; i++)
	{
		sprintf(msg, "%d", i);
		CSstore(cdb, msg);
	}

	char **messages = CSquery(cdb, 750, 1250);
	
	for(i = 750; i < 1251; i++)
	{		
		sprintf(msg, "%d", i);
		TEST_ASSERT_EQUAL_STRING(msg, messages[i-750]);
	}

	puts("Passed - Query multiple messages.");	
}

void test_change_length_db()
{
	TEST_ASSERT_EQUAL_INT(CS_STEP, CSlength(cdb));
	
	int i;

	for(i = 0; i < 1030; i++) CSstore(cdb, "test_message");	

	int new_cs_step = 2 * CS_STEP;

	TEST_ASSERT_EQUAL_INT(new_cs_step, CSlength(cdb));

	puts("Passed - Realoc done correctly.");	
}
