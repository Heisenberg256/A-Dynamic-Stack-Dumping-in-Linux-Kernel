#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <errno.h>
#include <pthread.h>

//for grading purpose.
//note: I have written the test program according to the symbols below.
//if you decide to change the symbols, please make sure you change the way they are called accordingly before testing.

#ifndef GRADING
#define TEST_SYMBOL1 "sys_open"
#define TEST_SYMBOL2 "sys_close"
#define TEST_SYMBOL3 "invalid_symbol"
#define TEST_SYMBOL4 "sys_time"
#define TEST_DRIVERS "not used in this program"
#endif

//make sure you change the filename according to your environment.
#define TEST_FILE "/home/root/test"

#define INSDUMP 359
#define RMDUMP 360

//for input
struct dumpmode_t {
	unsigned int mode;
};

//to test sibling behaviour
void* sibling(void* input){
	int fd;
	fd = open(TEST_FILE,O_RDWR);
	if(fd<0)
	{
	    printf("SIBLING PROCESS ERROR: Failed to open file %s\n",TEST_FILE);
    }
	printf("SIBLING PROCESS: calling %s to invoke dump_stack\n",TEST_SYMBOL2);
    
    close(fd);

    printf("SIBLING PROCESS: check dmesg. [dump_stack should be printed this time. dumpmode = 1]\n");
	return 0;
}

//to test other process behaviour
void* different_process(void* input){
	int fd;
	printf("OTHER PROCESS: calling %s to invoke dump_stack\n",TEST_SYMBOL1);
	fd = open(TEST_FILE,O_RDWR);
	if(fd<0)
	{
	    printf("OTHER PROCESS ERROR: Failed to open file %s\n",TEST_FILE);
    }
    close(fd);
    printf("OTHER PROCESS: check dmesg. [dump_stack should not be printed this time. dumpmode = 0]\n");
	return 0;
}

//to test other process behaviour
void* different_process_2(void* input){
	int fd;
	printf("OTHER PROCESS: calling %s to invoke dump_stack\n",TEST_SYMBOL2);
	fd = open(TEST_FILE,O_RDWR);
	if(fd<0)
	{
	    printf("OTHER PROCESS ERROR: Failed to open file %s\n",TEST_FILE);
    }
    close(fd);
    printf("OTHER PROCESS: check dmesg. [dump_stack should be printed this time. dumpmode > 1]\n");
	return 0;
}

