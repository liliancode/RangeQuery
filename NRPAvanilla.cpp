
#include "NRPAvanilla.h"
#include "GameRQ.h"
#include <tuple>
#include <map>
using namespace std;

void DynAff(int threadID, int it, int bestscore, int level, int TopLevel, int NbThreads, int MinLevel);
void DynAffInit();

//////////////////////////////////////////////////////////////////////////////////////
//
//     NRPA   VANILLA


void NRPAvanilla::PlayoutNRPAvanilla(Ptype * W, MyRand & R, _Sequence & OutSeq, u64 & OutScore)
{
	GameRQ  G;
	G.Reset();

	while (!G.IsTerminated())
	{
		IDMov idmove = G.LPossMoves.SelectSoftmaxMove(W, R);
		G.PlayOneMove(idmove);
	}
	OutScore = G.GetScore();
	OutSeq = G.Seq;
}


void NRPAvanilla::AdaptVanilla(Ptype * W, _Sequence & S)
{
	Ptype Polp[32768];
	memcpy(Polp, W, GameRQ::GetMaxIDMov() * sizeof(Ptype));


	GameRQ G;
	G.Reset();
 
	 

	// Adapt each weight relative to NRPA algorithm formula
	Fori(S.size())  
	{
		_ListPossMovesU16 & LChilds = G.LPossMoves;
		 
		IDMov IDplayed = S.get(i);

		size_t nbChilds = LChilds.size();
		
		Ptype Sigma = 0;
		Forj(nbChilds)
		{
			IDMov idmov = LChilds.get(j);
			Sigma += exp(Polp[idmov]);
		}

		Ptype inv = 1 / Sigma;

		W[IDplayed] += alpha;

		Forj(nbChilds)
		{
			IDMov idmov = LChilds.get(j);
			W[idmov] += -alpha * exp(Polp[idmov]) * inv;
		}
		G.PlayOneMove(IDplayed);
	}
}

 

string TimeSinceBeginning();
 

 

void printMovesPS(_Sequence & S);

void NRPAvanilla::nrpaVanilla(vector<Ptype *> & policy, int level, MyRand & R, int TopLevel, _Sequence & OutSeq, u64 & OutScore, int IDThread )
{


	if (level == 0)
	{
		_Sequence Seq;
		u64 score;
		OutScore = 0;
	 	 
		PlayoutNRPAvanilla(policy[0], R, Seq, score );
		if (score > OutScore)
		{
			OutScore = score;
			OutSeq = Seq;
		}
	}
	else
	{
		_Sequence BestSeq;
		u64       BestScore = 0;
		 

		const int N = 100;

		memcpy(policy[level - 1], policy[level], GameRQ::GetMaxIDMov() * sizeof(Ptype));
	 
		Fori(N)
		{

			_Sequence NewSeq;
			u64       NewScore = 0;
			vector<IDMov>  NewNodeHistory;
			vector<Ptype>  NewSigmaHistory;
			nrpaVanilla(policy, level - 1, R, TopLevel, NewSeq, NewScore, IDThread );



			if (NewScore >= BestScore)
				AdaptVanilla(policy[level - 1], NewSeq) ;
			else
				AdaptVanilla(policy[level - 1], BestSeq) ;

			//if ( (NewScore > BestScore) && (NewScore > 175) && (level >= 4) )
			//if ((NewScore > BestScore) && (NewScore > 175) && (level >= 3))
			//	printMovesPS(NewSeq);


			if (BestScore <= NewScore)
			{
				BestSeq = NewSeq;
				BestScore = NewScore;
			}


			DynAff(IDThread, i, BestScore, level, TopLevel,NbThreads, 4);
 
		}
		OutSeq = BestSeq;
		OutScore = BestScore;
	}


}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
//		         	BENCH

 


void   NRPAvanilla::OneCore(Data & D, int level, int ID, int autoseed)
{

	// init prob
	vector<Ptype *> policy;
	Fori(10) policy.push_back(new Ptype[GameRQ::GetMaxIDMov()]);
	Fori(10)  Forj(GameRQ::GetMaxIDMov())  policy[i][j] = PolicyInitValue;

	D.Start();

	MyRand R;      // random generator PRIVATE to this thread
	if (autoseed == 0)
		R.AutoSeed();  // random seed


	int nbprint = 0;

	while (D.TimeElapsedSeconds() < MinTimeSeconds)
	{
		_Sequence BestSeq;
		u64       BestScore;

		nrpaVanilla(policy, level, R, level, BestSeq, BestScore, ID );

		//PrintSeq2(nbprint++, ID, BestSeq);

		D.NewSample(BestSeq, BestScore);
	}

	Fori(10)
		delete[] policy[i];

	D.Stop();

}

void AffDistrib(vector<Data> & DL, Data & DCumul, int level, int nbT);



void NRPAvanilla::RunBatchMultiCore(int level, Data & Cumul)
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
		thread_pool.push_back(thread(&NRPAvanilla::OneCore, this, ref(DataList[i]), level, i, 0));


	Fori(NbThreads)
		thread_pool[i].join();

	Cumul.AddAllData(DataList);

	cout << "Alpha : " << alpha;
	AffDistrib(DataList, Cumul, level, NbThreads);



}


 