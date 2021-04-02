#pragma once
#include "Type.h"
#include "Rand.h"

/*
Optimized data structure used to provide quick suppression by range query
using unsigned 16 bits IDMov with SSE.

Suppr...() functions replace found values with a 0
Pack() function gathers non zero values and update nb
*/

#define MAX_MOVES_IN_LIST 124

class  _ListPossMovesU16
{
	u16	List[MAX_MOVES_IN_LIST];  // sizeof is 2*124+8 = 16x16 bytes  
	size_t nb = 0;
	Ptype TotalProb(Ptype * Weight);
	 
public:

	void  AddIf(IDMov ID, int ok)	{ List[nb] = ID, nb += ok; }
	size_t   size()					{ return nb; }
	IDMov get(size_t i)				{ return List[i]; }
	void  Clear()					{ nb = 0; }
	void  push_back(IDMov idmove)   { List[nb++] = idmove; }					 
	 
	void SupprIDmovesByRange(u16 min, u16 max);			// replace all values in [min,max[ by 0
	void SupprIDmove(u16 idmov);                        // remove one idmove
	void Pack();										// remove all 0 values in the list
	void RemoveAtPos(size_t p);
	 
	  
	// selection
	IDMov SelectRandomMove(MyRand & R);
	IDMov SelectWeightedMove(Ptype * Weight, MyRand & R, Ptype & Sigma);
	IDMov SelectSoftmaxMove(Ptype * Weight, MyRand & R);
 
};

