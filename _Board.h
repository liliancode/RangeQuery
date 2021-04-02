#pragma once
#include "Type.h"
 

const int WIDTH_BORDER  = 4;
const int WIDTH_TOTAL = 32 * (1 - V5D_5T) + 64 * V5D_5T;

class _Board
{
	
	u8   board[WIDTH_TOTAL*WIDTH_TOTAL];

	 
	static _Board     SlowInitBoard();
	static _Board     StartingBoard;

public :

	static size_t        GetPos(int x, int y);
	void Reset()         { *this = StartingBoard; }

	// Get Info :
	void Get9x4Dirs(int B9[4], int x,int y);      // get data for 9 squares x 4 directions - centered on (x,y)
	int  Get2bitsInfo(int dir, int x, int y);  	  // bit0 : dot at given pos   -   bit1 : line for this direction
	int  Get5_5D5T(int dir, int x, int y);	      // return dot/seg/dot/seg/dot/seg... for 5 squares starting at (x,y)  5D: 10 bits  /  5T: 9 bits

	// query
	static bool Get5OK(int dir, int x, int y);                                            // check whether the next 5 squares are inside the gameboard
	static void GetHoleXY(int dir, int x, int y, int DistHole, int & HoleX, int & HoleY); // Get Hole position

	// play at 
	void Play(int dir, int x, int y);


	 
	// Init at program startup
	static void BoardStaticInit(); 

	///////////////////////////////////////////////////////////////////////
	//
	// IdMove 

	// 	IdMove (15 bits) :    d=> 2 bits /  v=> 7 bits    /   u => 6 bits 
 
	
	// o   CreateIDMove( d, x, y)   => IdMove
	// o   DecodeMove(Idmove)       => d, x, y


	static IDMov CreateIDMove(int d, int x, int y);
	static void  DecodeMove(IDMov IDm, int & d, int & x, int & y);
	static void  Create4IDMove(IDMov IDM[4], int x, int y);

};
  