#include <algorithm>
#include <bitset>
#include "_Board.h"
#include "GameRQ.h"
#include "Rand.h"
#include "GameBasic.h"
#include "_ListPossMovesU16.h"
#include "Data.h"
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   Precomputation: MoveIsPossibleAndHoleShift_S5 >> array used to quickly detect new possible moves on 5 squares
//

// precomputation of the valid move and Holeshift associated to 5 squares
// 5 squares, 2 bits per square => 10 bits  => 1024 configurations



// value returned :
// bit 0  : possible move found (1 = yes)
// bit 1+ : value of the holeshift

// example :
// 01 01 00 01 01   => 2 dots, one hole, 2 dots 
// move is possible because no segment is present and we have 4 dots and 1 hole
// the HoleShift for this move is 2
// return value : 2 << 1 | 1


// 
u8 MoveIsPossibleAndHoleShift_S5[1024];

void GameRQ::PreCompute()
{
	Fori(1024) MoveIsPossibleAndHoleShift_S5[i] = 0;

	//5T & 5D idem

	MoveIsPossibleAndHoleShift_S5[0b0101010100] = (0 << 1) | 1;
	MoveIsPossibleAndHoleShift_S5[0b0101010001] = (1 << 1) | 1;
	MoveIsPossibleAndHoleShift_S5[0b0101000101] = (2 << 1) | 1;
	MoveIsPossibleAndHoleShift_S5[0b0100010101] = (3 << 1) | 1;
	MoveIsPossibleAndHoleShift_S5[0b0001010101] = (4 << 1) | 1;

}
 

/////////////////////////////////////////////////////////////////////////////
//
//			Reset and Precomputation
//
//
 

// build List of Idmoves at startup
_ListPossMovesU16 GameRQ::ListPossMoveAtStartup;
_ListPossMovesU16 InitStartupMoveFromNaive()
{

	_Board B;
	B.Reset();

	// look for moves with naive approach
	vector<NaiveMove>   List;
	GameBruteForce::GetPossibleMoves(B, List);

	// encode them 
	vector<IDMov> LIdMove;
	for (NaiveMove M : List)
	{
		//int pXY = Board::GetPos(M.x, M.y);
		//IDMov idM = pUV_d_pXY[M.dir][pXY] + M.PosHole;

		IDMov idM = _Board::CreateIDMove(M.dir, M.x, M.y);
		LIdMove.push_back(idM);
	}
	std::sort(LIdMove.begin(), LIdMove.end());

	// send to shortlist
	_ListPossMovesU16 LIdMoveStart;
	for (IDMov i : LIdMove)
		LIdMoveStart.push_back(i);
	return LIdMoveStart;
}

IDMov GameRQ::MaxIDMOV;
void  GameRQ::ComputeMaxIdMove()
{
	int maxID = 0;
 
	for (int dir = 0; dir < 4; dir++)
	{ 
		for (int x = 0; x < WIDTH_TOTAL; x++)
			for (int y = 0; y < WIDTH_TOTAL; y++)
				if (_Board::Get5OK(dir, x, y))
				{
					IDMov ID = _Board::CreateIDMove(dir, x, y);
					if (ID > maxID)
						maxID = ID;
				}
	}
	MaxIDMOV = maxID + 10;
	cout << "Max ID move: " << MaxIDMOV << endl;
}


void GameRQ::staticInit()
{
	ListPossMoveAtStartup = InitStartupMoveFromNaive();
	PreCompute();
	ComputeMaxIdMove();
}


 
void GameRQ::Reset()
{
	LPossMoves = ListPossMoveAtStartup;
	B.Reset();
	Seq.Clear();
}

