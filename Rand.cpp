#include "Rand.h"
#include <immintrin.h>
#include <emmintrin.h>
#include <iostream>
 

void MyRand::AutoSeed()
{
	u32 z1, z2, z3, z4;
	_rdrand32_step(&z1);
	_rdrand32_step(&z2);
	_rdrand32_step(&z3);
	_rdrand32_step(&z4);
	 
	cur_seed = _mm_setr_epi32(z1, z2, z3, z4);
}
 
 

///////////////////////////////////////////////////////////////////////
//
//   INTEL SIMD RANDOM
//
//  https://software.intel.com/content/www/us/en/develop/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor.html
//

float MyRand::GetRand_01()
{
	float t = (float) GetRandomU32();
	const u32 mx = -1;
	const float mxx = mx;
	float v = t / mxx;
	if (v == 1)
		  return v;
	return v;
}

u32 MyRand::GetRandomU32()
{ 
	if (PosInBuffer == 0)
	{
		rand__sse2(Buffer);
		rand__sse2(Buffer+4);
		rand__sse2(Buffer+8);
		rand__sse2(Buffer+12);
	}
	int t = PosInBuffer;
	PosInBuffer = (PosInBuffer + 1) & 15; // modulo %16
	return Buffer[t];
}



void MyRand::rand__sse2(u32* result)
{
	ALIGNED_(16) __m128i cur_seed_split;
	ALIGNED_(16) __m128i multiplier;
	ALIGNED_(16) __m128i adder;
	ALIGNED_(16) __m128i mod_mask;
 
	 
	ALIGNED_(16) static const unsigned int mult[4] = { 214013, 17405, 214013, 69069 };
	ALIGNED_(16) static const unsigned int gadd[4] = { 2531011, 10395331, 13737667, 1 };
	ALIGNED_(16) static const unsigned int mask[4] = { 0xFFFFFFFF, 0, 0xFFFFFFFF, 0 };
	//ALIGNED_(16) static const unsigned int masklo[4] = { 0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF };
	
	adder = _mm_load_si128((__m128i*) gadd);
	multiplier = _mm_load_si128((__m128i*) mult);
	mod_mask = _mm_load_si128((__m128i*) mask);
 
	cur_seed_split = _mm_shuffle_epi32(cur_seed, _MM_SHUFFLE(2, 3, 0, 1));
	cur_seed = _mm_mul_epu32(cur_seed, multiplier);
	multiplier = _mm_shuffle_epi32(multiplier, _MM_SHUFFLE(2, 3, 0, 1));
	cur_seed_split = _mm_mul_epu32(cur_seed_split, multiplier);
	cur_seed = _mm_and_si128(cur_seed, mod_mask);
	cur_seed_split = _mm_and_si128(cur_seed_split, mod_mask);
	cur_seed_split = _mm_shuffle_epi32(cur_seed_split, _MM_SHUFFLE(2, 3, 0, 1));
	cur_seed = _mm_or_si128(cur_seed, cur_seed_split);
	cur_seed = _mm_add_epi32(cur_seed, adder);
	_mm_storeu_si128((__m128i*) result, cur_seed);
}
 

void MyRand::Bench()
{
	MyRand __R;
	u32 t = 0;

	cout << "Perf of MyRand  " << endl;

	Horloge H;
	H.Start();

	u64 nbScenar = 0;
	while (H.TimeElapsedSeconds() < 3) // need at least 3 sec of tests
		for (int i = 0; i < 100; i++)
		{
			t += __R.GetRandomU32();
			nbScenar++;
		}

	double dt_sec = H.TimeElapsedSeconds();
	double proc_speed = CPUSpeed * 1000 * 1000 * 1000;
	double Totcycles = dt_sec * proc_speed;


	cout << " =>  Cycles per rand  : " << Totcycles / nbScenar << endl;

	cout << t << endl;

}