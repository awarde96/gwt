#include "ModelKernelsInit.h"
#include "ModelKernels.h"
void ParMatMult(
		Word16 *  __restrict__ M1,
		Word16 *  __restrict__ M2,
		Word16 *  __restrict__ Out,
		Word16 *  __restrict__ A,
		Word16 *  __restrict__ IA,
		Word16 *  __restrict__ JA,
		Kernel_T *Ker)

{
	/* Local variables used by this kernel */
	rt_dma_copy_t DmaR_Evt1;
	rt_dma_copy_t DmaR_Evt2;
	rt_dma_copy_t DmaR_Evt3;
	rt_dma_copy_t DmaR_Evt4;
	rt_dma_copy_t DmaR_Evt5;
	rt_dma_copy_t DmaW_Evt1;
	int Iter;
	int Last, NextLast, NextNextLast;
	int N_Ti = 0;
	int Iter1;
	int Last1, NextLast1, NextNextLast1;
	int N_Ti1 = 0;
	int N_TiIp = 0, InPlane, OutPlane=0;
	KerMatMultParallel_ArgT S_KerArg0, *KerArg0 = &S_KerArg0;

	/* Initialize KerArg, Kernel invariant arguments */
	KerArg0->W_In1 = (unsigned int) (20);
	KerArg0->H_In1 = (unsigned int) (20);
	KerArg0->W_In2 = (unsigned int) (1);
	KerArg0->W_Out = (unsigned int) (1);
	KerArg0->BufferColIn2 = (short int * __restrict__) (0);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	rt_dma_memcpy((rt_pointerT) M1+(0), (rt_pointerT) (L1_Memory + 80)+0, 800, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	rt_dma_memcpy_2d((rt_pointerT) M2+(0), (rt_pointerT) (L1_Memory + 0)+0, 40, 
		2, 2, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt2);
	rt_dma_memcpy((rt_pointerT) A+(0), (rt_pointerT) (L1_Memory + 1760)+0, 400, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt3);
	rt_dma_memcpy((rt_pointerT) IA+(0), (rt_pointerT) (L1_Memory + 2560)+0, 42, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt4);
	rt_dma_memcpy((rt_pointerT) JA+(0), (rt_pointerT) (L1_Memory + 2648)+0, 400, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt5);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on Iter1 space */
	Iter1=0; {
		/* Loop Iteration Body on Iter1 space */
		/* Elaborate Last, Next_Last, Next_Next_Last */
		Last1 = ((Iter1+1) == 1); NextLast1 = ((Iter1+2) == 1); NextNextLast1 = ((Iter1+3) == 1);
		/* =======================Read Next Tile=========================================== */
		rt_dma_wait(&DmaR_Evt1);
		rt_dma_wait(&DmaR_Evt3);
		rt_dma_wait(&DmaR_Evt4);
		rt_dma_wait(&DmaR_Evt5);
		if (!Last1) {
			rt_dma_memcpy((rt_pointerT) M1 + ((0+1)*800),
					(rt_pointerT) (L1_Memory + 80) + (800*((N_Ti1+1) % 2)), 800, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
			rt_dma_memcpy((rt_pointerT) A + ((0+1)*400),
					(rt_pointerT) (L1_Memory + 1760) + (400*((N_Ti1+1) % 2)), 400, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt3);
			rt_dma_memcpy((rt_pointerT) IA + ((0+1)*42),
					(rt_pointerT) (L1_Memory + 2560) + (44*((N_Ti1+1) % 2)), 42, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt4);
			rt_dma_memcpy((rt_pointerT) JA + ((0+1)*400),
					(rt_pointerT) (L1_Memory + 2648) + (400*((N_Ti1+1) % 2)), 400, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt5);
		}
		/* ===================End Read Next Tile=========================================== */
		/* Kernel Iteration Loop on Iter space */
		Iter=0; {
			/* Loop Iteration Body on Iter space */
			/* Elaborate Last, Next_Last, Next_Next_Last */
			Last = ((Iter+1) == 1); NextLast = ((Iter+2) == 1); NextNextLast = ((Iter+3) == 1);
			/* =======================Read Next Tile=========================================== */
			rt_dma_wait(&DmaR_Evt2);
			if (!Last) {
				rt_dma_memcpy_2d((rt_pointerT) M2 + ((0+1)*2),
						(rt_pointerT) (L1_Memory + 0) + (40*((N_Ti+1) % 2)), 40, 
						2, 2, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt2);
			}
			else if (!Last1) {
				rt_dma_memcpy_2d((rt_pointerT) M2 + (0),
						(rt_pointerT) (L1_Memory + 0) + (40*((N_Ti+1) % 2)), 40, 
						2, 2, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt2);
			}
			/* ===================End Read Next Tile=========================================== */
			/* Call Kernel LOC_INNER_LOOP */
			KerArg0->In1 = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 80) + 800*((N_Ti1) % 2));
			KerArg0->In2 = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 0) + 40*((N_Ti) % 2));
			KerArg0->Out = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 1680) + 40*((N_Ti1) % 2));
			KerArg0->OutFirstCol = (unsigned int) (0)*1;
			KerArg0->A = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 1760) + 400*((N_Ti1) % 2));
			KerArg0->IA = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 2560) + 44*((N_Ti1) % 2));
			KerArg0->JA = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 2648) + 400*((N_Ti1) % 2));
			rt_team_fork(gap8_ncore(), (void *) KerMatMultParallel, (void *) KerArg0);
			N_Ti++;
			/* End Kernel Iteration Loop on Iter space */
		}
		Iter=1;
		/* =======================Write Tile=========================================== */
		if (0) {
			rt_dma_wait(&DmaW_Evt1);
		}
		rt_dma_memcpy((rt_pointerT) Out + ((0)*40),
			(rt_pointerT) (L1_Memory + 1680) + (40*(N_Ti1 % 2)), 40, RT_DMA_DIR_LOC2EXT, 0, &DmaW_Evt1);
		/* ===================End Write Tile=========================================== */
		N_Ti1++;
		/* End Kernel Iteration Loop on Iter1 space */
	}
	Iter1=1;
	/* =======================Write Last Tile=========================================== */
	rt_dma_wait(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}

