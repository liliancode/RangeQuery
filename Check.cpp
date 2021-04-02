#include "Type.h"
#include "GameRQ.h"
#include "GameBasic.h"
#include <algorithm>
 
// compare two list of moves - moves may be at different orders

bool CheckEqual(const vector<NaiveMove> & L1, const  vector<NaiveMove> & L2)
{
	Fori(L1.size())
	{
		bool found = false;
		
		Forj( L2.size() )
			if (L1[i] == L2[j]) 
				found = true;
		
		if (not found)
		{
			cout << "ERROR" << L1[i].dir << " "  << L1[i].x << " " << L1[i].y << " " << L1[i].PosHole  << endl; 
			return false;
		}
	}

	Fori(L2.size())
	{
		bool found = false;

		Forj( L1.size() )
			if (L2[i] == L1[j])
				found = true;

		if (not found)
		{
			cout << "ERROR" << L2[i].dir << " " << L2[i].x << " " << L2[i].y << " " << L2[i].PosHole << endl; 
			return false;
		}
	}
	return true;
}

void AffBoard(_Board & B);
void AffAdvancedMoves(_ListPossMovesU16 & PossibleMoves, int idmoveJustPlayed);
void AffNaiveMoves(vector<NaiveMove> & PossibleMoves, NaiveMove idmoveJustPlayed);
extern u8  MoveIsPossibleAndHoleShift_S5[1024];

// run a game and check results with brute force
void CheckGame(MyRand & R, GameRQ & G,   bool debug = true)
{

	vector<NaiveMove> LStart;
	GameBruteForce::GetPossibleMoves(G.B, LStart);

	if (G.LPossMoves.size() != LStart.size())
	{
		cout << "Error wrong list size at startup" << endl;
		AffBoard(G.B);
		AffNaiveMoves(LStart, 0);

		WaitForKey();
	}
	 
	while ( ! G.IsTerminated()  )
	{
		// choose random move and plays
		 
		IDMov idmove = G.LPossMoves.SelectRandomMove(R);

		G.PlayOneMove(idmove);

		//AffBoard(G.B);
		//AffAdvancedMoves(G.LPossMoves, idmove);


		// translate Advanced IdMove => Naive IdMove
		vector<NaiveMove> AdvancedMovesL;
		Fori(G.LPossMoves.size())
		{
			int dir, x, y;
			_Board::DecodeMove(G.LPossMoves.get(i), dir, x, y);
			int S5 = G.B.Get5_5D5T(dir, x, y);

			int HoleShift =  MoveIsPossibleAndHoleShift_S5[S5] >> 1;
			AdvancedMovesL.push_back(NaiveMove(dir,x,y, HoleShift));
		}
		

		// get idmove list using brute force
		vector<NaiveMove> LMove;
		GameBruteForce::GetPossibleMoves(G.B, LMove);

		// compare two lists
		if (CheckEqual(LMove, AdvancedMovesL) == false)
		{
			 AffBoard(G.B); 
			 AffAdvancedMoves(G.LPossMoves, idmove);

			 cout << "^^" << endl << "vv Basic Board" << endl;
			 AffBoard(G.B);
			 AffNaiveMoves(LMove,0);

			 WaitForKey();
		}
	
	 
	} 	 
}

// batch testing

void CheckAdvancedMethod()
{
	 
	cout << "\n******************************************************" << endl;
	cout << "Start Validation " << endl;
	MyRand R;
	 
	  

	for (int i = 0 ; i < 100000 ; i++)
	{
		if ( i % 1000 == 0)
		   cout << i << "/" << 100000 << endl;
 
		GameRQ G;
		G.Reset();

		CheckGame(R, G);
		 
	}
	 
 
}