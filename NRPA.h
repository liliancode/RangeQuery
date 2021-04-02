#pragma once

#pragma once
#include "Type.h"

#include "_ListPossMovesU16.h"
#include "_Board.h"
#include "Rand.h"
#include "Data.h"

class NRPA
{
	  
	u32    NbThreads;
	Ptype  alpha = 1;
	Ptype  PolicyInitValue = 1;        // weights initialized at exp(0)

	 
	void nrpa(vector<Ptype *> & policy, int level, MyRand & R, int TopLevel, _Sequence & OutSeq, u64 & OutScore, int IDThread, vector<IDMov> & NodeHistory);
	void Adapt(Ptype * W, _Sequence & S, vector<IDMov> & NodeHistory);

public :

	 const int MinTimeSeconds = 10;
	 
	 NRPA(u32 NbThreadss, Ptype Alpha) : NbThreads(NbThreadss) { NRPA::alpha = Alpha;  }

	 void   OneCore(Data & D, int level, int ID, int autoseed = 0);
  
	 void  RunBatchMultiCore(int level, Data & Cumul);
	  

};
 