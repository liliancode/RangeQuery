
#include "Type.h"
#include "Data.h"
#include "GameRQ.h"

char WaitForKey() 
{ 
	char c = getc(stdin);
	char d = c;
	while (c != '\n') { c = getchar(); }
	return d; 
}

// add some " " to obtain a string of length len

string k(string in, int len )
{
	string wp = "";
	int l = (int)in.size();
	for (int i = 0; i < len - l; i++)
		wp += ' ';
	return wp + in;
}

string k(int in, int len )
{
	return k(to_string(in), len );
}

string k0(string in, int len)
{
	string wp = "";
	int l = (int)in.size();
	for (int i = 0; i < len - l; i++)
		wp += '0';
	return wp + in;
}

string k0(int in, int len)
{
	return k0(to_string(in), len);
}

// 123456 => 123 456

string Millions(u64 val)
{
	vector<int> L;
	while (val > 0)
	{
		L.push_back(val % 1000);
		val /= 1000;
	}

	string T = "";
	if (L.size() == 0)
		L.push_back(0);

	Fori(L.size()-1)
	{
		string h = to_string(L[i]);
		while (h.size() < 3) 
			h = "0" + h;
		T =  h + " " + T;
	}
	T = to_string(L[L.size() - 1]) + " " + T;
	return T;


}
 


//////////////////////////////////////////////////////////////////
//
//  Get CPU speed

double CPUSpeed = 4.2;
void  DetectCPUSpeed()
{
	 
	u32 kk = 1;
	Horloge T;
	cout << "Evaluating CPU speed ";
	T.Start();
	const int nb = 100000000;
	Fori(nb)
	{
		kk += 17;
		kk >>= 1;
		kk += 17;
		kk >>= 1;
		kk += 17;
		kk >>= 1;
		kk += 17;
		kk >>= 1;
		kk += 17;
		kk >>= 1;
	}
	double dt = T.TimeElapsedSeconds();
	double speed = 1.0 / dt;
	cout << kk << endl;
	if ( (speed < 8) && (speed > 1) )
	{ 
	   cout << "CPU Speed: " << speed << "GHz"<<endl;
	   CPUSpeed = speed;
	}
	else
	{
		cout << "FAIL to detect CPU speed" << endl;
	}
 
}


