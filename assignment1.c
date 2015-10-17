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
#define FWD_MSG "\tForwarded"

/* Process Messages */
#define PP_TXT "messages/ppMsg.txt"
#define C1_TXT "messages/c1Msg.txt"
#define C2_TXT "messages/c2Msg.txt"
#define C3_TXT "messages/c3Msg.txt"

/* Log Messages */
#define PP_LOG "log/ppLog.txt"
#define C1_LOG "log/c1Log.txt"
#define C2_LOG "log/c2Log.txt"
#define C3_LOG "log/c3Log.txt"

/* Logged Messages Status */
int ppStatus = 0;
int c1Status = 0;
int c2Status = 0;
int c3Status = 0;
int roundTurnNo = 0;

/* Unnamed Pipes */
int pipeFd[2];	
int pipeC1Fd[2];
//int pipeC3Fd[2];

void childProcess1(int pipeParentFd[]);
void childProcess2(int pipeParentFd[]);
void childProcess3(int pipeParentFd[]);

void childProcess1Content(int pipeParentFd[]);
void childProcess2Content(int pipeParentFd[]);
void childProcess3Content(int pipeC3Fd[]);
void parentProcessContent();

char buffer[128];			/* Buffer for the message content, with ASCII 128 bit max */
char tmpBuffer[130];		/* Buffer for the whole line message that included MsgID, and the Message */

int main(int argc, char* argv[] ) {

	/* Remove all existing log files */
	/*if (access(PP_LOG, F_OK) == -1) {
		remove(PP_LOG);
	}
	if (access(C1_LOG, F_OK) == -1) {
		remove(C1_LOG);
	}
	if (access(C2_LOG, F_OK) == -1) {
		remove(C2_LOG);
	}
	if (access(C3_LOG, F_OK) == -1) {
		remove(C3_LOG);
	}
	if (access(FIFO_PIPE, F_OK) == -1) {
		remove(FIFO_PIPE);
		printf("Remove FiFO\n");
	}
	*/
	
	int pid;		//Initialize first process to ID 1
	FILE* fp;
	int msgId;	


	/* Create unnamed pipe for the current process to the child */
	if(pipe(pipeFd) == -1) {
		perror("Pipe Error");
	}

	pid = (int)fork();

	if(pid == 0) {
		/* Child 1 section */

		close(pipeFd[1]);		/* Close the write pipe of child */
		childProcess1(pipeFd);
		//returnToParentProcess(pipeFd);
		return 0;

	}
	else {
		/* Parent or root section */
		close(pipeFd[0]);		/* Close the read pipe of parent */
		//wait(&pid);

		if(ppStatus == 0) {
			/* Write to Childs */
			fp = fopen(PP_TXT, "r");	/* Open Message File */
			while(fgets(tmpBuffer, (sizeof(tmpBuffer) + 2), fp)) {			/* Read all the messages from the file */
				write(pipeFd[1], tmpBuffer, sizeof(tmpBuffer));
			}
			ppStatus = 1;		/* File is logged */
			//snprintf(buffer, sizeof(buffer), "%s\n" , buffer);
			
			fclose(fp); /* Close file */	
			/* Read Messages from Parent Process of Child 3 using named pipe */
		
		}
		//parentProcessContent();
			
		
		//printf("Parent - Writing \n")
		
		//write(pipeFd[1], buffer, sizeof(buffer));

		/* Fork Child process 3 */
		//wait(&pid);
		childProcess3(pipeFd);

		//close(pipeFd[1]);		/* Close the write pipe */

		return 0;
	}
        	
	return EXIT_SUCCESS;
}

void parentProcessContent() {
	// FILE* fp;
	// int msgId;	

	

	// if(roundTurnNo >= 2) {				/* If this is the second round trip of the processes */
	// 	return;
	// } else {							/* If the round trip of the processes is none or less than one */
	// 	fp = fopen(PP_LOG, "a+");					/* Open a log file */
	// 	printf("Parent Process 1: reading from Child 3..\n");
	// 	while(read(pipeC3Fd[0], tmpBuffer, sizeof(tmpBuffer))) {	/* Read all the buffers from pipe */
	// 		sscanf(tmpBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
	// 		//printf("%d%s\n",msgId,buffer);
	// 		if(msgId == 4) {											/* Message is belongs to the child */
	// 			strcat(buffer, KEEP_MSG);								/* Append message will "keep" word*/
	// 		} else {													/* Message is NOT belongs to the child */
	// 			write(pipeFd[1], tmpBuffer, sizeof(tmpBuffer));			/* Forward the message to next child */
	// 			strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
	// 		}
	// 		//printf("%d%s\n",msgId,buffer);
	// 		fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
	// 	}
	// 	fclose(fp);
	// 	//childProcess1Content(pipeFd);
//	}
}

