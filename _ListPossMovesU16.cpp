#include "_ListPossMovesU16.h"
#include <emmintrin.h>
#include <algorithm>
 
///////////////////////////////////////////////////////////////
//
//   Select Random Move
//
//

IDMov _ListPossMovesU16::SelectRandomMove(MyRand & R)
{
	size_t v = R.GetRandomU32() % nb;
	IDMov idmove = List[v];
	return idmove;
}


IDMov _ListPossMovesU16::SelectSoftmaxMove(Ptype * Weight, MyRand & R)
{
	Ptype P[MAX_MOVES_IN_LIST+1];  
	Ptype TotPr = 0;

	Fori(nb)
	{
		P[i] = exp(Weight[List[i]]);
		TotPr += P[i];
	}
	P[nb] = 2;

	// select one element randomly

	Ptype v = (Ptype)(R.GetRand_01() * TotPr);

	size_t pos = 0;
	Ptype p = P[pos];
	while (v > p)
	{
		v -= p;
		pos++;
		p = P[pos];
	}
	if (pos >= nb)
		pos = nb - 1;

	u16 idmove = List[pos];
	return idmove;
}



IDMov _ListPossMovesU16::SelectWeightedMove(Ptype * Weight, MyRand & R, Ptype & Sigma)
{
	Ptype P[MAX_MOVES_IN_LIST];
	Ptype TotPr = 0;

	Fori(nb)
	{
		P[i] =  (Weight[List[i]]);
		TotPr += P[i];
	}
	Sigma = TotPr;

	// select one element randomly

	Ptype v = (Ptype)(R.GetRand_01() * TotPr);

	size_t pos = 0;
	Ptype p = P[pos];
	while (v > p)
	{
		v -= p;
		pos++;
		p = P[pos];
	}
	if (pos >= nb)
		pos = nb - 1;

	u16 idmove = List[pos];
	return idmove;
}
 

////////////////////////////////////////////////////
//
//    List Management  with SSE2
//



//  remove one value in list


void _ListPossMovesU16::SupprIDmove(u16 idmov)
{

	__m128i IDM = _mm_set_epi16(idmov, idmov, idmov, idmov, idmov, idmov, idmov, idmov);
	

	__m128i* BaseAdr = (__m128i*)(List);
	__m128i  values = *BaseAdr;

	__m128i mask = _mm_cmpeq_epi16(values, IDM);         // look for the given value
	__m128i newvalues = _mm_andnot_si128(mask, values);  // replace the value  by 0000
	*BaseAdr = newvalues;

	//      next values	
	size_t nbloop = ((nb - 1) / 8);
	Fori(nbloop)
	{
		BaseAdr += 1;
		values = *BaseAdr;
		mask      = _mm_cmpeq_epi16(values, IDM);
		newvalues = _mm_andnot_si128(mask, values);
		*BaseAdr = newvalues;
	}
}


 
// remove values in intervals [ min[i], max[i] ]
 
void _ListPossMovesU16::SupprIDmovesByRange(u16 min, u16 max)
{ 

	min--;
	max++;
	 
	// loading  
	__m128i MINS = _mm_set1_epi16(min);
	__m128i MAXS = _mm_set1_epi16(max);
	 
	__m128i values, tmin, tmax, mask, newvalues;
	__m128i* BaseAdr = (__m128i*)(List);
	 
	//      next values	
	size_t nbloop = 1+((nb-1) / 8);
	for (size_t i = nbloop ; i > 0 ; i--)
	{ 
		
		values = *BaseAdr;
		tmin = _mm_cmpgt_epi16(values, MINS);
		tmax = _mm_cmpgt_epi16(MAXS, values);
		mask = _mm_and_si128(tmin, tmax);
		newvalues = _mm_andnot_si128(mask, values);
		*BaseAdr = newvalues;
		BaseAdr += 1;
	}
	 
} 
 
 

// any value = 0000 in the list is removed

void _ListPossMovesU16::Pack()     
{
	
 
	size_t s = 0;
	size_t p = 0;
	 

	 
	for (size_t i = nb; i > 0; i--)
	{
		IDMov id = List[p++];
		List[s] = id;
		s += id == 0 ? 0 : 1;  
	}
	nb = s;
}

void _ListPossMovesU16::RemoveAtPos(size_t p)
{
	List[p] = List[nb-1];
	nb--;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//


 