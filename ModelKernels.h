#ifndef __MODELKERNEL_H__
#define __MODELKERNEL_H__

#include "KernelLibStdTypes.h"
#include "ModelKernelsInit.h"
#include "MatMultBasicKernels.h"
#define _L1_Memory_SIZE 3448
#define _L2_Memory_SIZE 0
extern char *L1_Memory; /* Size given for generation: 40000 bytes, used: 3448 bytes */
extern char *L2_Memory; /* Size used for generation: 0 bytes */
extern void ParMatMult(
		Word16 *  __restrict__ M1,
		Word16 *  __restrict__ M2,
		Word16 *  __restrict__ Out,
		Word16 *  __restrict__ A,
		Word16 *  __restrict__ IA,
		Word16 *  __restrict__ JA,
		Kernel_T *Ker);
#endif
