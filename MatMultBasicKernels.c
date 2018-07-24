/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap8.h"
#include "MatMultBasicKernels.h"


#define Min(a, b)               (((a)<(b))?(a):(b))

#ifdef OLD_RUNTIME
#define wait_synch_barrier()          eu_bar_trig_wait_clr(eu_bar_addr(0))
#else
#define wait_synch_barrier()          rt_team_barrier()
#endif

static int CoreCountDynamic = 1;
static int ActiveCore = gap8_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
  unsigned int NCore;
  unsigned int Log2Core;
  unsigned int Chunk;

  if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap8_ncore();
  Log2Core = gap8_fl1(NCore);
  Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
  return Chunk;
}

void KerMatMultParallel(KerMatMultParallel_ArgT *Arg)

{
  //short int * __restrict__ In1 = Arg->In1;
  //unsigned int W_In1 = Arg->W_In1;
  //unsigned int H_In1 = Arg->H_In1;
  signed char * __restrict__ In2 = Arg->In2;
  unsigned int W_In2 = Arg->W_In2;
  short int * __restrict__ Out = Arg->Out;
  unsigned int W_Out = Arg->W_Out;
  unsigned int OutFirstCol = Arg->OutFirstCol;
  short int * __restrict__ Bias = Arg->Bias;

  signed char * __restrict__ A = Arg->A;
  short int * __restrict__ IA = Arg->IA;
  short int * __restrict__ JA = Arg->JA;
  short int * __restrict__ row_start = Arg->row_start;

  unsigned int W_In1 = 300;
  unsigned int H_In1 = 10;

  unsigned int H_In2 = W_In1;
  unsigned int H_Out = H_In1;
  unsigned int Line, Col, i;

  unsigned int CoreId = gap8_coreid();
  unsigned int ChunkCell = ChunkSize(W_In2);
  unsigned int First = CoreId*ChunkCell, Last  = Min(W_In2, First+ChunkCell);

  //regualr fully connected version that works fine
  //for (Col=0; Col<W_In2; Col++) {
  /*for (Col=First; Col<Last; Col++) {
    for (Line=0; Line<H_In1; Line++) {
      int S = 0;
      for (i=0; i<W_In1; i++) S = S + In1[Line*W_In1 + i] * In2[i*W_In2+Col];
      //Out[Line*W_Out+Col+OutFirstCol] = S;
    }
  }*/
//  wait_synch_barrier();

  //working version where JA consists of column index's
  /*int acc = 0;
  //int total = H_In1/rt_nb_pe();
    for (Line=0; Line<H_In1; Line++) {
      int S = 0;     
      int prev = 0;
      if((unsigned int) gap8_coreid() == (Line)%8){
        acc = IA[Line];
        //acc = IA[Line+1] - IA[Line];
        for (int i=0; i<IA[Line+1] - IA[Line]; i++) {   
          //prev += JA[acc+i];            
          S += A[i+acc] * In2[JA[(i+acc)]];//In1[Line*W_In1 + i] * In2[i*W_In2+Col];
          //printf("%d ", S);
          //printf("%d\n", In2[i+acc%10]);
        }     
        //Out[Line*W_Out+Col+OutFirstCol] = S;
        S += Bias[Line];
        Out[Line] = S;
      }
      //acc += IA[Line+1] - IA[Line];
    }*/

  //version where JA consists of offsets
  int acc = 0;
  //int total = H_In1/rt_nb_pe();
  //outer loop to allocate each row ro a core
    for (Line=0; Line<H_In1; Line++) {
      // if statement to allcocate the row to the core
      if((unsigned int) gap8_coreid() == (Line)%8){
        int prev = row_start[Line];
        int S = 0;     //initialise the sum for that row
        acc = IA[Line]; //set the accumulator to the first JA entry for the row from IA
        //acc = IA[Line+1] - IA[Line];
        for (int i=0; i</*W_In1*/IA[Line+1] - IA[Line]; i++) {        
          prev += JA[acc+i];      
          S += A[i+acc] * In2[prev - (Line*W_In1)];//In1[Line*W_In1 + i] * In2[i*W_In2+Col]; 
        }     
        S += Bias[Line];
        Out[Line] = S;
      }
      //acc += IA[Line+1] - IA[Line];
    }


  /*for(Col=First; Col<Last; Col++){
    for(unsigned int i = 0; i < W_Out; i++){
      int S = 0;
      for(int j = 0; j < IA[i+1] - IA[i]; j++){
        S = S + A[j] * In2[JA[j]];
      }
      //Out[i] = S;
    }
  }*/
}