void childProcess1(int pipeParentFd[]) {

	//char buffer[128];
	/* Declare a pipe for child 1 */
	if(c1Status == 0) {

		/* Initialize pipe for child 1 */
		if(pipe(pipeC1Fd) == -1) {
			perror("Pipe Error");
		}


		int pid = fork();

		if(pid == 0) {
			/* Child Process 2 section */
			close(pipeC1Fd[1]);		/* Close the write pipe of child */
//			wait(&pid);
			//childProcess2(pipeC1Fd);

			//printf("Child Process 2: reading from Child Process 1..\n");
			//read(pipeC1Fd[0], buffer, sizeof(buffer));
			//printf("I'm Child Process 2, I am reading message: %s\n", buffer);

			// fp = fopen(C2_LOG, "w");					/* Open a log file */
			// printf("Child Process 2: reading from Child Process 1..\n");
			// while(read(pipeC1Fd[0], buffer, sizeof(buffer))) {	/* Read all the buffers from pipe */
			// 	sscanf(buffer, "%d%[^\n]",&msgId, buffer);		/* Break the message into the ID and message */
			// 	//printf("%d%s\n",msgId,buffer);
			// 	if(msgId == 3) {								 Message is belongs to the child 
			// 		strcat(buffer, KEEP_MSG);					/* Append message will "keep" word*/
			// 	} else {										/* Message is NOT belongs to the child */
			// 		strcat(buffer, FWD_MSG);					/* Append message will "forward" word*/
			// 	}
			// 	//printf("%d%s\n",msgId,buffer);
			// 	fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
			// }

			//close(pipeC1Fd[0]);		/* Close the read pipe */
		}
		else {
			/* Parent - Child Process 1 section */

			close(pipeC1Fd[0]);		/* Close the read pipe of parent */

			/*printf("Child Process 1 - Writing \n");
			snprintf(buffer, sizeof(buffer), "1\tHello! I'm Child Process 1");
			write(pipeC1Fd[1], buffer, sizeof(buffer));
	*/	
		
			childProcess1Content(pipeParentFd);	
			//childProcess2(pipeC1Fd);

			//close(pipeC1Fd[1]);		/* Close the write pipe */
			//close(pipeParentFd[0]);		/* Close the read pipe */

		}
		c1Status = 1;		/* File is logged */
	}
	else {
		childProcess1Content(pipeParentFd);
	}

}

