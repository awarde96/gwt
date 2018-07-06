/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "Gap8.h"
#include "ModelKernels.h"
#include "model-size.h"

#define MOUNT           1
#define UNMOUNT         0
#define CID             0
#define STACK_SIZE      2048

L2_MEM short int *M1;
L2_MEM signed char *M2;
L2_MEM short int *Out1;
//L2_MEM short int *Out2;

L2_MEM signed char *A;
L2_MEM short int *IA;
L2_MEM short int *JA;

L2_MEM rt_perf_t *cluster_perf;
int finished = 0;
int nnz = 0;

unsigned short lfsr = 0xACE1u;
unsigned bit;

static void cluster_main()
{
  	printf ("cluster master start\n");
    unsigned int ElapsedTime[2];
    rt_perf_t *perf = cluster_perf;
    // initialize the performance clock
    rt_perf_init(perf);
    // Configure performance counters for counting the cycles
    rt_perf_conf(perf, (1<<RT_PERF_CYCLES));

    rt_perf_reset(perf);
    rt_perf_start(perf);
    ParMatMult(/*M1,*/ M2, Out1, A, IA, JA, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    ElapsedTime[0] = rt_perf_get(perf, RT_PERF_CYCLES);

    ElapsedTime[1] = rt_perf_get(perf, RT_PERF_CYCLES) - ElapsedTime[0];

    printf("ParMatMult: %d cycles %d ops/cycle\n", ElapsedTime[0], (4170000/ElapsedTime[0]));
    //printf("ParVectMatMult: %d cycles, %d ops/cycle\n", ElapsedTime[1], (4170000/ElapsedTime[1]));
}

static void end_of_app(){
  finished = 1;
  printf ("End of application\n");
}

unsigned rand(){
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
  return lfsr = (lfsr >> 1) | (bit << 15);
}

int main()
{

  printf ("Matrix Mult start\n");
  int H_M1 = 10;
  int W_M1 = WIDTH;
  int H_M2 = WIDTH;
  int W_M2 = 1;

  int W_Out = H_M1;
  int H_Out = W_M2;

  M1  = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, W_M1*H_M1*sizeof(short int));
  M2  = (signed char *) rt_alloc(RT_ALLOC_L2_CL_DATA, W_M2*H_M2*sizeof(char));
  Out1 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, W_Out*H_Out*sizeof(short int));
  //Out2 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, W_Out*H_Out*sizeof(short int));
  if ((M1==0)||(M2==0)||(Out1==0) /*||(Out2==0)*/) {
    printf("Failed to allocate mem.\n"); return 0;
  }

  for(int i=0;i< W_M1*H_M1;i++) {
    if(i%SPARSITY == 1)
      M1[i]=2;
    else
      M1[i]=0;
  }
  for(int i=0;i< W_M2*H_M2;i++) M2[i]=3;

  nnz = 0;
  for(int i = 0; i < W_M1*H_M1; i++){
    if(M1[i] == 0)
      nnz++;
  }

  A = (signed char *) rt_alloc(RT_ALLOC_L2_CL_DATA, nnz*sizeof(signed char));
  JA = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, nnz*sizeof(short int));
  IA = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, (H_M1+1)*sizeof(short int));

  int index = 0;
  for(int i = 0; i < W_M1*H_M1; i++){
    if(M1[i] != 0)
      A[index++] = M1[i];
  }

  IA[0] = 0;
  index = 0;
  int count = 0;
  for(int i = 0; i < H_M1; i++){
    //int count = 0;
    for(int j = 0; j < W_M1; j++){
      if(M1[i*W_M1 + j] != 0){
        count++;
        JA[index++] = j;
      }

    }
    IA[i+1] = count;
  }



  // Allocate some events
  if (rt_event_alloc(NULL, 8)){printf("Error in events allocation...\n"); return -1;}

  // Switch on the cluster
  rt_cluster_mount(MOUNT, CID, 0, NULL);

  cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  if (cluster_perf == NULL) return -1;

  // Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) return -1;

  // Allocate the predetermined memory size in the shared L1 memory that the cluster can act on
  L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _L1_Memory_SIZE);

  if(L1_Memory == NULL ){
    printf("Memory Allocation Error! Quit...\n");
    return 1;
  }

  printf("Calling cluster....\n");
  rt_cluster_call(NULL, CID, cluster_main, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);

  rt_cluster_mount(UNMOUNT, CID, 0, NULL);

  count = 0;
  printf("Results\n");
  for(int i=0;i<H_Out;i++){
    for(int j=0;j<W_Out;j++){
    	printf("%d ", Out1[i*W_Out+j]);
    	count++;
      //if(Out1[i*W_Out+j] != Out2[i*W_Out+j]){
      //printf("Error, result of different methods does not correspond!\n");
      //return -1;
      //}
    }
    printf("\nCount ");
  }
  /*printf("%d\nA \n", count);


  for(int i = 0; i < nnz; i++){
    printf("%d ", A[i]);
  }

  printf("\nJA \n");
  for(int i = 0; i < nnz; i++){
    printf("%d ", JA[i]);
  }

  printf("\nIA \n");
  for(int i = 0; i < H_M1 +1; i++){
    printf("%d ", IA[i]);
  }*/

  printf("\n");
  for(int i = 0; i < W_Out; i++){
    int S = 0;
    for(int j = 0; j < IA[i+1] - IA[i]; j++){
      S = S + A[j] * M2[JA[j]];
      //printf("%d ", A[j] * M2[JA[j]]);
    }
    printf("%d ", S);
  }

  for(int i = 0; i < 100; i++){
    printf("%d ", rand()%4);
  }

  printf("Test success\n");

  return 0;
}

