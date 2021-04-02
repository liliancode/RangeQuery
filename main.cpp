

using namespace std;
 
#include "_Board.h"
#include "NMCS.h"
#include "NRPAvanilla.h"

 
#include "NRPA.h"
#include "GameBasic.h"
#include "GameRQ.h"


u32 NbThreads = 10;
float alpha = 1;
Horloge StartClock;
  
void ChangeNbThread()
{
	try
	{
		cout << "Enter number of threads : ";
		string nbb;
		cin >> nbb;
		int nb = atoi(nbb.c_str());
		if ((nb > 0) && (nb < 300)) { NbThreads = nb; }
	}
	catch (...) {}
}

void ChangeAlpha()
{
	try
	{
		cout << "Enter new alpha : ";
		string nbb;
		cin >> nbb;
		float nb = (float) atof(nbb.c_str());
		if ((nb > 0) && (nb < 200))   alpha = nb;
	}
	catch (...) {}
}
 
void TestRAM();

extern void CheckAdvancedMethod();
void Go(char c)
{
	if (c == '0')  exit(0);
 
	if (c == '1') { MyRand R; GameBruteForce::Demo(R, true); }
	if (c == '2') { MyRand R; GameRQ::Demo(R, true); }
	if (c == '3') { CheckAdvancedMethod(); }
	 

	if (c == '4')  MyRand::Bench();
	if (c == '5')  GameBruteForce::Bench();

	if (c == '6') { GameRQ::BenchRQ(0);  }
	if (c == '7') { GameRQ::BenchRQ(1); }
	if (c == '8') { GameRQ::BenchRQ(2); }
	

	if (c == 'a')  ChangeNbThread();
	if (c == 'b')  ChangeAlpha();
	 
	  
	 
}

void AffMenu()
{
	if (V5D_5T == 0)
		cout << " \nRUNNING 5D    RUNNING 5D    RUNNING 5D    RUNNING 5D    \n ";
	else
		cout << " \nRUNNING 5T    RUNNING 5T    RUNNING 5T    RUNNING 5T    \n";
	cout << "\n////////////////////////////////////////////////////////////////\n";
	cout << "0- Quit" << endl;
	 
	cout << "1- Demo game - Brute Force method" << endl;
	cout << "2- Demo game - RangeQuery method" << endl;
	cout << "3- Check RangeQuery method" << endl;
	cout << "4- Performance test - Random Generator" << endl;
	cout << "5- Performance test - Brute Force" << endl;
	cout << "6- Performance test - RangeQuery + Random selection" << endl;
	cout << "7- Performance test - RangeQuery + weighted prob" << endl;
	cout << "8- Performance test - RangeQuery + softmax" << endl;
	 
	cout << endl;

	cout << "a- Change the number of threads - Current: " << NbThreads << endl;
	cout << "b- Change the value of alpha - Current: " << alpha << endl;
 	 
	cout << endl;
 
	cout << "c- NMCS vanilla" << endl;

	cout << endl;
 
	cout << "d- NRPA vanilla" << endl;

	cout << endl;

	cout << "e- NRPA optimized" << endl;



	cout << endl;
	cout << "c200 => run 200 times scenario c" << endl;
	cout << "" << endl;
}

////////////////////////////////////////////////////////////////////////////////

void DetectCPUSpeed();
 

void InitPossMove();
  
void AffDistrib(vector<Data> & DL, Data & DCumul, int level, int nbT);
int main(int argc, char** argv)
{
	 
	 
	DetectCPUSpeed();
	_Board::BoardStaticInit();
	GameRQ::staticInit();
	  
	 
	

	if (argc == 3)
	{
		StartClock.Start();
		int nbthreadss = (int)atoi(argv[2]);
		int level = (int)atoi(argv[1]);
		Data Cumul;
		Fori32(500)
		{
			cout << "========================================================" << endl;
			cout << "BATCH : " << i +1  << endl;
			Data ThisRun;
			 
			NRPA(nbthreadss, 1).RunBatchMultiCore(level, Cumul);
		}
	}
	 
	while (true)
	{
		AffMenu();
		cout << "Scenario?  (1,2,3...)   ";
		string c;
		cin >> c;
		try
		{
			char t = c.c_str()[0];
			int nb = 1;
			c.erase(c.begin());
			if (c.length() > 0)
				nb = stoi(c);

			if (   t == 'c' || t == 'd' || t == 'e'  )
			{ 
				 
				cout << "Level ? ";
				string d;
				cin >> d;
				int level = stoi(d);
  
				Data Cumul;
				StartClock.Start();
				cout << "******************************************************" << endl;
				cout << "RUNNING " << nb << " times scenario: " << t << endl;
				cout << "******************************************************" << endl;
			 
				Fori32(nb)
				{
					cout << "========================================================" << endl;
					cout << "BATCH : " << i+1 << "/" << nb << endl;
					Data ThisRun;
					 
					if (t == 'c') NMCS(NbThreads).RunBatchMultiCore(level, Cumul);

					if (t == 'd') NRPAvanilla(NbThreads, alpha).RunBatchMultiCore(level, Cumul);
					 
					if (t == 'e') NRPA(NbThreads, alpha).RunBatchMultiCore(level, Cumul);

				 
					 
				}
			}
			else Go(t);
		}
		catch (...) {}
	} 

	return 0;
}
