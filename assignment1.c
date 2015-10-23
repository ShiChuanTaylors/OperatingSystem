/*
 *  assignment1.h
 *  assignment1.c
 *
 *  Created by Ting Shi Chuan on 20/10/2015.
 *
*/

#include "assignment1.h"

int main(int argc, char* argv[] ) {

	char readBuffer[130];	/* Buffer to read the whole line message that included MsgID, and the Message */
	char finalMsg[200];		/* Buffer for the Final message that need to write into log file */
	FILE* fp;
	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[20];

	if(argc < 2) {
		printf("Usage:<./assignment1> <Message.txt>\n");
		return 1;
	}

	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		perror("Message Text Cannot Open: ");
		return EXIT_FAILURE;
	}
	/* Remove all existing log files */
	remove(PP_LOG); remove(C1_LOG); remove(C2_LOG); remove(C3_LOG); remove(FIFO_PIPE);
	
	int pid;		
	int pipeFd[2];
	/* Create unnamed pipe for the current process to the child */
	if(pipe(pipeFd) == -1) {
		perror("Pipe Error");
	}
	pid = (int)fork();		/* Fork a child process 1 */
	if(pid == 0) {
		/* Child 1 section */
		close(pipeFd[1]);		/* Close the write pipe of child */
		childProcess1(pipeFd);
		return 0;
	}
	else {
		/* Parent or root section */
		close(pipeFd[0]);		/* Close the read pipe of parent */

		/* Write to Childs */
		printf("Parent writing to CP 1...\n");
		/* Open Message File */

		while(fgets(readBuffer, (sizeof(readBuffer) + 2), fp)) {			/* Read all the messages from the file */
			calcTimestamp(timing, timestamp);								/* Calculate Timestamp */
	 		snprintf(timeWrite2PipeRecord, sizeof(timeWrite2PipeRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);	/* Print timestamp to buffer */
			strcat(finalMsg, timeWrite2PipeRecord);							/* Add the timestamp at the front of message */
			strcat(finalMsg, readBuffer);									/* Add the buffer message after the timestamp*/
			write(pipeFd[1], finalMsg, sizeof(finalMsg));					/* Write the final message into the pipe */
			
			finalMsg[0] = '\0';
		}
		
		fclose(fp); /* Close file */	
		printf("Parent wrote to CP1...\n");
		close(pipeFd[1]);
		printf("Parent: Process 3 Start!\n");
		childProcess3();
		printf("Parent: Process 3 DONE!\n");
	}
	return EXIT_SUCCESS;
}


void childProcess1(int pipeParentFd[]) {

	char buffer[128];
	int pipeC1Fd[2]; 	/* Pipe for Child 1 to 2 */
	/* Declare a pipe for child 1 */

	/* Initialize pipe for child 1 */
	if(pipe(pipeC1Fd) == -1) {
		perror("Pipe Error");
	}
	printf("Pipe C1 Fd is piped\n");
	int pid = fork();
	if(pid == 0) {
		/* Child Process 2 section */
		close(pipeC1Fd[1]);		/* Close the write pipe of child */
		printf("Process 2 is running\n");
		childProcess2(pipeC1Fd);
	}
	else {
		/* Parent - Child Process 1 section */
		close(pipeC1Fd[0]);		/* Close the read pipe of parent */
		printf("RUNNING CHILD PROCESS 1 CONTENT\n");
		childProcess1Content(pipeC1Fd, pipeParentFd);
		printf("RUNNING CHILD PROCESS 1 CONTENT DONE!\n");
	}
}

