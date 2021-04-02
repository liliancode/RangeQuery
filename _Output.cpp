#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
 
#include <sstream>
#include "_Board.h"
#include "Rand.h"
#include "GameBasic.h"
#include "GameRQ.h"
#include "Data.h"
#include "Type.h"

const int GREEN = 2;
const int RED   = 1;
const int WHITE = 0;

///////////////////////////////////////////////////////////////////////////
//
//   virtual console management

class Cons
{
	char ConsoleXY[300][200];
	int  CouleurXY[300][200];

	char GrilleJeu[4][WIDTH_TOTAL][WIDTH_TOTAL];
	char GrilleJeuPrec[4][WIDTH_TOTAL][WIDTH_TOTAL];
	vector<string> oldMSG;
	vector<string> newMSG;

	int CursorX, CursorY; // cursor coordinate

	void outstring(string text, int coul) // print string on the right
	{
		CursorX = (WIDTH_TOTAL+2) * 4 + 3;
		for (char c : text)
		{
			ConsoleXY[CursorX][CursorY] = c;
			CouleurXY[CursorX][CursorY] = coul;
			CursorX++;
		}
		CursorY++;
	}
	void outchar(char c, int x, int y, int coul)
	{
		ConsoleXY[x][y] = c;
		CouleurXY[x][y] = coul;
	}

	void basculeMSG() // bascule les messages a l'ecran dans ConsoleXY
	{
		CursorY = 0;

		for (string text : newMSG)
		{
			int coul = 0;
			// si ce msg n'est pas dans l'ancienne liste, c'est un nouveau => aff vert
			if (find(oldMSG.begin(), oldMSG.end(), text) == oldMSG.end())
				coul = 2;
			outstring(text, coul);
		}

		for (string text : oldMSG)
		{
			 
			// si ce msg n'est pas dans la nouvelle liste, c'est un disparu => aff rouge
			if (find(newMSG.begin(), newMSG.end(), text) == newMSG.end())
			   outstring(text, 1);
		}
	}

	void basculeGrilles() // bascule les 4 grilles dans ConsoleXY 
	{
		GrilleJeu[0][0][0] = 'X';
		GrilleJeu[0][0][1] = '^';
		GrilleJeu[0][1][0] = '>';


		int LCol = WIDTH_TOTAL + 2;
		for (int d = 0; d < 4; d++)
		{
			for (int x = 0; x < WIDTH_TOTAL ; x++)
				for (int y = 0; y < WIDTH_TOTAL ; y++)
				{
					int coul = 0;
					if ( GrilleJeu[d][x][y] != GrilleJeuPrec[d][x][y] )
						coul = 2;
					int xx = x + d * LCol;
					int yy = WIDTH_TOTAL - y - 1;
					outchar(GrilleJeu[d][x][y], xx, yy, coul);
				}

			for (int y = 0; y < WIDTH_TOTAL; y++)
			{ 
				int yy = WIDTH_TOTAL - y - 1;
				outchar('0' + (yy / 10), LCol-2 + d * LCol, y, 0);
				outchar('0' + (yy % 10), LCol-1 + d * LCol,  y, 0);
			}
		}

		for (int d = 0; d < 4; d++)
		for (int x = 0 ; x < WIDTH_TOTAL  ; x++)
		{
			int xx = x + d * LCol;
			outchar('0' + (x / 10), xx, WIDTH_TOTAL, 0);
			outchar('0' + (x % 10), xx, WIDTH_TOTAL+1, 0);
		}

		
	}

public:
	 
	void Show()   // to screen
	{
		basculeGrilles();
		basculeMSG();
	
		for (int y = 0; y < WIDTH_TOTAL + 2; y++)  //5T bug
		{
			for (int x = 0; x < (WIDTH_TOTAL+2)*4 + 36; x++)
			{

				if (CouleurXY[x][y] == GREEN)  cout << "\033[32m"; 
				if (CouleurXY[x][y] == RED)    cout << "\033[31m"; 
				if (CouleurXY[x][y] == WHITE)  cout << "\033[37m"; 
				cout << ConsoleXY[x][y];
			}
			cout << endl;
		}
	}

	void Reset()   // clear virtual console
	{
		CursorX = CursorY = 0;
		for (int y = 0; y < 200; y++)
			for (int x = 0; x < 300 ; x++)
			{ 
				ConsoleXY[x][y] = ' ';
				CouleurXY[x][y] = 0;
			}
		
		// bascule GrilleJeu dans GPrec
		memcpy(GrilleJeuPrec, GrilleJeu, 4 * WIDTH_TOTAL * WIDTH_TOTAL );

		// copie la liste des coups dans oldcoups
		oldMSG.clear();
		for (string t : newMSG)
			oldMSG.push_back(t);
		newMSG.clear();
	}