void childProcess1Content(int pipeParentFd[]) {
	int msgId;
	FILE* fp;
	/* Read Messages from Parent Process of Child 1 */
	printf("Open Children 1 Log File..\n");
	fp = fopen(C1_LOG, "a+");					/* Open a log file */
	printf("Child Process 1: reading from Parent..\n");
	while(read(pipeParentFd[0], tmpBuffer, sizeof(tmpBuffer))) {	/* Read all the buffers from pipe */
		
		//printf("%s\n",tmpBuffer);
		sscanf(tmpBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
		//printf("%d%s\n",msgId,buffer);
		if(msgId == 2) {											/* Message is belongs to the child */
			strcat(buffer, KEEP_MSG);								/* Append message will "keep" word*/
		} else {													/* Message is NOT belongs to the child */
			write(pipeC1Fd[1], tmpBuffer, sizeof(tmpBuffer));		/* Forward the message to next child */

			strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
		}
		//printf("%d%s\n",msgId,buffer);
		fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
		printf("DONE!\n");
	}
	//printf("I'm Child Process 1, I am reading message: %s\n", buffer);
	printf("Children 1 Log FIle saved successfully\n");
	fclose(fp);

	if(c1Status == 0) {
		/* Write to Childs from Message File*/
		fp = fopen(C1_TXT, "r");	/* Open Message File */
		while(fgets(tmpBuffer, (sizeof(tmpBuffer) + 2), fp)) {			/* Read all the messages from the file */
			write(pipeC1Fd[1], tmpBuffer, sizeof(tmpBuffer));
			
		}
		c1Status = 1;		/* File is logged */
		fclose(fp);	
	}
	
}

void childProcess2(int pipeParentFd[]) {
	int msgId;
	//char buffer[128];
	int namedPipe;
	FILE* fp;

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
	
	/* Writing logs message to each child processes */
	/*printf("Child Process 2- Writing \n");
	snprintf(buffer, sizeof(buffer), "3\tHello from namedpipe..\n");
	write(namedPipe, buffer, sizeof(buffer));*/
	if(c3Status == 0) {
		/* Write to Childs from Message File*/
		fp = fopen(C2_TXT, "r");	/* Open Message File */
		while(fgets(tmpBuffer, (sizeof(tmpBuffer) + 2), fp)) {			/* Read all the messages from the file */
			write(namedPipe, tmpBuffer, sizeof(tmpBuffer));
		}	
		c3Status = 1;		/* File is logged */
	}

	fclose(fp);

	/* Read Messages from Parent Process of Child 2 */
	fp = fopen(C2_LOG, "a+");					/* Open a log file */
	printf("Child Process 2: reading from Child 1..\n");
	while(read(pipeParentFd[0], tmpBuffer, sizeof(tmpBuffer))) {	/* Read all the buffers from pipe */
		sscanf(tmpBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
		//printf("%d%s\n",msgId,buffer);
		if(msgId == 3) {											/* Message is belongs to the child */
			strcat(buffer, KEEP_MSG);								/* Append message will "keep" word*/
		} else {													/* Message is NOT belongs to the child */
			write(namedPipe, tmpBuffer, sizeof(tmpBuffer));			/* Forward the message to next child */
			strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
		}
		//printf("%d%s\n",msgId,buffer);
		fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
	}

	fclose(fp); /* Close file */

	if(c2Status != 0) {
	//	childProcess3Content();
	}

}

void childProcess3(int pipeParentFd[]) {
	
	/* Create the Child process 3 from the Parent */
	/* Fork a new process */
	/* Create a new pipe for child process 3 */
	int pipeC3Fd[2];
	int pid;	
	pipe(pipeC3Fd);
	FILE* fp;
	
	pid = fork();
	if(pid == 0) {
		/* Child 3 section */
		/* Child 3 section is opposite compare to the child 1, where child writes to parent */

		close(pipeC3Fd[0]);		/* Close the read pipe of child */
		
		childProcess3Content(pipeC3Fd);

	}
	else {

		/* Parent or root section */
		close(pipeC3Fd[1]);		/* Close the write pipe of parent */

		wait(&pid);
		int msgId;							/* If the round trip of the processes is none or less than one */
		fp = fopen(PP_LOG, "a+");					/* Open a log file */
		printf("Parent Process 1: reading from Child 3..\n");
		while(read(pipeC3Fd[0], tmpBuffer, sizeof(tmpBuffer))) {	/* Read all the buffers from pipe */
			
			sscanf(tmpBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
			//printf("%s\n",buffer);
			if(msgId == 1) {											/* Message is belongs to the child */
				strcat(buffer, KEEP_MSG);								/* Append message will "keep" word*/
			} else {													/* Message is NOT belongs to the child */
				//write(pipeFd[1], tmpBuffer, sizeof(tmpBuffer));			/* Forward the message to next child */
				strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
			}

			printf("%d%s\n",msgId,buffer);
			fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
			printf("IM DONE!\n");
		}
		printf("Close file PP LOG\n");
		fclose(fp);
			//childProcess1Content(pipeFd);
		
		/*printf("Parent Process reading from Child Process 3\n");
		read(pipeC3Fd[0], buffer, sizeof(buffer));
		printf("I'm Parent Process, I am reading message: %s\n", buffer);
*/
		//childProcess3Content();
		//close(pipeC3Fd[0]);		/* Close the read pipe */
		
	}

}

void childProcess3Content(int pipeC3Fd[]) {
	int msgId, namedPipe;
	FILE* fp;
	//char tBuffer[128];
	/* Write Back to Parent */
	/*printf("Child Process 3 - Writing \n");
	snprintf(buffer, sizeof(buffer), "3\tHello from Child Process 3");
	write(pipeC3Fd[1], buffer, sizeof(buffer));
*/
	if(c3Status == 0) {
		fp = fopen(C3_TXT, "r");	/* Open Message File */
		while(fgets(tmpBuffer, (sizeof(tmpBuffer) + 2), fp)) {			/* Read all the messages from the file */
			write(pipeC3Fd[1], tmpBuffer, sizeof(tmpBuffer));
		}
		
		c3Status = 1;
		//roundTurnNo++;
		//parentProcessContent();
	}
	

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
	//namedPipe = open(FIFO_PIPE, O_RDONLY);
	

	/* Read from the named pipe */
	/*read(namedPipe, buffer, sizeof(buffer));
	printf("I'm Child Process 3, I am reading message from a namedpipe: %s\n", buffer);
*/
	/* Read Messages from Parent Process of Child 3 using named pipe */
	// fp = fopen(C3_LOG, "a+");					/* Open a log file */
	// printf("Child Process 3: reading from Child 2..\n");
	// while(read(namedPipe, tmpBuffer, sizeof(tmpBuffer))) {	/* Read all the buffers from pipe */
	// 	sscanf(tmpBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
	// 	//printf("%d%s\n",msgId,buffer);
	// 	if(msgId == 4) {											/* Message is belongs to the child */
	// 		strcat(buffer, KEEP_MSG);								 Append message will "keep" word
	// 	} else {													/* Message is NOT belongs to the child */
	// 		write(pipeC3Fd[1], tmpBuffer, sizeof(tmpBuffer));			/* Forward the message to next child */
	// 		strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
	// 	}
	// 	//printf("%d%s\n",msgId,buffer);
	// 	fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
	// }

	// //close(pipeC3Fd[1]);		/* Close the write pipe */
	// fclose(fp); /* Close file */

	
	/*
	if(c3Status == 0) {
		/* Write to Childs from Message File*/
		// fp = fopen(C1_TXT, "r");	/* Open Message File */
		// while(fgets(tmpBuffer, (sizeof(tmpBuffer) + 2), fp)) {			 Read all the messages from the file 
		// 	write(pipeC3Fd[1], tmpBuffer, sizeof(tmpBuffer));
		// }	
		// c3Status = 1;		/* File is logged */

		//roundTurnNo++;
		//parentProcessContent();
//	}
}
