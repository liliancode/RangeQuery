
 
#include "_Board.h"
#include <cstring>
#include <emmintrin.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//	 GAMEBOARD MANAGEMENT
//
//
//   gameboard : 1 byte for each square 
//   bit 0/2/4/6   : dot 
//   bit 1         : diag top left
//   bit 3         : vert upwards
//   bit 5         : diag top right
//   bit 7         : horizontal to the right
//   4 directions sorted in clockwise order

const u8  DotMask = 1 | (1 << 2) | (1 << 4) | (1 << 6);
const u8  DotSegMask_Dir[4] = { 1 + 2, 4 + 8, 16 + 32, 64 + 128 };
 
const int dirX[4] = { -1, 0 , 1, 1 };
const int dirY[4] = { 1, 1 , 1, 0 };

_Board _Board::SlowInitBoard()
{
	 
	_Board B;

	Fori(WIDTH_TOTAL*WIDTH_TOTAL)
		B.board[i] = 0;

	int p[][2] =
	{ {2,3}, {1,3}, {0,3}, {0,4}, {0,5}, {0,6},
	  {1,6}, {2,6}, {3,6}, {3,7}, {3,8}, {3,9},
	  {4,9}, {5,9}, {6,9}, {6,8}, {6,7}, {6,6},
	  {7,6}, {8,6}, {9,6}, {9,5}, {9,4}, {9,3},
	  {8,3}, {7,3}, {6,3}, {6,2}, {6,1}, {6,0},
	  {5,0}, {4,0}, {3,0}, {3,1}, {3,2}, {3,3} };

	for (int * L : p)
	{
		int dec = -5 + WIDTH_TOTAL / 2;
		int x = L[0] + dec;
		int y = L[1] + dec;
		size_t pos = GetPos(x, y);
		B.board[pos] = DotMask;
	}
	return B;
}


_Board   _Board::StartingBoard;


void _Board::BoardStaticInit()
{ 
	_Board::StartingBoard = _Board::SlowInitBoard(); 
}
 

// upleft / up / upright / horiz
// when traversing the grid in a given direction,
// in the 1D array we use this step to increase the index
const size_t Step_Dir[4]  = {  WIDTH_TOTAL-1, WIDTH_TOTAL, WIDTH_TOTAL + 1, 1 };



int _Board::Get5_5D5T(int dir, int x, int y)
{
	int Block5 = 0;

	size_t step = Step_Dir[dir];
	int mask = (int) DotSegMask_Dir[dir];
	size_t p = GetPos(x, y);
	
    Block5 |= (board[p + step * 0] & mask) << (2 * 0);
	Block5 |= (board[p + step * 1] & mask) << (2 * 1);
	Block5 |= (board[p + step * 2] & mask) << (2 * 2);
	Block5 |= (board[p + step * 3] & mask) << (2 * 3);
	Block5 |= (board[p + step * 4] & mask) << (2 * 4);

	Block5 >>= 2 * dir;
	return Block5 & Filter5D5T;
}




int _Board::Get2bitsInfo(int dir, int x, int y)    // slow
{ 
	size_t p = GetPos(x, y);
	int value = board[p] >> (2*dir);
	 
	return value & 3;
}

 


void _Board::Get9x4Dirs(int B9[4], int x, int y)
{
	size_t p = GetPos(x, y);
	
	B9[0] = B9[1] = B9[2] = B9[3] = 0;
	size_t dec = 0;
	size_t p0 = p - 4 * Step_Dir[0];
	size_t p1 = p - 4 * Step_Dir[1];
	size_t p2 = p - 4 * Step_Dir[2];
	size_t p3 = p - 4 * Step_Dir[3];

	Fori(9)
	{
		B9[0] |= (board[p0 + i * Step_Dir[0]] & DotSegMask_Dir[0]) << dec;
		B9[1] |= (board[p1 + i * Step_Dir[1]] & DotSegMask_Dir[1]) << dec;
		B9[2] |= (board[p2 + i * Step_Dir[2]] & DotSegMask_Dir[2]) << dec;
		B9[3] |= (board[p3 + i * Step_Dir[3]] & DotSegMask_Dir[3]) << dec;
		dec += 2;
	}
	B9[1] >>= 2;
	B9[2] >>= 4;
	B9[3] >>= 6;
	 

}
 


