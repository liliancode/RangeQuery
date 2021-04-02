#pragma once
 

#include "Type.h"
#include <immintrin.h>
#include <emmintrin.h>

#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align (x))
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif
#endif


class MyRand   // rand using SSE2
{
	 
 	u32 Buffer[32];
	int PosInBuffer = 0;
	ALIGNED_(16) __m128i cur_seed = _mm_setr_epi32(1, 2, 3, 4);  
 	void rand__sse2(u32* result);  
	
public:
	 
	void   AutoSeed();       //   generate REAL random seed
	
	u32    GetRandomU32();   //   unsigned int 32

	float  GetRand_01();     //   random between [0,1]

	static void  Bench();

};
