/*
 *  assignment1.h
 *  finalAss1.c
 *
 *  Created by Ting Shi Chuan on 20/10/2015.
 *
*/

#ifndef assignment1_h
#define assignment1_h

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
#define MSG_TXT "messages/Messages.txt"		/* Text Messages to pass thru pipes */

/* Log Messages */
#define PP_LOG "log/ppLog.txt"		/* Log file for Parent Process 1 */
#define C1_LOG "log/c1Log.txt"		/* Log file for Child Process 1 */
#define C2_LOG "log/c2Log.txt"		/* Log file for Child Process 2 */
#define C3_LOG "log/c3Log.txt"		/* Log file for Child Process 3 */

void childProcess1(int pipeParentFd[]);		
void childProcess2(int pipeParentFd[]);
void childProcess3();

void childProcess1Content(int pipeC1Fd[], int pipeParentFd[]);
void childProcess2Content(int pipeParentFd[]);
void childProcess3Content(int pipeC3Fd[]);
void calcTimestamp(int timing[], time_t timestamp);

#endif /* assignment1_h */