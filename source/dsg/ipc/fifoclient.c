/*
 *==============================================================================
 *                                  fifoclient.c                              
 *                  Copyright (c) 2013 Jan Voges. All rights reserved.         
 *==============================================================================
 *
 * Description:
 *   FIFO server writing a string with at maximum 80 characters to a pipe.
 *
 * Created:
 *   Aug 16, 2011  by Jan Voges, Intel Corp.
 *
 * Changes:
 *   Mmm dd, yyyy: Did something (Jan Voges).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  const char* PROGRAM = "fifoclient.c";
  const char* FIFO    = "MYFIFO";

  FILE* fp;
  char  buf[80];
  
  if (argc != 2)
  {
    fprintf(stdout, "-I- %s: Usage: fifoclient [string]\n", PROGRAM);
    exit(-1);
  }

  if (strlen(argv[1]) > 80)
  {
    fprintf(stderr, "-E- %s: Buffer exceeded: %d Aborting!\n", PROGRAM, strlen(argv[1]));
    exit(-1);
  }

  if (access(FIFO, F_OK) != 0)
  {
    fprintf(stderr, "-E- %s: FIFO '%s' does not exist!\n", PROGRAM, FIFO);
    exit(-1);
  }

  if ((fp = fopen(FIFO, "w")) == NULL)
  {
    fprintf(stderr, "-E- %s: Error opening FIFO '%s'!\n", PROGRAM, FIFO);
    exit(-1);
  }

  fputs(argv[1], fp);

  fclose(fp);
  
  return(0);
}

