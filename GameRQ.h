#pragma once
#include "Type.h"
#include "Rand.h"
#include "_Board.h"
#include "_ListPossMovesU16.h"
#include "_Sequence.h"

 

class GameRQ  // Range Query Method
{
private:
 
	 
	static _ListPossMovesU16 ListPossMoveAtStartup;
	static void PreCompute();
	static IDMov MaxIDMOV;
	static void  ComputeMaxIdMove();

	
public:

	_Board         B;		         // Current State of the GameBoard
	_ListPossMovesU16 LPossMoves;       // List of the posibles moves  -  optimized for speed
	_Sequence      Seq;              // history of the played moves
	  
	   
	void Reset();
	
	void PlayOneMove(IDMov idmove);

	bool IsTerminated() { return LPossMoves.size() == 0;  }

	u64  GetScore()     { return Seq.size(); }

	static IDMov	GetMaxIDMov() { return GameRQ::MaxIDMOV; }


	static void staticInit();




	

	///////////////////////////////////////////////////////////////////////
	//
	// TEST

	static int  Demo(MyRand & R, bool debug);
	static void BenchRQ(int mode);
	 
};

 
 