	void MSG(string text)
	{
		newMSG.push_back(text);
	}



	void out(int dir, int x, int y, char c)
	{
		//cout << dir << "  " << x <<"-" <<  y << "   " << c << endl;
		  GrilleJeu[dir][x][y] = c;
	}

 
};

Cons Consol;

 
/////////////////////////////////////////////////////////////
//

 
void InfoMove(string MoveCode, int dir, int x, int y, int HolePos, char c = 'X')
{
	int HoleX, HoleY;
	_Board::GetHoleXY(dir, x, y, HolePos, HoleX, HoleY);
	 
	string txt = MoveCode + " D" + k(dir, 1) + " " + k(x, 2) + "-" + k(y, 2);
	string txx = " (" + k(HoleX, 2) + "," + k(HoleY, 2) + ")";

	Consol.out(dir, HoleX, HoleY, c);

	Consol.MSG(txt + txx);
}
 


///////////////////////////////////////////////////////////
//
//  Demo Naive Game


void AffBoard(_Board & B)
{
	Consol.Reset();

	// affichage des pions / traits
	char Code[4][4] = { { '.', 'o' , '\\', '\\'},{ '.', 'o' , '|', '|'} ,{ '.', 'o' , '/', '/'},  { '.', 'o' , '_', '_'} };

	for (int dir = 0; dir < 4; dir++)
		for (int x = 0; x < WIDTH_TOTAL; x++)
			for (int y = 0; y < WIDTH_TOTAL; y++)
			{

				int code = B.Get2bitsInfo(dir, x, y);
				Consol.out(dir, x, y, Code[dir][code]);
			}
}




void AffNaiveMoves(vector<NaiveMove> & PossibleMoves, NaiveMove idmoveJustPlayed)
{
	for (NaiveMove M : PossibleMoves)
		InfoMove( M.ToString() ,M.dir,M.x,M.y,M.PosHole);

	Consol.MSG("--------");
	 
	NaiveMove J = idmoveJustPlayed;
	if (J.dir!= 0 || J.x != 0 || J.y != 0 || J.PosHole != 0)
  	  InfoMove( J.ToString() , J.dir, J.x, J.y, J.PosHole,'@');
	
	Consol.MSG("--------");

	Consol.Show();
}

string IdMoveToS(u16 idMove)
{
	char buff[100];
	snprintf(buff, sizeof(buff),"%6i", (int)  idMove);
	return buff;
}

void AffAdvancedMoves(_ListPossMovesU16 & Moves, int idmoveJustPlayed)
{
	int dir, x, y, holePos=0;

	 
	Fori(Moves.size())
	{
		//InfoMove::DecodeMove(Moves.get(i), dir, x, y, holePos);
		_Board::DecodeMove(Moves.get(i), dir, x, y);
		InfoMove(IdMoveToS(Moves.get(i)), dir, x, y, holePos);
	}

	Consol.MSG("--------");
	 
	if (idmoveJustPlayed != 0)
	{
		_Board::DecodeMove(idmoveJustPlayed, dir, x, y);
 	    InfoMove(IdMoveToS(idmoveJustPlayed), dir, x, y, holePos);
    }
	Consol.MSG("--------");

	Consol.Show();

}



/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//   Print information to monitor process

string k(int in, int len);

string Millions(u64 val);

_Sequence BestFound;

extern Horloge StartClock;

void printMovesPS(  _Sequence & S);



#include <ctime>
#include <iomanip>
string current_time1()
{
	system_clock::time_point tp = system_clock::now();

	time_t raw_time = system_clock::to_time_t(tp);

	// Tm* does not need to be deleted after use, because tm* is created by localtime, and there will be one in each thread
	struct tm  *timeinfo = std::localtime(&raw_time);

	std::stringstream ss;
	ss << std::put_time(timeinfo, "%Y_%m_%d___%Hh%M_%S");
	 
	return ss.str();
}

extern Horloge StartClock;
string D2(int v)
{
	string k = to_string(v);
	if (k.length() < 2) k = string("0") + k;
	return k;
}
string TimeSinceBeginning()
{
	u64 deltaT = (u64) StartClock.TimeElapsedSeconds();
	
	u64 S = deltaT % 60;
	deltaT /= 60;
	u64 M = deltaT % 60;
	deltaT /= 60;
	u64 H = deltaT % 24;
	deltaT /= 24;
	u64 D = deltaT;

	stringstream ss;
	ss << D << "D" << D2(H) << "H" << D2(M) << ":" << D2(S);
    return ss.str();
}

