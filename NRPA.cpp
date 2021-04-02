 
#include "NRPA.h"
#include "GameRQ.h"
 
using namespace std;
void DynAff(int threadID, int it, int bestscore, int level, int TopLevel, int NbThreads, int MinLevel);
void DynAffInit();
 
 
//////////////////////////////////////////////////////////////////////////////////////
//
//     NRPA   main function



void PlayoutNRPA(Ptype * W, MyRand & R, _Sequence & OutSeq, u64 & OutScore,vector<IDMov> & NodeHistory)
{
	GameRQ  G;
	G.Reset();
	 
	NodeHistory.reserve(2048);
	while (!G.IsTerminated())
	{
		Ptype sigma;
		IDMov idmove = G.LPossMoves.SelectWeightedMove(W, R, sigma);
		
		// save all child data
		 
		NodeHistory.push_back(idmove);
		NodeHistory.push_back((IDMov) G.LPossMoves.size());
		Fori(G.LPossMoves.size())
			NodeHistory.push_back(G.LPossMoves.get(i));


		G.PlayOneMove(idmove);
	}
	OutScore = G.GetScore();
	OutSeq   = G.Seq;
}
 
 

void NRPA::Adapt(Ptype * W, _Sequence & S, vector<IDMov> & NodeHistory )
{
	vector<Ptype> CopyProb;
	CopyProb.resize(NodeHistory.size());
	vector<Ptype> LSigma;
	 
	 
	   
	// copy values before updating - compute sigma
	size_t p = 0; 
	size_t depth = 0;  // the ""i-th"" child
	while (p < NodeHistory.size())
	{
		IDMov  IDplayed = NodeHistory[p++];
		size_t nbChilds = NodeHistory[p++];

	    Ptype Sigma = 0;
		 
		Forj(nbChilds)
		{
			Ptype pb = W[NodeHistory[p]];
			CopyProb[p] = pb;
			Sigma += pb;
			p++;
		}
		depth++;
		LSigma.push_back(Sigma);
	}

	// replay sequence S and adapt each weight relative to NRPA algorithm 
	 
	p = 0;
	depth = 0;
	while (p < NodeHistory.size())
	{
	 	IDMov  IDplayed = NodeHistory[p++];
		size_t nbChilds = NodeHistory[p++];
		 
		Ptype inv = 1 / LSigma[depth];
		  
		W[IDplayed] *= exp(alpha);
		Forj(nbChilds)
		{
			IDMov  idmov = NodeHistory[p];
			W[idmov] *= exp(-alpha * CopyProb[p] * inv);
			p++;
		}
			
		depth++;
	}
	   
}





 
  
void printMovesPS(_Sequence & S);


void NRPA::nrpa(vector<Ptype *> & policy, int level, MyRand & R, int TopLevel, _Sequence & OutSeq, u64 & OutScore, int IDThread, vector<IDMov> & NodeHistory)
{
	
	 
	if (level == 0)
	{ 
		_Sequence Seq;
		u64 score;
		OutScore = 0;
		Fori(1)
		{
			vector<IDMov>  NewNodeHistory;
			vector<Ptype>  NewSigmaHistory;
  		   PlayoutNRPA(policy[0],R, Seq, score, NewNodeHistory );
		   if (score > OutScore)
		   {
			   OutScore = score;
			   OutSeq = Seq;
			   NodeHistory = NewNodeHistory;
		   }
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
  			nrpa(policy, level - 1, R, TopLevel,NewSeq,NewScore, IDThread, NewNodeHistory );
			 
			

			if (NewScore >= BestScore   )
				  Adapt(policy[level - 1], NewSeq, NewNodeHistory ) ;
			else 
			 	 Adapt(policy[level - 1], BestSeq, NodeHistory ) ;
					
			  

			if (BestScore  <= NewScore)
			{
				BestSeq   = NewSeq;
				BestScore = NewScore; 
				NodeHistory = NewNodeHistory;	 
			}
				 
				 
			DynAff(IDThread, i, BestScore,level,TopLevel, NbThreads, 4);
			 
			
			
			
		}
		OutSeq   = BestSeq;
		OutScore = BestScore;
	}

	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
//		         	BENCH


 
/*
void PrintSeq2(int nbprint, int ID, _Sequence & S)
{
	if (nbprint < 2)
	{
	 
			cout << "T" << k(ID, 2) << "_" << k(S.size(), 2) << " n" << nbprint << ": " << S.ToString() << endl;
	 
	}
}*/


void   NRPA::OneCore(Data & D, int level, int ID, int autoseed )
{
	 
	// init prob
	vector<Ptype *> policy;
	Fori(10) policy.push_back(new Ptype[GameRQ::GetMaxIDMov()]);
	Fori(10)  Forj(GameRQ::GetMaxIDMov())  policy[i][j] = NRPA::PolicyInitValue;

	D.Start();

	MyRand R;      // random generator PRIVATE to this thread
	if ( autoseed == 0)
  	   R.AutoSeed();  // random seed


	int nbprint = 0;

	while (D.TimeElapsedSeconds() < MinTimeSeconds)
	{
		_Sequence BestSeq;
		u64       BestScore;
		
		

		vector<IDMov>  NewNodeHistory;
		 
		nrpa(policy, level, R, level, BestSeq, BestScore,ID, NewNodeHistory );

		//PrintSeq2(nbprint++, ID, BestSeq);

		D.NewSample(BestSeq, BestScore);
	}

	Fori(10) 
		delete[] policy[i];

	D.Stop();
	 
}
 
void AffDistrib(vector<Data> & DL, Data & DCumul, int level, int nbT);



void NRPA::RunBatchMultiCore(int level, Data & Cumul)
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
		thread_pool.push_back(thread(& NRPA::OneCore, this, ref(DataList[i]), level, i,0));
	 

	Fori(NbThreads)
		thread_pool[i].join();

	Cumul.AddAllData(DataList);
 
	cout << "Alpha : " << NRPA::alpha;
	AffDistrib(DataList, Cumul, level, NbThreads);
	 
}
 
  