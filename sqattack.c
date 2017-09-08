#include "stdio.h"
#include "definitions.h"

const int KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10,	1, 10 };
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };

int Attackedsquare(const int sq, const int side, const BOARD *pos) {

	int pce,i,tempsq,dir;
	
	ASSERT(SquareOnB(sq));
	ASSERT(ValidSide(side));
	ASSERT(Boardcheck(pos));
	
	// pawns
	if(side == WHITE) {
		if(pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP) {
			return TRUE;
		}
	} else {
		if(pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP) {
			return TRUE;
		}	
	}
	
	// knights
	for(i = 0; i < 8; ++i) {		
		pce = pos->pieces[sq + KnDir[i]];
		ASSERT(PceValidEmptyOffbrd(pce));
		if(pce != OFFBOARD && IsKNT(pce) && ColourOfPiece[pce]==side) {
			return TRUE;
		}
	}
	
	// rooks, queens
	for(i = 0; i < 4; ++i) {		
		dir = RkDir[i];
		tempsq = sq + dir;
		ASSERT(SqIs120(tempsq));
		pce = pos->pieces[tempsq];
		ASSERT(PceValidEmptyOffbrd(pce));
		while(pce != OFFBOARD) {
			if(pce != EMPTY) {
				if(IsRKQN(pce) && ColourOfPiece[pce] == side) {
					return TRUE;
				}
				break;
			}
			tempsq += dir;
			ASSERT(SqIs120(tempsq));
			pce = pos->pieces[tempsq];
		}
	}
	
	// bishops, queens
	for(i = 0; i < 4; ++i) {		
		dir = BiDir[i];
		tempsq = sq + dir;
		ASSERT(SqIs120(tempsq));
		pce = pos->pieces[tempsq];
		ASSERT(PceValidEmptyOffbrd(pce));
		while(pce != OFFBOARD) {
			if(pce != EMPTY) {
				if(IsBISQN(pce) && ColourOfPiece[pce] == side) {
					return TRUE;
				}
				break;
			}
			tempsq += dir;
			ASSERT(SqIs120(tempsq));
			pce = pos->pieces[tempsq];
		}
	}
	
	// kings
	for(i = 0; i < 8; ++i) {		
		pce = pos->pieces[sq + KiDir[i]];
		ASSERT(PceValidEmptyOffbrd(pce));
		if(pce != OFFBOARD && IsKING(pce) && ColourOfPiece[pce]==side) {
			return TRUE;
		}
	}
	
	return FALSE;
	
}