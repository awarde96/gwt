#ifndef __MODELKERNEL_H__
#define __MODELKERNEL_H__

#include "KernelLibStdTypes.h"
#include "ModelKernelsInit.h"
#include "MatMultBasicKernels.h"
#define _L1_Memory_SIZE 4328
#define _L2_Memory_SIZE 0
extern char *L1_Memory; /* Size given for generation: 64000 bytes, used: 4328 bytes */
extern char *L2_Memory; /* Size used for generation: 0 bytes */
extern void ParMatMult(
		Word8 *  __restrict__ M2,
		Word16 *  __restrict__ Out,
		Word8 *  __restrict__ A,
		Word16 *  __restrict__ IA,
		Word16 *  __restrict__ JA,
		Word16 * __restrict__ Bias,
		Kernel_T *Ker);
#endif