#include <fstream>
void AffDistrib(vector<Data> & DL, Data & DCumul, int level, int nbT)
{
	// BATCH INFO
	
	stringstream SB;
	/*
	Data ThisBatchAllThreads;
	Fori(DL.size()) ThisBatchAllThreads.AddData(DL[i]);

	SB << endl << ">>> THIS BATH INFOS" << endl;

	SB << "Simulations all Threads : " << Millions(ThisBatchAllThreads.TotalRuns()) << endl;
	SB << "Mean Simul  : " << ThisBatchAllThreads.TotalRuns() / (float)(nbT) << endl;
	SB << "Mean score  : " << ThisBatchAllThreads.MeanScore() << endl;
	SB << "Mean time   : " << ThisBatchAllThreads.GetTimeUsedSeconds() / nbT << endl;

	 

	//ThisBatchAllThreads.PrintDistrib(SB);

	SB << endl << endl;
	*/
	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////

	// ALL BATCH INFO

	int secStart = (int)StartClock.TimeElapsedSeconds();


	SB << endl;
	SB << ">>> SUMMARY - ALL BATCHS <<<         "  ;
	SB << " Nested Level " << level << " | Nb Threads " << nbT;
	SB << " | Time since beginning  " << secStart << " s |  Mean time per run " << DCumul.GetTimeUsedSeconds() / DCumul.TotalRuns();
	SB << " | Total Sim " << Millions(DCumul.TotalRuns()) << " | Mean score " << DCumul.MeanScore();
	SB << " | Best known score  " << DCumul.GetBestScore() << endl;

	SB << "Best seq  : " << DCumul.GetBestSeq().ToString() << endl;

	DCumul.PrintDistrib(SB);

	string info = SB.str();

	cout << info;

	try
	{ 
   	   fstream fs;
	   fs.open(current_time1() + ".txt",  std::fstream::out  );
	   fs << info;
	   fs.close();
	}
	catch (...) {}
 
	_Sequence S = DCumul.GetBestSeq();
	 
  	 printMovesPS(  S);
	 
	// liste des branchements

	 
	GameRQ G;
	G.Reset();
	cout << "Childs : ";
	Fori(S.size())  
	{
		_ListPossMovesU16 & LChilds = G.LPossMoves;
		 
		IDMov IDplayed = S.get(i);
		size_t nbChilds = LChilds.size();
		 
		cout << nbChilds << " ";
		G.PlayOneMove(IDplayed);
	}
	cout << endl;
	 
}



/////////////////////////////////////////////////////////////////////////////
//
//
//  export sequence to postcript file

typedef struct
{
	int x;
	int y;
	int lastx;
	int lasty;
	int holex;
	int holey;
	int HoleShift;
	void dec(int dx, int dy)
	{
		x += dx;
		y += dy;
		lastx += dx;
		lasty += dy;
		holex += dx;
		holey += dy;
	}
} InfMove;