int main()
{
	struct dumpmode_t d_mode;
	unsigned int dump_id;
	int retval;
	unsigned int invalid_id,pending_removal;
	int fd,status;
	pid_t child_pid;
	pthread_t thread1,thread0,thread2;

	//Testcase 0
	printf("\nUSER: Dumpstack test program \n");
	printf("\nUSER: Testcase 0: insdump with dumpmode 0 (testing for owner process)\n");
	printf("USER: insdump using symbol = %s\n",TEST_SYMBOL1);
	d_mode.mode = 0;

	retval = syscall(INSDUMP, TEST_SYMBOL1, &d_mode);
	if(retval < 0)
	{
		printf("ERROR: while calling INSDUMP at %s\n",TEST_SYMBOL1);
		return -1;
	}
	dump_id = retval;
	printf("USER: INSDUMP successful at %s with dumpid = %u \n",TEST_SYMBOL1,dump_id);

	//Testing with owner process.
	printf("USER: calling %s  (as parent) to invoke dump_stack \n", TEST_SYMBOL1 );
	fd = open(TEST_FILE,O_RDWR);
	if(fd<0)
	{
	    printf("ERROR: Failed to open file %s\n",TEST_FILE);
    }
    close(fd);

    printf("USER: check dmesg for dump_stack\n");

    if(syscall(RMDUMP,dump_id)<0)
    {
		printf("ERROR: while RMDUMP\n");
		return -1;	
	}

	printf("USER: RMDUMP with dumpid = %u success!\n",dump_id);

	//Testcase 1
	printf("\nUSER: Testcase 1: insdump with dumpmode 0 (testing for other process)\n");
	printf("USER: insdump using symbol = %s\n",TEST_SYMBOL1);
	d_mode.mode = 0;

	retval = syscall(INSDUMP, TEST_SYMBOL1, &d_mode);
	if(retval < 0)
	{
		printf("ERROR: while calling INSDUMP at %s\n",TEST_SYMBOL1);
		return -1;
	}
	dump_id = retval;
	printf("USER: INSDUMP successful at %s with dumpid = %u \n",TEST_SYMBOL1,dump_id);

	printf("USER: requesting other process to invoke dump_stack at %s \n", TEST_SYMBOL1);

	//spawning thread to act as other process.
	pthread_create(&thread0,NULL, different_process, (void *) NULL);
	pthread_join(thread0,NULL);
	
    if(syscall(RMDUMP,dump_id)<0)
    {
		printf("ERROR: while RMDUMP\n");
		return -1;	
	}

	printf("USER: RMDUMP with dumpid = %u success!\n",dump_id);

	//Testcase 2
    printf("\nUSER: Testcase 2: insdump with dumpmode 1 (testing for sibling process)\n");
    printf("USER: insdump using symbol = %s\n",TEST_SYMBOL2);
	d_mode.mode = 1;

	retval = syscall(INSDUMP, TEST_SYMBOL2, &d_mode);
	if(retval < 0)
	{
		printf("ERROR: while calling INSDUMP at %s\n",TEST_SYMBOL2);
		return -1;
	}
	dump_id = retval;
	printf("USER: INSDUMP successful at %s with dumpid = %u \n",TEST_SYMBOL2,dump_id);

	printf("USER: requesting sibling process to invoke dump_stack at %s \n", TEST_SYMBOL2);

	//spawning new process(sibling) to test dump_mode = 1
	pthread_create(&thread1,NULL, sibling, (void *) NULL);
	pthread_join(thread1,NULL);
	
    if(syscall(RMDUMP,dump_id)<0)
    {
		printf("ERROR: while RMDUMP\n");
		return -1;	
	}

	printf("USER: RMDUMP with dumpid = %u success!\n",dump_id);

	//Testcase 3
	printf("\nUSER: Testcase 3: insdump with dumpmode 2 (testing for other process)\n");
    printf("USER: insdump using symbol = %s\n",TEST_SYMBOL2);
	d_mode.mode = 2;

	retval = syscall(INSDUMP, TEST_SYMBOL2, &d_mode);
	if(retval < 0)
	{
		printf("ERROR: while calling INSDUMP at %s\n",TEST_SYMBOL2);
		return -1;
	}
	dump_id = retval;
	printf("USER: INSDUMP successful at %s with dumpid = %u \n",TEST_SYMBOL2,dump_id);

	printf("USER: requesting other process to invoke dump_stack at %s \n", TEST_SYMBOL2);

	//spawning new process to test dump_mode > 1
	pthread_create(&thread2,NULL, different_process_2, (void *) NULL);
	pthread_join(thread2,NULL);
	
    if(syscall(RMDUMP,dump_id)<0)
    {
		printf("ERROR: while RMDUMP\n");
		return -1;	
	}

	printf("USER: RMDUMP with dumpid = %u success!\n",dump_id);

	//Testcase 4
	printf("\nUSER: Testcase 4: insdump with INVALID symbol_name\n");
	printf("USER: insdump using symbol = %s\n",TEST_SYMBOL3);
	d_mode.mode = 2;

	retval = syscall(INSDUMP, TEST_SYMBOL3, &d_mode);
	if(retval < 0)
	{
		printf("ERROR: while calling INSDUMP at %s\n",TEST_SYMBOL3);
		printf("ERROR: ivalid symbol_name\n");
	}
	else
	{
		dump_id = retval;
		printf("USER: INSDUMP successful at %s with dumpid = %u \n",TEST_SYMBOL3,dump_id);
		
		if(syscall(RMDUMP,dump_id)<0)
    	{
			printf("ERROR: while RMDUMP\n");
			return -1;	
		}
		printf("USER: RMDUMP with dumpid = %u success!\n",dump_id);
	}

	//Testcase 5
	printf("\nUSER: Testcase 5: RMDUMP with invalid dumpid\n");
	printf("USER: insdump using symbol = %s\n",TEST_SYMBOL4);
	d_mode.mode = 0;

	retval = syscall(INSDUMP, TEST_SYMBOL4, &d_mode);
	if(retval < 0)
	{
		printf("ERROR: while calling INSDUMP at %s\n",TEST_SYMBOL4);
		printf("ERROR: ivalid symbol_name\n");
		return -1;
	}
	dump_id = retval;
	printf("USER: INSDUMP successful at %s with dumpid = %u \n",TEST_SYMBOL4,dump_id);
	//intentionally setting dumpid to random value. for testing error handling.
	pending_removal = retval;
	invalid_id = 999;
	if(syscall(RMDUMP,invalid_id)<0)
	{
		printf("ERROR: while RMDUMP\n");
		printf("ERROR: invalid dump_id\n");
	}
	
	//Testcase 6
	printf("\nUSER: Testcase 6: insdump with dumpmode 1. testing for other processes(non sibling)\n");
    printf("USER: insdump using symbol = %s\n",TEST_SYMBOL2);
	d_mode.mode = 1;

	retval = syscall(INSDUMP, TEST_SYMBOL2, &d_mode);
	if(retval < 0)
	{
		printf("ERROR: while calling INSDUMP at %s\n",TEST_SYMBOL2);
		return -1;
	}
	dump_id = retval;
	printf("USER: INSDUMP successful at %s with dumpid = %u \n",TEST_SYMBOL2,dump_id);

	printf("USER: requesting different process (not_sibling) to invoke dump_stack at %s \n", TEST_SYMBOL2);

	//creating child process to test for dumpmode = 1
	child_pid = fork();

	if(child_pid == 0)
	{	
		//To be executed by child only.
		printf("DIFFERENT PROCESS (CHILD): trying to call %s to invode dump_stack\n",TEST_SYMBOL2);
		fd = open(TEST_FILE,O_RDWR);
		if(fd<0)
		{
		    printf("DIFFERENT PROCESS (CHILD) ERROR: Failed to open file %s\n",TEST_FILE);
	    }
	    close(fd);
	    printf("DIFFERENT PROCESS (CHILD): check dmesg. [dump_stack should not be printed this time. dumpmode = 1]\n");
	    printf("DIFFERENT PROCESS (CHILD): Exiting\n");
	}
	else 
	{
		//To be executed by parent process only.
		//Parent waits for child to complete before proceeding.
		wait(&status);
		if(syscall(RMDUMP,dump_id)<0)
	    {
			printf("ERROR: while RMDUMP\n");
			return -1;	
		}
		printf("USER: RMDUMP with dumpid = %u success!\n",dump_id);

		//Testcase 7
		printf("\nUSER: Testcase 7: testing process termination behaviour\n");

		printf("USER: Now, the dump_stack kprobe with dumpid %u is not removed by test program\n", pending_removal);
		printf("USER: This remaining kprobe will be taken care of (removed) when this process exits.\n");
		printf("USER: Adding more insdumps for testing such termination behaviour\n");

		syscall(INSDUMP, TEST_SYMBOL1, &d_mode);
		syscall(INSDUMP, TEST_SYMBOL2, &d_mode);
		
		printf("USER: INSDUMP at %s and %s.\n",TEST_SYMBOL1,TEST_SYMBOL2);
		printf("\nUSER: Exiting (without removing some dumpstacks/kprobes). Check if they are automatically removed.\n");
	}
	return 0;
}