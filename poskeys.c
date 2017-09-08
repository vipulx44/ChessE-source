#include "stdio.h"
#include "definitions.h"

BITS64 GenKey(const BOARD *pos) {

	int sq = 0;
	BITS64 finalKey = 0;
	int piece = EMPTY;
	
	// pieces
	for(sq = 0; sq < BOARDSQN; ++sq) {
		piece = pos->pieces[sq];
		if(piece!=NO_SQ && piece!=EMPTY && piece != OFFBOARD) {
			ASSERT(piece>=wP && piece<=bK);
			finalKey ^= PieceKeys[piece][sq];
		}		
	}
	
	if(pos->side == WHITE) {
		finalKey ^= SideKey;
	}
		
	if(pos->enPas != NO_SQ) {
		ASSERT(pos->enPas>=0 && pos->enPas<BOARDSQN);
		ASSERT(SquareOnB(pos->enPas));
		ASSERT(RanksBoard[pos->enPas] == RANK_3 || RanksBoard[pos->enPas] == RANK_6);
		finalKey ^= PieceKeys[EMPTY][pos->enPas];
	}
	
	ASSERT(pos->castlePerm>=0 && pos->castlePerm<=15);
	
	finalKey ^= CastleKeys[pos->castlePerm];
	
	return finalKey;
}