void printMovesPS(  _Sequence & S)
{

	try
	{
		size_t length = S.size();

		stringstream filename1;
		filename1 << length << "_img.eps";

		stringstream filename2;
		filename1 << length << "_pos.txt";

		ofstream fp1(filename1.str());
		ofstream fp2(filename2.str());

		Fori(S.size())
			fp2 << S.get(i) << " ";
		fp2 << endl;

		fp1 << "%%!PS-Adobe-2.0 EPSF-2.0\n%%%%BoundingBox: 0 0 420 630\n%%%%Pages: "<< length+2 << "\n";
		fp1 << "/n0 36 def\n\n/board0\n5  0   5  1   5  2   4  2   3  2   2  2   2  3   2  4   2  5\n1  5   0  5  -1  5  -1  4  -1  3  -1  2  -2  2  -3  2  -4  2\n-4 1  -4  0  -4 -1  -3 -1  -2 -1  -1 -1  -1 -2  -1 -3  -1 -4\n0 -4   1 -4   2 -4   2 -3   2 -2   2 -1   3 -1   4 -1   5 -1\nn0 2 mul array astore def\n\n/step 30 def\n/stone_radius 10 def\n/X {step mul} def\n/Y {step mul neg} def\n\n/Times findfont stone_radius scalefont setfont\n\n/circle {\n    1 setlinewidth\n    newpath exch X exch Y stone_radius 0 360 arc stroke\n} def\n\n/fillcircle {\n    1 setlinewidth\n    newpath exch X exch Y stone_radius 0 360 arc fill stroke\n} def\n\n/number {\n    exch X exch Y moveto\n    4 string cvs show\n} def\n\n/line {\n    0.5 setlinewidth\n    newpath\n    exch X exch Y moveto\n    exch X exch Y lineto\n    stroke\n} def\n\n/thickline {\n    2 setlinewidth\n    newpath\n    exch X exch Y moveto\n    exch X exch Y lineto\n    stroke\n} def\n\n/start_game {\n    0 1 n0 1 sub {\n 2 mul /i exch def\n board0 i get board0 i 1 add get circle\n    } for\n} def\n\n/up_to_numbered {\n    /n exch def\n    n 7 mul copy\n    start_game\n    n { line number } repeat\n} def\n\n/up_to {\n    /n exch def\n    n 7 mul copy\n    start_game\n    n 0 ne {\n n 1 sub { line circle pop } repeat\n thickline fillcircle pop\n    } if\n} def\n";

		GameRQ G;
		G.Reset();
		int pos = length;
		vector<int> List;
		extern	u8 MoveIsPossibleAndHoleShift_S5[1024];
		vector<InfMove> InfoMoves;
		Fori(length)
		{
			int dxdir[] = { -1,0,1,1 };
			int dydir[] = { 1,1,1,0 };
			InfMove T;
			int move = S.get(i);
			int dir;
		 
			_Board::DecodeMove(move, dir, T.x, T.y);
			T.lastx = T.x + 4 * dxdir[dir];
			T.lasty = T.y + 4 * dydir[dir];
		
			// look for hole
			int S5 = G.B.Get5_5D5T(dir, T.x, T.y);
			T.HoleShift = MoveIsPossibleAndHoleShift_S5[S5] >> 1;
			T.holex = T.x + T.HoleShift * dxdir[dir];
			T.holey = T.y + T.HoleShift * dydir[dir];

			InfoMoves.push_back(T);
			G.PlayOneMove(move);
		}

		Fori(length)
		{
			int mid = 32;
		 
			InfMove K = InfoMoves[length - i -1];
			K.dec(-mid+1, -mid+1);
	 
			fp1  << pos <<" "<< K.holex << " " << K.holey << " " << K.x << " " << K.y << " " << K.lastx << " " << K.lasty << endl;

			fp2 << "It: " << pos + 1 << " X0: " << K.x << " Y0: " << K.y << " X1: " << K.lastx << " Y1: " << K.lasty << " Hole: " << K.HoleShift << "\n";

			pos--;
		}


		fp1 << "\n";

		fp1 << "%%%%Page:  1  1\n280 420 translate\n" << length << " up_to_numbered\nshowpage\n";

		Fori(length)
			fp1 << "%%%%Page: " << i+2 << " " << i+2 <<"\n280 420 translate\n" << i+1 << " up_to\nshowpage\n";

		fp1 << "\n";


		fp1.close();
		fp2.close();
		cout << "WRITE TO FILE OK - SCORE : " << S.size() << endl;
	}
	catch (...) {}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Dynamic Output of Results



int LastScore[5000][10] = { 0 };
int LastItera[5000][10] = { 0 };

string TimeSinceBeginning();
static int mutex = 0;
double T0 = -100;
string k0(int in, int len);

void DynAff(int threadID, int it, int bestscore, int level, int TopLevel, int NbThreads,int MinLevel = 4)
{
	LastScore[threadID][level] = bestscore;
	LastItera[threadID][level] = it;

	if (threadID != 0) return;


	//if ((level < MinLevel) && (TopLevel != level)) return;
	double T1 =  StartClock.TimeElapsedSeconds();
	if (T1 - T0 < 30) return;
	
	 
 
	if (mutex == 0)
	{ 
		mutex = 1;
		T0 = T1;
		fprintf(stderr, "%s ========================================\n", TimeSinceBeginning().c_str());
		Fori(TopLevel)
		{
			int level = i + 1;
			// best
			int mxx = 0;
			Forj(NbThreads)
				mxx = (mxx < LastScore[j][level]) ? LastScore[j][level] : mxx;

			// max count
			int mxcount = 0;
			Forj(NbThreads)
				if (LastScore[j][level] == mxx) mxcount++;


			// moy
			float moy = 0;
			Forj(NbThreads) moy += LastScore[j][level];
			moy /= NbThreads;

			// med
			vector<int> val;
			Forj(NbThreads)   val.push_back(LastScore[j][level]);
			std::sort(val.begin(), val.end());
			int med = val[val.size() / 2];


			fprintf(stderr, "L%1d - MAX %3d(%2d) - MOY %5.2f - MED %3d\n", level, mxx, mxcount, moy, med);
		}
		Fori(TopLevel)
		{
			int level = i + 1;
			fprintf(stderr, "L%1i =>",level);
			Forj(NbThreads)
				fprintf(stderr, "I%s-%s ", k0(LastItera[j][level],2).c_str(), k0(LastScore[j][level],3).c_str());
			fprintf(stderr, "\n");
		}
		mutex = 0;
	}
}
void DynAffInit()
{
	T0 = -100;
	for (int k = 0; k < 10; k++)
		for (int i = 0; i < 5000; i++)
		{ 
			LastScore[i][k] = 0;
			LastItera[i][k] = 0;
		}
}