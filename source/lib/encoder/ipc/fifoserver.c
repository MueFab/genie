/*
 *==============================================================================
 *                                  fifoserver.c                              
 *                  Copyright (c) 2013 Jan Voges. All rights reserved.         
 *==============================================================================
 *
 * Description:
 *   FIFO server creating a pipe and waiting in a while-1 loop to receive data 
 *   in chunks of 80 bytes from another process that is writing to this pipe.
 *
 * Created:
 *   Aug 16, 2011  by Jan Voges, Intel Corp.
 *
 * Changes:
 *   Mmm dd, yyyy: Did something (Jan Voges).
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>

int main(void)
{
  const char* FIFO    = "MYFIFO";

  FILE*   fp = NULL;
  char    readbuf[80];

  /* Create the FIFO if it doesn't exist */
  if (access(FIFO, F_OK) == 0) {
    fprintf(stderr, "Error: FIFO '%s' exists!\n", FIFO);
    exit(-1);
  }
  else {
    umask(0);
    mknod(FIFO, S_IFIFO|0666, 0);
  }

  /* Wait for the pipe */
  while (1) {
    fp = fopen(FIFO, "r");
    fgets(readbuf, 80, fp);

    /* Print data, alternatively start cache simulation */
    fprintf(stdout, "Received string: |%s|\n", readbuf);

    fclose(fp);
  }

  return(0);
}
