//
// simple test of the simulated virtual memory system
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simVM.h"

#define VMEMSIZE 64*8
#define PMEMSIZE 4*8
#define PAGESIZE 4*4
#define TLBSIZE 2*4
#define MEMSIZE (VMEMSIZE*PAGESIZE)

int main(void){
  srand(time(NULL));
  void *h = createVM(VMEMSIZE, PMEMSIZE, PAGESIZE, TLBSIZE, 1, 0);
  if (h == NULL){
    fprintf(stderr, "createVM failed!\n");
    exit(1);
  }
  uint i=0,j=0;
  long sum=0, sum2=0;
  while(i<MEMSIZE){
    j=lrand48()%MEMSIZE;
    if(i==j){
      i++;
    }
    writeInt(h, j, j);
  }
  i=0;
  while(i<MEMSIZE){
    j=lrand48()%MEMSIZE;
    int tmp = readInt(h, j);    
    sum += tmp;
    sum2 += tmp;
    if(i==j){
      i++;
    }
  }
  printf("sum is %lu (should be %lu)\n", sum, sum2);

  // print stats
  printf("\n");
  printStatistics(h);
  printf("\n");

  return 0;
}


