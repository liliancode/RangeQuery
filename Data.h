#pragma once
#include <sstream>
#include "Type.h"
#include "_Sequence.h"

#define _NbMoves 65536

#define MaxScore 240

struct Data
{
private :
	Horloge  Clock;
	_Sequence BestSeq;
	int       BestScore;
	u64      Distribution[MaxScore];
	double   TimeUsedForThisRunSeconds;

public:


	_Sequence GetBestSeq()       { return BestSeq;  }
	double GetTimeUsedSeconds() { return TimeUsedForThisRunSeconds; }
	int GetBestScore()               { return BestScore; }
	void Start()                  { Clock.Start();  }
	double TimeElapsedSeconds()   { return Clock.TimeElapsedSeconds();  }
	void Stop()                   { TimeUsedForThisRunSeconds = Clock.TimeElapsedSeconds(); }
	 
	
	void NewSample(_Sequence & NewSeq, int newScore)
	{
		Distribution[newScore]++;
		if (BestScore < newScore)
		{
			BestSeq = NewSeq;
			BestScore = newScore;
		}
	}
	 
	void AddAllData(vector<Data> & LD)
	{
		Fori(LD.size())
			AddData(LD[i]);
	}

	void AddData(const Data & D)
	{
		Fori(MaxScore)
	  	  Distribution[i] += D.Distribution[i];

		TimeUsedForThisRunSeconds += D.TimeUsedForThisRunSeconds;


		if (D.BestScore > BestScore)
		{
			BestSeq = D.BestSeq;
			BestScore = D.BestScore;
		}	
	}


	u64 TotalRuns()
	{
		u64 tot = 0;
		Fori(MaxScore)
			tot += Distribution[i];
		return tot;
	}

	float MeanScore()
	{
		float mean = 0;
		Fori(MaxScore)
			mean += Distribution[i] * i;
		return mean / TotalRuns();
	}

	Data()
	{
		Fori(MaxScore) Distribution[i] = 0;
		TimeUsedForThisRunSeconds = 0;
		BestScore = 0;
	}

	void PrintDistrib(stringstream & SB)
	{
		int nbRows = 20;
		int nbCols = 12;
		SB << endl;
		Fori32(nbRows)
		{
			Forj32(nbCols)
			{
				int m = i + j * nbRows;
				if ( m < MaxScore)
					SB << "S" << k(m, 2) << ": " << k(Distribution[m], 7) << " | ";
			}
			SB << endl;
		}
	}

};
