#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BASE_PROCESSES 2
#define FIFO_PIPE "mypipe.fifo"

char buffer[500];

int main(int argc, char* argv[] ) {

	FILE* fp;
	int msgId;

	fp = fopen("c1Msg.txt", "r");	/* Open Message File */
	while(fscanf(fp, "%d%[^\n]", &msgId, buffer) != EOF) {		/* Read all the messages from the file */

		snprintf(buffer, sizeof(buffer), "%d%s", msgId, buffer);
		//printf("%d%s\n", msgId, buffer);
		printf("%s\n", buffer);
		
	}

	/*while(fgets(buffer, sizeof(buffer), fp)) {
		printf("%s", buffer);
	}*/

	return EXIT_SUCCESS;
}
