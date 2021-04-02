#pragma once
#pragma once

#pragma once
#include "Type.h"

#include "_ListPossMovesU16.h"
#include "_Board.h"
#include "Rand.h"
#include "Data.h"

class NRPAvanilla
{

	u32    NbThreads;
	Ptype  alpha = 1;
	Ptype  PolicyInitValue = 0;        // weights initialized at 0

	

	void nrpaVanilla(vector<Ptype *> & policy, int level, MyRand & R, int TopLevel, _Sequence & OutSeq, u64 & OutScore, int IDThread);
	void PlayoutNRPAvanilla(Ptype * W, MyRand & R, _Sequence & OutSeq, u64 & OutScore);
	void AdaptVanilla(Ptype * W, _Sequence & S);

public:

	const int MinTimeSeconds = 10;
	  

	NRPAvanilla(u32 NbThreadss, Ptype Alpha) : NbThreads(NbThreadss) { NRPAvanilla::alpha = Alpha; }

	void   OneCore(Data & D, int level, int ID, int autoseed = 0);

	void   RunBatchMultiCore(int level, Data & Cumul);


};
