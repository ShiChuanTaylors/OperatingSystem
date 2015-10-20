#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


#define FIFO_PIPE "tmp/mypipe.fifo"
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

int rootProcess;

void childProcess1(int pipeParentFd[]);
void childProcess2(int pipeParentFd[]);
void childProcess3();

void childProcess1Content(int pipeC1Fd[], int pipeParentFd[]);
void childProcess2Content(int pipeParentFd[]);
void childProcess3Content(int pipeC3Fd[]);
void calcTimestamp(int timing[], time_t timestamp);

int main(int argc, char* argv[] ) {

	char buffer[128];		/* Buffer for the message content, with ASCII 128 bit max */
	char readBuffer[130];	/* Buffer for the whole line message that included MsgID, and the Message */
	char finalMsg[200];		/* Buffer for the whole line message that included MsgID, and the Message */
	FILE* fp;
	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[20];

	/* Remove all existing log files */
	remove(PP_LOG); remove(C1_LOG); remove(C2_LOG); remove(C3_LOG); remove(FIFO_PIPE);
	
	int pid;		//Initialize first process to ID 1
	int msgId;	

	/* Create unnamed pipe for the current process to the child */
	if(pipe(pipeFd) == -1) {
		perror("Pipe Error");
	}

	rootProcess = (int)fork();		/* Fork a child process 1 */

	if(rootProcess == 0) {
		/* Child 1 section */
		close(pipeFd[1]);		/* Close the write pipe of child */
		//setpriority(PRIO_PROCESS, 0, -20);
		childProcess1(pipeFd);
		return 0;

	}
	else {
		/* Parent or root section */
		close(pipeFd[0]);		/* Close the read pipe of parent */

		/* Write to Childs */
		printf("Parent writing to CP 1...\n");
		fp = fopen(PP_TXT, "r");	/* Open Message File */
		while(fgets(readBuffer, (sizeof(readBuffer) + 2), fp)) {			/* Read all the messages from the file */
			calcTimestamp(timing, timestamp);
	 		snprintf(timeWrite2PipeRecord, sizeof(timeWrite2PipeRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
			//snprintf(finalMsg, "%s%s", timeWrite2PipeRecord, readBuffer);
			//snprintf(finalMsg, sizeof(finalMsg), timeWrite2PipeRecord);
			strcat(finalMsg, timeWrite2PipeRecord);
			//printf("Concat: %s",finalMsg);
			strcat(finalMsg, readBuffer);
			printf("Writing: %s",finalMsg );

			write(pipeFd[1], finalMsg, sizeof(finalMsg));

			finalMsg[0] = '\0';
		}
		
		fclose(fp); /* Close file */	
		printf("Parent wrote to CP1...\n");
		
		/* Read Messages from Parent Process of Child 3 using named pipe */

		close(pipeFd[1]);
		// printf("RUNNING CHILD PROCESS 1 CONTENT\n");
		// childProcess1Content(pipeFd);
		// printf("RUNNING CHILD PROCESS 1 CONTENT DONE!\n");
//		setpriority(PRIO_PROCESS, 0, -30);

		
		// childProcess1Content(pipeFd);
		

		printf("Parent: Process 3 Start!\n");
		childProcess3();
		printf("Parent: Process 3 DONE!\n");

		//wait(&pid);
		/* Fork Child process 3 */
		/*printf("Process 3 done!\n");
		//wait(&pid);

		printf("Process 3 done!\n");
*/
		//close(pipeFd[1]);		/* Close the write pipe */

//		return 0;
	}
        	
	return EXIT_SUCCESS;
}


void childProcess1(int pipeParentFd[]) {

	char buffer[128];
	char readBuffer[130];
	char finalMsg[200];		
	//char buffer[128];
	/* Declare a pipe for child 1 */
	if(c1Status == 0) {

		/* Initialize pipe for child 1 */
		if(pipe(pipeC1Fd) == -1) {
			perror("Pipe Error");
		}

		printf("Pipe C1 Fd is piped\n");


		int pid = fork();

		if(pid == 0) {
			/* Child Process 2 section */

			close(pipeC1Fd[1]);		/* Close the write pipe of child */
			//wait(&pid);
			printf("Process 2 is running\n");

			/* Wait until child process one complete */
			
			childProcess2(pipeC1Fd);

			//close(pipeC1Fd[0]);		/* Close the read pipe */
		}
		else {
			/* Parent - Child Process 1 section */
			close(pipeC1Fd[0]);		/* Close the read pipe of parent */
			printf("RUNNING CHILD PROCESS 1 CONTENT\n");
			childProcess1Content(pipeC1Fd, pipeParentFd);
			printf("RUNNING CHILD PROCESS 1 CONTENT DONE!\n");

			
			
		}
		c1Status = 1;		/* File is logged */
	}
	else {
		childProcess1Content(pipeC1Fd, pipeParentFd);
	}

}

void childProcess1Content(int pipeC1Fd[], int pipeParentFd[]) {
	int msgId;
	FILE* fp;
	char buffer[128];
	char readBuffer[130];
	char finalMsg[200];	

	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[10];
	char timeWrite2FileRecord[10];

	
	/* Write to Childs from Message File*/
	printf("Write to Process 2\n");
	fp = fopen(C1_TXT, "r");	/* Open Message File */
	// while(fgets(readBuffer, (sizeof(readBuffer) + 2), fp)) {			 Read all the messages from the file 
	// 	write(pipeC1Fd[1], readBuffer, sizeof(readBuffer));
		
	// }
	while(fgets(readBuffer, (sizeof(readBuffer) + 2), fp)) {			/* Read all the messages from the file */
		calcTimestamp(timing, timestamp);
 		snprintf(timeWrite2PipeRecord, sizeof(timeWrite2PipeRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
		//snprintf(finalMsg, "%s%s", timeWrite2PipeRecord, readBuffer);
		//snprintf(finalMsg, sizeof(finalMsg), timeWrite2PipeRecord);
		strcat(finalMsg, timeWrite2PipeRecord);
		//printf("Concat: %s",finalMsg);
		strcat(finalMsg, readBuffer);
		printf("Writing: %s",finalMsg );

		write(pipeC1Fd[1], finalMsg, sizeof(finalMsg));

		finalMsg[0] = '\0';
	}

	c1Status = 1;		/* File is logged */
	fclose(fp);	
	
	printf("Write to Process 2 DONE!\n");


	/* Read Messages from Parent Process of Child 1 */
	printf("Open Children 1 Log File..\n");
	fp = fopen(C1_LOG, "a+");					/* Open a log file */
	printf("Child Process 1: reading from Parent..\n");
	// while(read(pipeParentFd[0], readBuffer, sizeof(readBuffer))) {	/* Read all the buffers from pipe */
		
	// 	//printf("%s\n",readBuffer);
	// 	sscanf(readBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
	// 	//printf("%d%s\n",msgId,buffer);
	// 	if(msgId == 2) {											/* Message is belongs to the child */
	// 		strcat(buffer, KEEP_MSG);								 Append message will "keep" word
	// 	} else {													/* Message is NOT belongs to the child */
	// 		write(pipeC1Fd[1], readBuffer, sizeof(readBuffer));		/* Forward the message to next child */

	// 		strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
	// 	}
	// 	//printf("%d%s\n",msgId,buffer);
	// 	fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
	// 	printf("DONEDONE!\n");
	// }

	while(read(pipeParentFd[0], finalMsg, sizeof(finalMsg))) {	/* Read all the buffers from pipe */
		
		sscanf(finalMsg, "%[^\t]\t%d%[^\n]",timeWrite2PipeRecord ,&msgId, buffer);				/* Break the message into the ID and message */
		calcTimestamp(timing, timestamp);
 		snprintf(timeWrite2FileRecord, sizeof(timeWrite2FileRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
 		printf("\n\n%s\n\n",timeWrite2FileRecord);
		if(msgId == 2) {											/* Message is belongs to the child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord,timeWrite2FileRecord, msgId, buffer, KEEP_MSG);	/* print message to file with KEEP */
			//strcat(buffer, KEEP_MSG);								
		} else {													 /* Message is NOT belongs to the child */
			write(pipeC1Fd[1], finalMsg, sizeof(finalMsg));				/* Forward the message to next child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord,timeWrite2FileRecord,msgId, buffer, FWD_MSG);		/* print message to file with FORWARDED */
		}
		printf("DONEDONEDONEDONE\n");
		timeWrite2FileRecord[0] = '\0';
		
	}
	//printf("I'm Child Process 1, I am reading message: %s\n", buffer);
	
	fclose(fp);
	close(pipeC1Fd[1]);		/* Close the write pipe */
	printf("Children 1 Log FIle saved successfully\n");

	
}

void childProcess2(int pipeParentFd[]) {
	int msgId;
	char buffer[128];
	char readBuffer[130];
	char finalMsg[200];	

	int namedPipe;
	FILE* fp;

	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[10];
	char timeWrite2FileRecord[10];

	//sleep(50000);
	printf("Running Child 2 Process..\n");


	/* Create a named pipe if is not exist */
	if (access(FIFO_PIPE, F_OK) == -1) {
		printf("Creating named pipe...\n");
		namedPipe = mkfifo(FIFO_PIPE, 0777); 
		if (namedPipe < 0) {
			/* the fifo name */
			/* check if fifo already /* if not then, create the fifo*/
			printf("Could not create fifo %s\n", FIFO_PIPE);
			exit(EXIT_FAILURE); 
		}
		printf("Named pipe created successfully: %s\n",FIFO_PIPE);
	}



	printf("Opening FIFO named pipe..\n");
	/* Open the named pipe on the write side of pipe, to connect with child process 3 */
	namedPipe = open(FIFO_PIPE, O_WRONLY);
	printf("NAME PIPE ISSS: %d\n", namedPipe);
	printf("FIFO file is opened..\n");
	printf("YOOOOOO..\n");


	
	/* Write to Childs from Message File*/
	printf("WRITE TO CHILD 3\n");
	fp = fopen(C2_TXT, "r");	/* Open Message File */
	// while(fgets(readBuffer, (sizeof(readBuffer) + 2), fp)) {			 Read all the messages from the file 
	// 	write(namedPipe, readBuffer, sizeof(readBuffer));
	// }	
	while(fgets(readBuffer, (sizeof(readBuffer) + 2), fp)) {			/* Read all the messages from the file */
		calcTimestamp(timing, timestamp);
 		snprintf(timeWrite2PipeRecord, sizeof(timeWrite2PipeRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
		//snprintf(finalMsg, "%s%s", timeWrite2PipeRecord, readBuffer);
		//snprintf(finalMsg, sizeof(finalMsg), timeWrite2PipeRecord);
		strcat(finalMsg, timeWrite2PipeRecord);
		//printf("Concat: %s",finalMsg);
		strcat(finalMsg, readBuffer);
		printf("Writing: %s",finalMsg );

		write(namedPipe, finalMsg, sizeof(finalMsg));

		finalMsg[0] = '\0';
	}
	c2Status = 1;		/* File is logged */
	printf("DONE WRITE TO CHILD 3\n");


	/* Read Messages from Parent Process of Child 2 */
	printf("Open Child 2 Log File..\n");
	fp = fopen(C2_LOG, "a+");					/* Open a log file */
	printf("Child Process 2: reading from Child 1..\n");
	// while(read(pipeParentFd[0], readBuffer, sizeof(readBuffer))) {	/* Read all the buffers from pipe */
	// 	printf("READING~~ CP2 from CP1\n");
	// 	sscanf(readBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
	// 	//printf("%d%s\n",msgId,buffer);
	// 	if(msgId == 3) {											/* Message is belongs to the child */
	// 		strcat(buffer, KEEP_MSG);								 Append message will "keep" word
	// 	} else {													/* Message is NOT belongs to the child */
	// 		write(namedPipe, readBuffer, sizeof(readBuffer));			/* Forward the message to next child */
	// 		strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
	// 	}
	// 	//printf("%d%s\n",msgId,buffer);
	// 	fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
	// 	printf("%d%s\n",msgId,buffer);
	// }

	while(read(pipeParentFd[0], finalMsg, sizeof(finalMsg))) {	/* Read all the buffers from pipe */
		
		sscanf(finalMsg, "%[^\t]\t%d%[^\n]",timeWrite2PipeRecord ,&msgId, buffer);				/* Break the message into the ID and message */
		calcTimestamp(timing, timestamp);
 		snprintf(timeWrite2FileRecord, sizeof(timeWrite2FileRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
		if(msgId == 3) {											/* Message is belongs to the child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord,timeWrite2FileRecord ,msgId, buffer, KEEP_MSG);	/* print message to file with KEEP */
			//strcat(buffer, KEEP_MSG);								
		} else {													 /* Message is NOT belongs to the child */
			write(namedPipe, finalMsg, sizeof(finalMsg));				/* Forward the message to next child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord,timeWrite2FileRecord ,msgId, buffer, FWD_MSG);		/* print message to file with FORWARDED */
		}
		
	}
	printf("Save and Close Child 2 Log File.\n");
	fclose(fp); /* Close file */

//	fclose(fp);

	
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
	char readBuffer[130];
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
		//wait(&pid);		
		childProcess3Content(pipeC3Fd);

	}
	else {

		/* Parent or root section */
		close(pipeC3Fd[1]);		/* Close the write pipe of parent */

		//wait(&pid);
		int msgId;							/* If the round trip of the processes is none or less than one */
		fp = fopen(PP_LOG, "a+");					/* Open a log file */
		printf("Parent Process 1: reading from Child 3..\n");
		// while(read(pipeC3Fd[0], readBuffer, sizeof(readBuffer))) {	/* Read all the buffers from pipe */
			
		// 	sscanf(readBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
		// 	//printf("%s\n",buffer);
		// 	if(msgId == 1) {											/* Message is belongs to the child */
		// 		strcat(buffer, KEEP_MSG);								 Append message will "keep" word
		// 	} else {													/* Message is NOT belongs to the child */
		// 		//write(pipeFd[1], readBuffer, sizeof(readBuffer));			/* Forward the message to next child */
		// 		strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
		// 	}
		// 	fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
		// 	printf("READING PP FROM C3\n");
		// }

		while(read(pipeC3Fd[0], finalMsg, sizeof(finalMsg))) {	/* Read all the buffers from pipe */
		
			sscanf(finalMsg, "%[^\t]\t%d%[^\n]",timeWrite2PipeRecord ,&msgId, buffer);				/* Break the message into the ID and message */
			calcTimestamp(timing, timestamp);
 			snprintf(timeWrite2FileRecord, sizeof(timeWrite2FileRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);

			if(msgId == 1) {											/* Message is belongs to the child */
				fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord, timeWrite2FileRecord ,msgId, buffer, KEEP_MSG);	/* print message to file with KEEP */
				//strcat(buffer, KEEP_MSG);								
			} else {													 /* Message is NOT belongs to the child */
				//write(pipeC3Fd[1], finalMsg, sizeof(finalMsg));				/* Forward the message to next child */
				fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord, timeWrite2FileRecord ,msgId, buffer, FWD_MSG);		/* print message to file with FORWARDED */
			}
			
		}
		printf("Close file PP LOG\n");
		fclose(fp);
		//close(pipeC3Fd[0]);		/* Close the read pipe */
		
	}

}

void childProcess3Content(int pipeC3Fd[]) {
	int msgId, namedPipe;
	char buffer[128];
	char readBuffer[130];
	char finalMsg[200];
	FILE* fp;

	time_t timestamp;
	int timing[3];
	char timeWrite2PipeRecord[10];
	char timeWrite2FileRecord[10];
	//setpriority(PRIO_PROCESS, 0, -20);

	/* Write Back to Parent */

		fp = fopen(C3_TXT, "r");	/* Open Message File */
		// while(fgets(readBuffer, (sizeof(readBuffer) + 2), fp)) {			 Read all the messages from the file 
		// 	write(pipeC3Fd[1], readBuffer, sizeof(readBuffer));
		// }
		
		
		//roundTurnNo++;
		//parentProcessContent();
	
	while(fgets(readBuffer, (sizeof(readBuffer) + 2), fp)) {			/* Read all the messages from the file */
		calcTimestamp(timing, timestamp);
 		snprintf(timeWrite2PipeRecord, sizeof(timeWrite2PipeRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
		//snprintf(finalMsg, "%s%s", timeWrite2PipeRecord, readBuffer);
		//snprintf(finalMsg, sizeof(finalMsg), timeWrite2PipeRecord);
		strcat(finalMsg, timeWrite2PipeRecord);
		//printf("Concat: %s",finalMsg);
		strcat(finalMsg, readBuffer);
		printf("Writing: %s",finalMsg );

		write(pipeC3Fd[1], finalMsg, sizeof(finalMsg));

		finalMsg[0] = '\0';
	}

	
	/* Read from Child Process 3 using the FIFO named pipe */

	/* Create a named pipe if is not exist */
	// if (access(FIFO_PIPE, F_OK) == -1) {
	// 	printf("CHILD PROCESS 3: Creating named pipe...\n");
	// 	namedPipe = mkfifo(FIFO_PIPE, 0777); 
	// 	if (namedPipe != 0) {
	// 		/* the fifo name */
	// 		 check if fifo already  if not then, create the fifo
	// 		printf("CHILD PROCESS 3: Could not create fifo %s\n", FIFO_PIPE);
	// 		exit(EXIT_FAILURE); 
	// 	}
	// 	printf("CHILD PROCESS 3: Named pipe created successfully: %s\n",FIFO_PIPE);
	// }

	/* Create a named pipe if is not exist */
	if (access(FIFO_PIPE, F_OK) == -1) {
		printf("Creating named pipe...\n");
		namedPipe = mkfifo(FIFO_PIPE, 0777); 
		if (namedPipe < 0) {
			/* the fifo name */
			/* check if fifo already /* if not then, create the fifo*/
			printf("Could not create fifo %s\n", FIFO_PIPE);
			exit(EXIT_FAILURE); 
		}
		printf("Named pipe created successfully: %s\n",FIFO_PIPE);
	}
	
	printf("CHILD PROCESS 3: Opening FIFO!!\n");
	
	namedPipe = open(FIFO_PIPE, O_RDONLY);		/* Open the named pipe on the read side of pipe, to connect with child process 2 */
	

	printf("CHILD PROCESS 3: Openned FIFO successfully!!\n");

	printf("OPEN C3LOG\n");


	/* Read Messages from Parent Process of Child 3 using named pipe */
	fp = fopen(C3_LOG, "a+");					/* Open a log file */
	printf("Child Process 3: reading from Child 2..\n");
	// while(read(namedPipe, readBuffer, sizeof(readBuffer))) {	/* Read all the buffers from pipe */
	// 	sscanf(readBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
	// 	//printf("%d%s\n",msgId,buffer);
	// 	if(msgId == 4) {											/* Message is belongs to the child */
	// 		strcat(buffer, KEEP_MSG);								/* Append message will "keep" word */
	// 	} else {													/* Message is NOT belongs to the child */
	// 		write(pipeC3Fd[1], readBuffer, sizeof(readBuffer));			/* Forward the message to next child */
	// 		strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
	// 	}
	// 	printf("%d%s\n",msgId,buffer);
	// 	fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
	// 	printf("READING CP3 from CP2\n");
	// }
	while(read(namedPipe, finalMsg, sizeof(finalMsg))) {	/* Read all the buffers from pipe */
		
		sscanf(finalMsg, "%[^\t]\t%d%[^\n]",timeWrite2PipeRecord ,&msgId, buffer);				/* Break the message into the ID and message */
		calcTimestamp(timing, timestamp);
		snprintf(timeWrite2FileRecord, sizeof(timeWrite2FileRecord), "%d:%d:%d\t",timing[0],timing[1],timing[2]);
		if(msgId == 4) {											/* Message is belongs to the child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord, timeWrite2FileRecord ,msgId, buffer, KEEP_MSG);	/* print message to file with KEEP */
			//strcat(buffer, KEEP_MSG);								
		} else {													 /* Message is NOT belongs to the child */
			write(pipeC3Fd[1], finalMsg, sizeof(finalMsg));				/* Forward the message to next child */
			fprintf(fp,"%s\t\t%s\t%d%s%s\n",timeWrite2PipeRecord, timeWrite2FileRecord ,msgId, buffer, FWD_MSG);		/* print message to file with FORWARDED */
		}
		
	}

	close(pipeC3Fd[1]);		/* Close the write pipe */
	
	fclose(fp); /* Close file */
	printf("Close and Save %s\n", C3_LOG);
//	sleep(50000);

}


void calcTimestamp(int timing[], time_t timestamp) {
	
	timestamp = 0;
	int now = time(&timestamp);

	/*int clock = now - 18000;
    int secsPastMidnight = clock % 86400;
    int hours = (secsPastMidnight / 3600);
    int minutes = (secsPastMidnight % 3600) / 60;
    int seconds = secsPastMidnight % 60;*/

    int clock = now - 18000;
    int secsPastMidnight = clock % 86400;
    timing[0] = (secsPastMidnight / 3600);
    timing[1] = (secsPastMidnight % 3600) / 60;
    timing[2] = secsPastMidnight % 60;

    //return t;
}
