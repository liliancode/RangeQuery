#include "GameBasic.h"
#include "_Board.h"
 
string NaiveMove::ToString()
{
	char buff[100];
	snprintf(buff,sizeof(buff),  "%i_%2i_%2i_%i", dir,x,y,PosHole);
	 
	string R = buff;
	 
	return R;
}

bool operator == (const NaiveMove& a, const NaiveMove & b)
{
	if (a.dir != b.dir) return false;
	if (a.x != b.x) return false;
	if (a.y != b.y) return false;
	if (a.PosHole != b.PosHole) return false;
	return true;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//
//     Naive approach 0 : all moves are tested to build the list of possible moves
//
// 
 
 
void  GameBruteForce::GetPossibleMoves(_Board & B,vector<NaiveMove> & List)
{
	List.clear();

	// horiz
	for (int dir = 0 ; dir < 4 ; dir ++)
	for (int x = 0 ; x < WIDTH_TOTAL ; x ++)
		for (int y = 0 ; y < WIDTH_TOTAL ; y++)
		{
			if (_Board::Get5OK(dir, x, y) )
			{
				int Block5 = B.Get5_5D5T(dir, x, y);
				
				int Filter5T[] = { 0b1111111111, 0b111111111 };   // 10 bits for 5D,  9bits for 5T
				Block5 &= Filter5T[V5D_5T];  
				 
				// for 5D: == 00b101010100  => same tests
				if ( Block5 == 0b101010100 ) List.push_back(NaiveMove(dir, x, y, 0));
				if ( Block5 == 0b101010001 ) List.push_back(NaiveMove(dir, x, y, 1));
				if ( Block5 == 0b101000101 ) List.push_back(NaiveMove(dir, x, y, 2));
				if ( Block5 == 0b100010101 ) List.push_back(NaiveMove(dir, x, y, 3));
				if ( Block5 == 0b001010101 ) List.push_back(NaiveMove(dir, x, y, 4));
			}
		}
}

 
void AffBoard(_Board & B);
void AffNaiveMoves(vector<NaiveMove> & PossibleMoves, NaiveMove idmoveJustPlayed);


int  GameBruteForce::Demo(MyRand & R, bool debug)
{
	_Board B;
	B.Reset();
	 
	vector<NaiveMove> LCoupsPoss;
	GameBruteForce::GetPossibleMoves(B, LCoupsPoss);

	 
	if (debug) { AffBoard(B); AffNaiveMoves(LCoupsPoss, NaiveMove()); }
	
	int score = 0;
	while (LCoupsPoss.size() > 0)
	{
		if (debug) WaitForKey();
		int v = R.GetRandomU32() % LCoupsPoss.size();
		NaiveMove M = LCoupsPoss[v];
		B.Play(M.dir,M.x,M.y);
		GameBruteForce::GetPossibleMoves(B,LCoupsPoss);
		if (debug) { AffBoard(B); AffNaiveMoves(LCoupsPoss, M); }
		score++;
	}
	return score;
}
 
 

void GameBruteForce::Bench()
{
	cout << "******************************************************" << endl;
	cout << "Start BENCH Basic Algorithm "  << endl;

	MyRand R;
	Horloge H;
	H.Start();

	int NbRounds = 0;
	int nbScenar = 0;
	int BestScore = 0;
	while (H.TimeElapsedSeconds() < 10) // need at least 10 sec of tests
		for (int i = 0; i < 100; i++)
		{
			int score = GameBruteForce::Demo(R,false);
			NbRounds += score;
			nbScenar++;
			BestScore = BestScore < score ? score : BestScore;
		}

	double dt_sec = H.TimeElapsedSeconds();
	double proc_speed = CPUSpeed * 1000 * 1000 * 1000;
	double Totcycles = dt_sec * proc_speed;
	int CyclesPerRound = (int)(Totcycles / NbRounds);


	cout << " =>  Cycles per round : " << CyclesPerRound << endl;
	cout << " =>  Rounds per game  : " << NbRounds / nbScenar << endl;
	cout << " =>  Best Score found : " << BestScore << endl << endl;

}
 