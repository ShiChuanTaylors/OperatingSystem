#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BASE_PROCESSES 2
#define FIFO_PIPE "mypipe.fifo"
#define KEEP_MSG "\tKeep"
#define FWD_MSG "\tForward"

void childProcess1(int pipeParentFd[]);
void childProcess2(int pipeParentFd[]);
void childProcess3(int pipeParentFd[]);

char buffer[128];

int main(int argc, char* argv[] ) {
	//char buffer[128];
	
/*
	if(argc < 3) {
		printf("Usage\n");
	}
*/
	//Creating a pipe with permission 764

	//Permission: 	rwx rw- r--
	//				111 110 100
	//				 7	 6   4

	//int npfd = mkfifo("/tmp/mypipe.pipe", 764);

	//FILE *fd = fopen("/tmp/mypipe.pipe", r+w);


	int pipeFd[2];	
	int msgId;	
	FILE* fp;

	
	int pid, i;		//Initialize first process to ID 1

	/* Create unnamed pipe for the current process to the child */
	if(pipe(pipeFd) == -1) {
		perror("Pipe Error");
	}

	pid = (int)fork();

	if(pid == 0) {
		/* Child 1 section */

		close(pipeFd[1]);		/* Close the write pipe of child */
		childProcess1(pipeFd);

		fp = fopen("child-log.txt", "w");
		printf("Child Process 1: reading from Parent..\n");
		while(read(pipeFd[0], buffer, sizeof(buffer))) {	/* read all the buffers from pipe */
			sscanf(buffer, "%d%[^\n]",&msgId, buffer);		/* Break the string into the ID and message */
			//printf("%d%s\n",msgId,buffer);
			if(msgId == 1) {
				strcat(buffer, KEEP_MSG);
			}
			else {
				strcat(buffer, FWD_MSG);
			}
			printf("%d%s\n",msgId,buffer);
			//fprintf(fp, "%s", buffer);
		}
		printf("I'm Child Process 1, I am reading message: %s\n", buffer);

		close(fp);				/* Close the log file */
		close(pipeFd[0]);		/* Close the read pipe */

		return 0;

	}
	else {
		/* Parent or root section */
		close(pipeFd[0]);		/* Close the read pipe of parent */

		/* Write to Childs */
		fp = fopen("dataProcess1.txt", "r");	/* Open Message File */
		while(fgets(buffer, (sizeof(buffer) + 2), fp)) {			/* Read all the messages from the file */
			write(pipeFd[1], buffer, sizeof(buffer));
		}
			
		//snprintf(buffer, sizeof(buffer), "%s\n" , buffer);
		
		fclose(fp); /* Close file */
			
			
		
		//printf("Parent - Writing \n")
		
		//write(pipeFd[1], buffer, sizeof(buffer));

		/* Fork Child process 3 */
		childProcess3(pipeFd);

		close(pipeFd[1]);		/* Close the write pipe */

		return 0;
	}
        	
	return EXIT_SUCCESS;
}

void childProcess1(int pipeParentFd[]) {

	//char buffer[128];
	/* Declare a pipe for child 1 */
	int pipeC1Fd[2];

	/* Initialize pipe for child 1 */
	if(pipe(pipeC1Fd) == -1) {
		perror("Pipe Error");
	}


	int pid = fork();

	if(pid == 0) {
		/* Child Process 2 section */
		close(pipeC1Fd[1]);		/* Close the write pipe of child */
		childProcess2(pipeC1Fd);

		printf("Child Process 2: reading from Child Process 1..\n");
		read(pipeC1Fd[0], buffer, sizeof(buffer));
		printf("I'm Child Process 2, I am reading message: %s\n", buffer);

		close(pipeC1Fd[0]);		/* Close the read pipe */
	}
	else {
		/* Parent - Child Process 1 section */

		close(pipeC1Fd[0]);		/* Close the read pipe of parent */

		/* Write to Parent */
		printf("Child Process 1 - Writing \n");
		snprintf(buffer, sizeof(buffer), "1\tHello! I'm Child Process 1");
		write(pipeC1Fd[1], buffer, sizeof(buffer));

		close(pipeC1Fd[1]);		/* Close the write pipe */

	}
}


void childProcess2(int pipeParentFd[]) {

	//char buffer[128];
	int namedPipe;
	/* Create a named pipe if is not exist */
	if (access(FIFO_PIPE, F_OK) == -1) {
		printf("Creating named pipe...\n");
		namedPipe = mkfifo(FIFO_PIPE, 0777); 
		if (namedPipe != 0) {
			/* the fifo name */
			/* check if fifo already /* if not then, create the fifo*/
			printf("Could not create fifo %s\n", FIFO_PIPE);
			exit(EXIT_FAILURE); 
		}
		printf("Named pipe created successfully: %s\n",FIFO_PIPE);
	}

	/* Open the named pipe for read only on child process 2 */
	namedPipe = open(FIFO_PIPE, O_WRONLY);
	
	//Writing logs message to each child processes

	printf("Child Process 2- Writing \n");
	snprintf(buffer, sizeof(buffer), "3\tHello from namedpipe..\n");
	write(namedPipe, buffer, sizeof(buffer));

}


void childProcess3(int pipeParentFd[]) {
	
	/* Create the Child process 4 from the Parent */
	/* Fork a new process */
	/* Create a new pipe for child process 4 */
	int pid;
	int pipeC3Fd[2];
	int namedPipe;
	pipe(pipeC3Fd);
	//char buffer[128];
	pid = fork();
	if(pid == 0) {
		/* Child 3 section */
		/* Child 3 section is opposite compare to the child 1, where child writes to parent */

		close(pipeC3Fd[0]);		/* Close the read pipe of child */
		
		/* Write to Parent */
		printf("Child Process 3 - Writing \n");
		snprintf(buffer, sizeof(buffer), "3\tHello from Child Process 3");
		write(pipeC3Fd[1], buffer, sizeof(buffer));


		/* Read from Child Process 3 using the FIFO named pipe */

		/* Create a named pipe if is not exist */
		if (access(FIFO_PIPE, F_OK) == -1) {
			printf("Creating named pipe...\n");
			namedPipe = mkfifo(FIFO_PIPE, 0777); 
			if (namedPipe != 0) {
				/* the fifo name */
				/* check if fifo already /* if not then, create the fifo*/
				printf("Could not create fifo %s\n", FIFO_PIPE);
				exit(EXIT_FAILURE); 
			}
			printf("Named pipe created successfully: %s\n",FIFO_PIPE);
		}

		/* Open the named pipe */
		namedPipe = open(FIFO_PIPE, O_RDONLY);
		
		/* Read from the named pipe */
		read(namedPipe, buffer, sizeof(buffer));
		printf("I'm Child Process 3, I am reading message from a namedpipe: %s\n", buffer);

		close(pipeC3Fd[1]);		/* Close the write pipe */

	}
	else {

		/* Parent or root section */
		close(pipeC3Fd[1]);		/* Close the write pipe of parent */
		printf("Parent Process reading from Child Process 3\n");
		read(pipeC3Fd[0], buffer, sizeof(buffer));
		printf("I'm Parent Process, I am reading message: %s\n", buffer);

		close(pipeC3Fd[0]);		/* Close the read pipe */
		
	}

}