void AffBoard(_Board & B);
void AffAdvancedMoves(_ListPossMovesU16 & PossibleMoves, int idmoveJustPlayed);
_ListPossMovesU16 InitStartupMoveFromNaive();
  
 
void GameRQ::PlayOneMove(IDMov idmove)
{
	Seq.push_back(idmove);
	 
	int dir, x, y;
	_Board::DecodeMove(idmove, dir, x, y);

	int S5 = B.Get5_5D5T(dir, x, y);

	int HoleShift = MoveIsPossibleAndHoleShift_S5[S5] >> 1;

	B.Play(dir, x, y);
	int xHole, yHole;

	B.GetHoleXY(dir, x, y, HoleShift, xHole, yHole);

	// Update moves
	 
	 
	int S9[4];
	B.Get9x4Dirs(S9, xHole,yHole);
	
	// 5T
	// SUPPR invalidated MOVES
	IDMov IDmovH[4];
	_Board::Create4IDMove(IDmovH, xHole, yHole);
	 

	Fori(4)  // remove moves near the played dot
	{
		//
		IDMov Max = IDmovH[i]; 
		IDMov Min = Max - 4;
		LPossMoves.SupprIDmovesByRange(Min, Max);
		 
	}

	 

	// 5T
	// remove moves linked to the line

	IDMov FirstIDMove = idmove;
	IDMov bounds[] = { 4,3 };
	IDMov Min = FirstIDMove - bounds[V5D_5T];
	IDMov Max = FirstIDMove + bounds[V5D_5T];
	LPossMoves.SupprIDmovesByRange(Min, Max);
	 

	// PACK non-zero values

	LPossMoves.Pack();

	 
	  
	// ADD new IDMoves

	Fori(4)
	{
		
		IDMov FirstIDMove = IDmovH[i];
		 
		IDMov Min = FirstIDMove - 4;
		
		// add new
		int S = S9[i];		// retrieve the information of 9 consecutive squares

		 
		// process a block of 5 squares and detect a config
		int code1 = MoveIsPossibleAndHoleShift_S5[(S >> 0) & Filter5D5T];
		LPossMoves.AddIf(Min + 0, code1 & 1);

		// process next block of 5 squares
		int code2 = MoveIsPossibleAndHoleShift_S5[(S >> 2) & Filter5D5T];
		LPossMoves.AddIf(Min + 1, code2 & 1);

		int code3 = MoveIsPossibleAndHoleShift_S5[(S >> 4) & Filter5D5T];
		LPossMoves.AddIf(Min + 2, code3 & 1);

		int code4 = MoveIsPossibleAndHoleShift_S5[(S >> 6) & Filter5D5T];
		LPossMoves.AddIf(Min + 3, code4 & 1);

		int code5 = MoveIsPossibleAndHoleShift_S5[(S >> 8) & Filter5D5T];
		LPossMoves.AddIf(Min + 4, code5 & 1);
		 
	}

}

  
 

///////////////////////////////////////////////////////////////////////////////////////////////
//
//    DEMO and Test



int GameRQ::Demo(MyRand & R, bool debug)
{
	GameRQ G;
	G.Reset();
	 

	if (debug)
	{
		AffBoard(G.B);
		AffAdvancedMoves(G.LPossMoves, 0);
	}


	while ( ! G.IsTerminated() )
	{
		if (debug) WaitForKey();

		// choose random move and plays
		  
		Ptype sigma;
		IDMov idmove = G.LPossMoves.SelectRandomMove( R);

		G.PlayOneMove(idmove);

		if (debug) { AffBoard(G.B); AffAdvancedMoves(G.LPossMoves, idmove); }
 
	}
	return G.GetScore();
}
 



void GameRQ::BenchRQ(int mode)
{
	cout << "******************************************************" << endl;
	cout << "Start BENCH  RQ " << endl;
	Horloge H;

	MyRand R;
	Ptype * weight = new Ptype[100000];
	Fori(100000) weight[i] = 1;
	
	H.Start();

	GameRQ G;
	 
	int NbRounds = 0;
	int nbScenar = 0;
	int BestScore = 0;
	while (H.TimeElapsedSeconds() < 4) // need at least 3 sec of tests
	//for (int  k = 0 ; k < 10000 ; k++)
		for (int i = 0; i < 100; i++)
		{
			Ptype sigma;
		    // run a game
			G.Reset();
			while (!G.IsTerminated())
			{
				IDMov idmove;
				if (mode == 0) idmove = G.LPossMoves.SelectRandomMove(R);
				if (mode == 1) idmove = G.LPossMoves.SelectWeightedMove(weight, R, sigma);
				if (mode == 2) idmove = G.LPossMoves.SelectSoftmaxMove(weight, R);
				
			 
				G.PlayOneMove(idmove);
			}
			int score = G.GetScore();
			NbRounds += score;
			nbScenar++;
			BestScore = BestScore < score ? score : BestScore;
		}

	double dt_sec = H.TimeElapsedSeconds();
	double proc_speed = CPUSpeed * 1000 * 1000 * 1000;
	double Totcycles = dt_sec * proc_speed;
	int CyclesPerRound = (int)(Totcycles / NbRounds);

	cout << " =>  Total Cycles     : " << Totcycles << endl;
	cout << " =>  Total Moves      : " << NbRounds << endl;
	cout << " =>  Cycles per round : " << CyclesPerRound << endl;
	cout << " =>  Rounds per game  : " << NbRounds / nbScenar << endl;
	cout << " =>  Best Score found : " << BestScore << endl << endl;

}
  
 