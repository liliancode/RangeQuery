#pragma once
#include "_Board.h"
#include "Rand.h"

// Brute force algorithm used to check RQ method


struct NaiveMove
{
	NaiveMove(int _dir = 0, int _x = 0, int _y = 0, int _PosHole = 0) : dir(_dir),
		x(_x), y(_y), PosHole(_PosHole) {}

	int dir;
	int x;
	int y;
	int PosHole;
	string ToString();
};
 
bool operator == (const NaiveMove& a, const NaiveMove & b);

class GameBruteForce
{

public :

   // analyze the whole board and return the list of possible moves
   static void  GetPossibleMoves(_Board & B, vector<NaiveMove> & List);
    
   static int   Demo(MyRand & R,bool debug);               // play a game from skratch
   static void  Bench();
													   
};