 
#include "NMCS.h"
#include "NRPA.h"
#include "GameRQ.h"
#include "_Sequence.h"

using namespace std;

 
//////////////////////////////////////////////////////////////////////////////////////
//
//     NMCS main function

 

int PlayoutNMCS(GameRQ & G, MyRand & R)
{
	 
	while ( ! G.IsTerminated() )
	{
		IDMov idmove = G.LPossMoves.SelectRandomMove(R);
	 
		G.PlayOneMove(idmove);
	}
 
	return (int) G.GetScore();
}


void DynAff(int threadID, int it, int bestscore, int level, int TopLevel, int NbThreads, int MinLevel);
void DynAffInit();


void _NMCS(GameRQ & G,    MyRand & R,  int TopLevel, int currentLevel, int ThreadID, int NbThreads)
{
	if (currentLevel == 0)  // fully random play
	{
		PlayoutNMCS(G, R);
	}
	else
	{
		_Sequence BestKnownSeq;
		u64		  BestKnowScore = 0;
		int it = 0;

		while (! G.IsTerminated() )
		{
			// evaluate best move among possible moves AND
			// the current Best Know Sequence
			 
			Fori( G.LPossMoves.size() )
			{
				// test idmove
				IDMov idMove = G.LPossMoves.get(i) ;
				
				GameRQ G2 = G;
				G2.PlayOneMove(idMove);
				_NMCS(G2,  R, TopLevel, currentLevel-1, ThreadID, NbThreads);

				// is it a best option ?
			 
				if ( G2.GetScore() > BestKnowScore )
				{
					// we replace the current best know sequence of moves
					BestKnownSeq  = G2.Seq;
					BestKnowScore = G2.GetScore();
				}
			}

			// play the current IDMov from BestKnownSeq
			size_t nbMovesPlayed = G.Seq.size();  // get num 
			IDMov BestMove = BestKnownSeq.get(nbMovesPlayed);
			G. PlayOneMove(BestMove);

			it++;
			DynAff(ThreadID, it, BestKnowScore, currentLevel, TopLevel, NbThreads,3);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
//
//				BENCH




//////////////////////////////////////////////////////////////////////////////////
//
//  THREAD n°ID : starts the computation of a NMCS of the given level
//  Compute at least 1 sequence using 1 thread
//  If sequences are not time-consuming (level 0/1), multiples sequences are 
//  generated for at least MinTime seconds





void   NMCS::OneCore(Data & D, int level, int ID, int NbThreads)
{
	//HANDLE mThread = GetCurrentThread();
	//SetThreadAffinityMask(mThread, 1 << (2*ID) );
	
	D.Start();
	MyRand R;      // random generator PRIVATE to this thread
	R.AutoSeed();  // random seed
	
	 
	int nbprint = 0;

	while (D.TimeElapsedSeconds() < MinTimeSeconds)
	{
		GameRQ G;
		G.Reset();

	 
	 
 		_NMCS(G,   R, level, level, ID, NbThreads);
		
		D.NewSample(G.Seq, G.GetScore());
		
	}

	D.Stop();
	DynAff(0, 0, 0, 0, 0, NbThreads, 3);
}


void AffDistrib(vector<Data> & DL, Data & DCumul, int level, int nbT);

void NMCS::RunBatchMultiCore(int level, Data & Cumul)
{
	DynAffInit();

	cout << "******************************************************" << endl;
	cout << "Score distribution - LEVEL :  " << level << endl;
	 
	vector<thread > thread_pool;
	cout << "Running " << NbThreads << " threads" << endl;

	 
	vector<Data> DataList;
	Fori(NbThreads)
		DataList.push_back(Data());
 

	Fori(NbThreads)
		thread_pool.push_back(thread(& NMCS::OneCore, this, ref(DataList[i]), level, i, NbThreads));

	  
	Fori(NbThreads)
		thread_pool[i].join();
  
	Cumul.AddAllData(DataList);
	 

	AffDistrib(DataList, Cumul, level, NbThreads);
 
} 

