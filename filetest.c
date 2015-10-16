#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BASE_PROCESSES 2
#define FIFO_PIPE "mypipe.fifo"

char buffer[128];

int main(int argc, char* argv[] ) {

	FILE* fp;
	int msgId;

	fp = fopen("dataProcess1.txt", "r");	/* Open Message File */
	while(fscanf(fp, "%d\t%[^\n]", &msgId, buffer) != EOF) {		/* Read all the messages from the file */
		if(msgId == 2) {
			printf("%d\t%s\n", msgId, buffer);
		}
	}
        	
	return EXIT_SUCCESS;
}
