/*
  input:
    - audio file
    - start time
    - end time

  output:
    16kHz 16-bit PCM wav file

  Compute f0 samples for given audio segment.
  Synthesize tones of frequencies given by the f0 samples.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <get_f0.h>

void usage(char *prog)
{
  printf("Usage: %s <audio file> <start time> <end time>\n\n", prog);
}

int main(int argc, char *argv[])
{
  if (argc != 4) {
    usage(argv[0]);
    exit(1);
  }

  return 0;
}