const u8 allDots = 1 + 4 + 16 + 64;
const u8 PlayMask_Dir[4] = { allDots + 2, allDots + 8, allDots + 32, allDots + 128 };

// play a given move
void _Board::Play(int dir, int x, int y)
{  

	size_t p    = GetPos(x, y);
	size_t step = Step_Dir[dir];
	
	u8 PlayMask = PlayMask_Dir[dir];
	// segment
	Fori(4)
		board[p + i * step] |= PlayMask;
	// 5T
	if (V5D_5T)
	   board[p + 4 * step] |= allDots;
	else // 5D 
		board[p + 4 * step] |= PlayMask;

} 
 
 
 

////////////////////////////////////////////////////////////
//
// static / debug / precomptation

size_t _Board::GetPos(int x, int y) { return x + y * WIDTH_TOTAL; }


void _Board::GetHoleXY(int dir, int x, int y, int holePos, int & HoleX, int & HoleY)
{
	HoleX = x + holePos * dirX[dir];
	HoleY = y + holePos * dirY[dir];
}
 
 
 
bool _Board::Get5OK(int dir, int x, int y)
{
	int min = WIDTH_BORDER;
	int max = WIDTH_TOTAL - WIDTH_BORDER;

	if (x < min) return false;
	if (x >= max) return false;
	if (y < min)  return false;
	if (y >= max)  return false;

	int xx = x + dirX[dir] * 4;
	int yy = y + dirY[dir] * 4;

	if (xx < min)  return false;
	if (xx >= max) return false;
	if (yy < min)  return false;
	if (yy >= max) return false;

	return true;
}

///////////////////////////////////////////


////////////////////////////////////////////////////////////////
//
//  IdMove Management
//
//


const int L = 64;

/*

Four directions are considered : upper-left, up, upper-right, right
These four directions are numbered separately, using the first 2 bits of the IdMove

For each direction, when we traverse the current line, the u coordinate is increasing.
When the line changes, the v coordinate also changes.

When u/v is known the associated index in the 1D array is : u + L * v

Here are the coordinates transformation :

d = 0 :
   u = y
   v = x + y
   => P = Lx + (L+1) y + 0

d = 1 :
	u = y
	v = x
	P = y + Lx

d = 2 :
	u = y
	v = x - y + (L-1)   (in order to satisfy v >= 0)
	P = Lx + (1-L)y + L(L-1)

d = 3 :
	u = x
	v = y
	P = x + Ly
*/


void _Board::Create4IDMove(IDMov IDM[4], int x, int y)
{

	// const static int a[4][3] = { {L,   (L + 1), 0},   { L, 1 , 0},   {L,(1 - L),L*(L - 1)},   {1,L,0} };

	int Lx = L * x;
	 
	int P0 = Lx + (L+1) * y;
	int P1 = Lx +         y;
	int P2 = Lx + (1-L) * y + L*(L-1);
	int P3 =  x +     L * y;

	IDM[0] = (0 << 13) | P0;
	IDM[1] = (1 << 13) | P1;
	IDM[2] = (2 << 13) | P2;
	IDM[3] = (3 << 13) | P3;

}


IDMov _Board::CreateIDMove(int d, int x, int y)
{

	const static int a[4][3] = { {L, (L + 1), 0}, { L, 1 , 0}, {L,(1 - L),L*(L - 1)}, {1,L,0} };

	const int *b = a[d];

	int P = b[0] * x + b[1] * y + b[2];



	IDMov idmov = (d << 13) | P;

	return idmov;
}

void _Board::DecodeMove(IDMov IDm, int & d, int & x, int & y)
{
	//__debugbreak();

	d = IDm >> 13;
	int p = IDm & 0b1111111111111;
	int u = p & (L - 1);
	int v = p >> 6;

	y = (d != 3) ? u : v;

	const static int a[4][3] = { {-1,1,0}, { 0,1,0 }, { 1,1,-L + 1} , {1,0,0 } };

	const int *b = a[d];

	x = b[0] * u + b[1] * v + b[2];

}