void childProcess1Content(int pipeC1Fd[], int pipeParentFd[]) {
	int msgId;
	FILE* fp;
	char buffer[128];		/* Buffer for the message content, with ASCII 128 bit max */
	char finalMsg[200];	

	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[10];
	char timeWrite2FileRecord[10];
	
	printf("Write to Process 2 DONE!\n");
	/* Read Messages from Parent Process of Child 1 */
	printf("Open Children 1 Log File..\n");
	fp = fopen(C1_LOG, "a+");					/* Open a log file */
	printf("Child Process 1: reading from Parent..\n");

	if(read(pipeParentFd[0], finalMsg, sizeof(finalMsg)) == -1) {
		perror("PIPE");
	}
	while(read(pipeParentFd[0], finalMsg, sizeof(finalMsg))) {	/* Read all the buffers from pipe */
		sscanf(finalMsg, "%[^\t]\t%d%[^\n]",timeWrite2PipeRecord ,&msgId, buffer);				/* Break the message into the ID and message */
		calcTimestamp(timing, timestamp);
 		snprintf(timeWrite2FileRecord, sizeof(timeWrite2FileRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
		if(msgId == 2) {											/* Message is belongs to the child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord,timeWrite2FileRecord, msgId, buffer, KEEP_MSG);	/* print message to file with KEEP */
		} else {													 /* Message is NOT belongs to the child */
			write(pipeC1Fd[1], finalMsg, sizeof(finalMsg));				/* Forward the message to next child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord,timeWrite2FileRecord,msgId, buffer, FWD_MSG);		/* print message to file with FORWARDED */
		}
		timeWrite2FileRecord[0] = '\0';
		
	}
	fclose(fp);
	close(pipeC1Fd[1]);		/* Close the write pipe */
	printf("Children 1 Log FIle saved successfully\n");
}

void childProcess2(int pipeParentFd[]) {
	int msgId;
	char buffer[128];
	char finalMsg[200];	

	int namedPipe;
	FILE* fp;

	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[10];
	char timeWrite2FileRecord[10];

	printf("Running Child 2 Process..\n");
	/* Create a named pipe if is not exist */
	if (access(FIFO_PIPE, F_OK) == -1) {
		printf("Creating named pipe...\n");
		namedPipe = mkfifo(FIFO_PIPE, 0777); 
		if (namedPipe < 0) {
			/* the fifo name */
			/* check if fifo already if not then, create the fifo*/
			printf("Could not create fifo %s\n", FIFO_PIPE);
			exit(EXIT_FAILURE); 
		}
		printf("Named pipe created successfully: %s\n",FIFO_PIPE);
	}

	printf("Opening FIFO named pipe..\n");
	/* Open the named pipe on the write side of pipe, to connect with child process 3 */
	namedPipe = open(FIFO_PIPE, O_WRONLY);

	/* Read Messages from Parent Process of Child 2 */
	printf("Open Child 2 Log File..\n");
	fp = fopen(C2_LOG, "a+");					/* Open a log file */
	printf("Child Process 2: reading from Child 1..\n");

	/* Read all the buffers from pipe */
	while(read(pipeParentFd[0], finalMsg, sizeof(finalMsg))) {	
		sscanf(finalMsg, "%[^\t]\t%d%[^\n]",timeWrite2PipeRecord ,&msgId, buffer);				/* Break the message into the ID and message */
		calcTimestamp(timing, timestamp);
 		snprintf(timeWrite2FileRecord, sizeof(timeWrite2FileRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
		if(msgId == 3) {											/* Message is belongs to the child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord,timeWrite2FileRecord ,msgId, buffer, KEEP_MSG);	/* print message to file with KEEP */
		} else {													 /* Message is NOT belongs to the child */
			write(namedPipe, finalMsg, sizeof(finalMsg));				/* Forward the message to next child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord,timeWrite2FileRecord ,msgId, buffer, FWD_MSG);		/* print message to file with FORWARDED */
		}
	}
	printf("Save and Close Child 2 Log File.\n");
	fclose(fp); /* Close file */
}

void childProcess3() {
	
	/* Create the Child process 3 from the Parent */
	/* Fork a new process */
	/* Create a new pipe for child process 3 */
	int pipeC3Fd[2];
	int pid;	
	pipe(pipeC3Fd);
	FILE* fp;
	char buffer[128];
	char finalMsg[200];

	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[10];
	char timeWrite2FileRecord[10];
	
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
		int msgId;							/* If the round trip of the processes is none or less than one */
		fp = fopen(PP_LOG, "a+");					/* Open a log file */
		printf("Parent Process 1: reading from Child 3..\n");

		while(read(pipeC3Fd[0], finalMsg, sizeof(finalMsg))) {	/* Read all the buffers from pipe */
		
			sscanf(finalMsg, "%[^\t]\t%d%[^\n]",timeWrite2PipeRecord ,&msgId, buffer);				/* Break the message into the ID and message */
			calcTimestamp(timing, timestamp);
 			snprintf(timeWrite2FileRecord, sizeof(timeWrite2FileRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);

			if(msgId == 1) {												/* Message is belongs to the child */
				fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord, timeWrite2FileRecord ,msgId, buffer, KEEP_MSG);	/* print message to file with KEEP */
			} else {													 	/* Message is NOT belongs to the child */
				fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord, timeWrite2FileRecord ,msgId, buffer, FWD_MSG);		/* print message to file with FORWARDED */
			}
			
		}
		printf("Close file PP LOG\n");
		fclose(fp);
	}
}

void childProcess3Content(int pipeC3Fd[]) {
	int msgId, namedPipe;
	char buffer[128];
	char finalMsg[200];
	FILE* fp;

	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[10];
	char timeWrite2FileRecord[10];

	/* Create a named pipe if is not exist */
	if (access(FIFO_PIPE, F_OK) == -1) {
		printf("Creating named pipe...\n");
		namedPipe = mkfifo(FIFO_PIPE, 0777); 
		if (namedPipe < 0) {
			/* the fifo name */
			/* check if fifo already if not then, create the fifo*/
			printf("Could not create fifo %s\n", FIFO_PIPE);
			exit(EXIT_FAILURE); 
		}
		printf("Named pipe created successfully: %s\n",FIFO_PIPE);
	}
	
	printf("CHILD PROCESS 3: Opening FIFO!!\n");
	
	namedPipe = open(FIFO_PIPE, O_RDONLY);		/* Open the named pipe on the read side of pipe, to connect with child process 2 */
	

	printf("CHILD PROCESS 3: Openned FIFO successfully!!\n");

	/* Read Messages from Parent Process of Child 3 using named pipe */
	printf("OPEN C3LOG\n");
	fp = fopen(C3_LOG, "a+");					/* Open a log file */
	printf("Child Process 3: reading from Child 2..\n");

	while(read(namedPipe, finalMsg, sizeof(finalMsg))) {	/* Read all the buffers from pipe */
		
		sscanf(finalMsg, "%[^\t]\t%d%[^\n]",timeWrite2PipeRecord ,&msgId, buffer);				/* Break the message into the ID and message */
		calcTimestamp(timing, timestamp);
		snprintf(timeWrite2FileRecord, sizeof(timeWrite2FileRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
		if(msgId == 4) {												/* Message is belongs to the child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord, timeWrite2FileRecord ,msgId, buffer, KEEP_MSG);	/* print message to file with KEEP */
		} else {													 	/* Message is NOT belongs to the child */
			write(pipeC3Fd[1], finalMsg, sizeof(finalMsg));				/* Forward the message to next child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord, timeWrite2FileRecord ,msgId, buffer, FWD_MSG);		/* print message to file with FORWARDED */
		}
		
	}

	close(pipeC3Fd[1]);		/* Close the write pipe */
	fclose(fp); 			/* Close file */
	printf("Close and Save %s\n", C3_LOG);
}

void calcTimestamp(int timing[], time_t timestamp) {
	//timestamp = 0;
	time(&timestamp);

    int clock = timestamp - 18000;
    int secsPastMidnight = clock % 86400;
    timing[0] = (secsPastMidnight / 3600);			/* hours */
    timing[1] = (secsPastMidnight % 3600) / 60;		/* minutes */
    timing[2] = secsPastMidnight % 60;				/* seconds */
}
