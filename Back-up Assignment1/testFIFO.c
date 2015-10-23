#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BASE_PROCESSES 2
#define FIFO_PIPE "testpipe.fifo"
#define KEEP_MSG "\tKeep"
#define FWD_MSG "\tForwarded"

/* Process Messages */
#define C2_TXT "messages/c2Msg.txt"

char buffer[128];			/* Buffer for the message content, with ASCII 128 bit max */
char tmpBuffer[130];		/* Buffer for the whole line message that included MsgID, and the Message */

int main(int argc, char* argv[] ) {

	FILE* fp;
	/* Remove all existing log files */

	remove(FIFO_PIPE);
	
	
	int pid;		//Initialize first process to ID 1
	
	int msgId, namedPipe;	
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

	pid = (int)fork();

	if(pid == 0) {
		/* Child 1 section */

		

		printf("Opening FIFO named pipe..\n");
		/* Open the named pipe for read only on child process 2 */
		namedPipe = open(FIFO_PIPE, O_RDONLY);
		printf("Opening success..\n");
		
		//fclose(fp);
		
		fp = fopen("TESTLOG.txt", "a+");					/* Open a log file */
		printf("Parent Process 1: reading from Child 3..\n");
		while(read(namedPipe, tmpBuffer, sizeof(tmpBuffer))) {	/* Read all the buffers from pipe */
			
			sscanf(tmpBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
			printf("MSG: %d, %s\n", msgId, buffer);
			if(msgId == 1) {											/* Message is belongs to the child */
				strcat(buffer, KEEP_MSG);								/* Append message will "keep" word*/
			} else {													/* Message is NOT belongs to the child */
				//write(pipeFd[1], tmpBuffer, sizeof(tmpBuffer));			/* Forward the message to next child */
				strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
			}
			fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
		}
		printf("Close file PP LOG\n");
		fclose(fp);

		return 0;

	}
	else {
		/* Parent or root section */
		
		printf("PARENT: Opening FIFO named pipe..\n");
		/* Open the named pipe for read only on child process 2 */
		namedPipe = open(FIFO_PIPE, O_WRONLY);
		
		printf("PARENT: Opened FIFO successfully.\n");
		
		/* Write to Childs from Message File*/
		fp = fopen(C2_TXT, "r");	/* Open Message File */
		while(fgets(tmpBuffer, (sizeof(tmpBuffer) + 2), fp)) {			/* Read all the messages from the file */
			printf("%s\n", tmpBuffer );
			write(namedPipe, tmpBuffer, sizeof(tmpBuffer));
		}	
		//c2Status = 1;		/* File is logged */
		printf("PARENT: Close file\n");
		fclose(fp);
		
	}
        	
	return EXIT_SUCCESS;
}


// void childProcess3(int pipeParentFd[]) {
	
// 	/* Create the Child process 3 from the Parent */
// 	/* Fork a new process */
// 	/* Create a new pipe for child process 3 */
// 	int pipeC3Fd[2];
// 	int pid;	
// 	pipe(pipeC3Fd);
// 	FILE* fp;
	
// 	pid = fork();
// 	if(pid == 0) {
// 		/* Child 3 section */
// 		/* Child 3 section is opposite compare to the child 1, where child writes to parent */

// 		close(pipeC3Fd[0]);		/* Close the read pipe of child */
		
// 		childProcess3Content(pipeC3Fd);

// 	}
// 	else {

// 		/* Parent or root section */
// 		close(pipeC3Fd[1]);		/* Close the write pipe of parent */

// 		wait(&pid);
// 		int msgId;							/* If the round trip of the processes is none or less than one */
// 		fp = fopen(PP_LOG, "a+");					/* Open a log file */
// 		printf("Parent Process 1: reading from Child 3..\n");
// 		while(read(pipeC3Fd[0], tmpBuffer, sizeof(tmpBuffer))) {	/* Read all the buffers from pipe */
			
// 			sscanf(tmpBuffer, "%d%[^\n]",&msgId, buffer);				/* Break the message into the ID and message */
// 			//printf("%s\n",buffer);
// 			if(msgId == 1) {											/* Message is belongs to the child */
// 				strcat(buffer, KEEP_MSG);								/* Append message will "keep" word*/
// 			} else {													/* Message is NOT belongs to the child */
// 				//write(pipeFd[1], tmpBuffer, sizeof(tmpBuffer));			/* Forward the message to next child */
// 				strcat(buffer, FWD_MSG);								/* Append message will "forward" word*/
// 			}
// 			fprintf(fp, "%d%s\n",msgId,buffer);				/* Write the messages into the log file */
// 		}
// 		printf("Close file PP LOG\n");
// 		fclose(fp);
// 			//childProcess1Content(pipeFd);
		
// 		/*printf("Parent Process reading from Child Process 3\n");
// 		read(pipeC3Fd[0], buffer, sizeof(buffer));
// 		printf("I'm Parent Process, I am reading message: %s\n", buffer);
// */
// 		//childProcess3Content();
// 		//close(pipeC3Fd[0]);		/* Close the read pipe */
		
// 	}

// }
