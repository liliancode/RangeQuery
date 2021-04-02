#pragma once

 
#include "Type.h"
 
class  _Sequence
{  // 5T
private:
	IDMov	   List[236];  // sizeof is 2*236+8 = 30x16 
	size_t     nb = 0;
	  
public:
 
	const size_t   size()         { return nb; }
	IDMov get(size_t i)           { return List[i]; }
	void  Clear()                 { nb = 0; }
	void  push_back(IDMov idmove) { List[nb++] = idmove; }

	string ToString()  
	{
		std::stringstream buf;
		Fori(nb) 
			buf << k(List[i], 6);
		return buf.str();
	}
	
};
