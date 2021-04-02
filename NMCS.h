#pragma once
#include "Type.h"
 
#include "_ListPossMovesU16.h"
#include "_Board.h"
#include "Rand.h"
#include "Data.h"





class NMCS
{
	// stats and perf

	bool AffRunning = false;
	
	u32 NbThreads;


public:
	 
	const int MinTimeSeconds = 10;


	NMCS(u32 NbThreads) : NbThreads(NbThreads) {}
	 
	 
	void  OneCore(Data & D, int level, int ID, int NbThreads);
	 
	 
	void RunBatchMultiCore(int level, Data & Cumul);
	 
	
	 
};
