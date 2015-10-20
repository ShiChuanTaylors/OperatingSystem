#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BASE_PROCESSES 2
#define FIFO_PIPE "mypipe.fifo"

int main(int argc, char* argv[] ) {

	char buffer[128];
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
	
	int rootPipe[2];
	int namedPipe;
	int isFirstForkStarted = 0;


	//Create a named pipe if is not exist
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
	int pid, i, child = 1;		//Initialize first process to ID 1




  	for(i = 0; i < 2; i++)
  	{	

  		//Create unnamed pipe for the current process to the child
		if(pipe(pipeFd) == -1) {
			perror("Pipe Error");
		}

		if(child == 1 && isFirstForkStarted == 0)		//The root process is forking the FIRST time
			isFirstForkStarted = 1;
		else if(child == 1 && isFirstForkStarted == 1)	//The root process is forking the SECOND time	
			child = 3;									//Most bottom child, which need to connect to 
  		

	  	pid = (int)fork();




		if(pid == 0)
	    {	
	    	//Assign child process id
	  		child++;
	    	
	    	/* Child Section */
	    	wait(&pid);		//Wait for parent to complete process until write message

	    	//Update status if the fork started
			if(child == 3) {
				printf("\nHey Bro! im in 3\n");	  
				wait(&pid);		//Wait for process 4 to complete
				
				printf("Opening named pipe...");
				//Open the named pipe for read only on child process 3
				namedPipe = open(FIFO_PIPE, O_RDONLY);
				//Writing logs message to each child processes
	  			int count;
	  			for(count = 0; count < 4; count++) {
	  				if(count != child) {
	  					/* Make sure the buffer message will not cause buffer overflow */
	  				/*	snprintf(buffer, sizeof(buffer), "%d\tHello from Process %d using FIFO", count, child);
	  			//	}
										
	  				
	  				write(namedPipe, buffer, sizeof(buffer));
	  			//}

	  			close(pipeFd[1]); //close the child write pipe	  			
				
	  			printf("Child No.%d reading from Parent..\n", child);
				read(pipeFd[0], buffer, sizeof(buffer));
				printf("I'm Child Process %d, I am reading message: %s\n", child, buffer);
				
			}
	  		else if(child == 4) {

	  			/* The forth child will pipe back to the parent process 1, */
	  			/* which means in this condition, it contrast to the normal condition */
	  			/* forth child process will only can WRITE to parent 1; */
	  			
	  			//wait(&pid);	//Wait until the children processes completed

	  			close(pipeFd[0]);	//close the child read pipe
	  			
	  			//		snprintf(buffer, sizeof(buffer), "3\tHello from Process %d using FIFO", child);
	  			//		write(namedPipe, buffer, sizeof(buffer));


	  			/* Connect with child process 3 with the FIFO named pipe */
	  			//Create a named pipe if is not exist
				/*if (access(FIFO_PIPE, F_OK) == -1) {
				
					namedPipe = mkfifo(FIFO_PIPE, 0777); 
					if (namedPipe != 0) {
						/* the fifo name */
						/* check if fifo already /* if not then, create the fifo*/
				/*		fprintf(stderr, "Could not create fifo %s\n", FIFO_PIPE);
						exit(EXIT_FAILURE); 
					}
				} */

//				//Open the named pipe for write on child process 4
				namedPipe = open(FIFO_PIPE, O_WRONLY);
				//printf("HELLLOOOOOO\n");
				read(namedPipe, buffer, sizeof(buffer));
//				printf("Process 4 reading from namedPipe: %s\n", buffer);*/
  					/* Make sure the buffer message will not cause buffer overflow */
  					snprintf(buffer, sizeof(buffer), "1\tHello from Child Process %d", child);

  					
  				
	  				write(pipeFd[1], buffer, sizeof(buffer));	
  			


	  		}	
	  		else {

	  			/* All the child only can read from the parents; NOT writing */

	  			if(child == 4) {
	  				wait(&pid);	//Wait until the children processes completed

	  				close(pipeFd[0]);	//close the child read pipe

	  				snprintf(buffer, sizeof(buffer), "1\tHello from Child Process %d", child);

	  				write(pipeFd[1], buffer, sizeof(buffer));	
		  			
	  			}
	  			else {
	  				close(pipeFd[1]); //close the child write pipe	  			
					//wait(&pid);	//Wait until the parent processes completed
		  			printf("Child No.%d reading from Parent..\n", child);
					read(pipeFd[0], buffer, sizeof(buffer));
					printf("I'm Child Process %d, I am reading message: %s\n", child, buffer);
					//printf("HIHIHIHIHI Child: %d\n",child);	
	  			}
	  			
	  			
	  		}
	    }
	  	else {

  			/* Parent Section */
	  		
	  		

	  		//printf("PARENT: HELLOO %d\n",child);
			if(child != 1)
				close(pipeFd[0]);

			
  				
	  		//Parent only can write to next child
  			
  			//Writing logs message to each child processes
  			/*int count;
  			for(count = 0; count < 4; count++) {
  				if(count != child) {
  					/* Make sure the buffer message will not cause buffer overflow */
  					//snprintf(buffer, sizeof(buffer), "%d\tHello from Process %d", count, child);

  			//	}
  					
  				
  			//}

			
			

			//printf("isFirstForkStarted: %d\n",isFirstForkStarted);
  			//Check if is the first time access parent process after fork
  			//printf("MYYYCHILD: %d\n", child );
	  		if(child == 3 && isFirstForkStarted == 1) {
	  			//child = 3;
	  			//wait(&pid);		//Wait for child to complete process until write message
	  			wait(&pid);
	  			printf("Parent 1 reading from Child..\n");
				read(pipeFd[0], buffer, sizeof(buffer));
				printf("I'm Parent Process 1, I am reading message: %s\n", buffer);
	  		}
	  		else {
	  			snprintf(buffer, sizeof(buffer), "[NO]\tHello from Process %d", child);
  				write(pipeFd[1], buffer, sizeof(buffer));
  				isFirstForkStarted == 1;	
	  		}

				
			//printf("isFirstForkStarted2: %d\n",isFirstForkStarted);
	  		

	  		//break; 	//Prevent the root parent to fork again

	  	}
  	}
        	
	return EXIT_SUCCESS;
}