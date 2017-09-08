#include "stdio.h"
#include "definitions.h"

const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 }; 
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;

const int PawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int KnightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int BishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int RookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

const int KingE[64] = {	
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	
};

const int KingO[64] = {	
	0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70		
};
// sjeng 11.2
//8/6R1/2k5/6P1/8/8/4nP2/6K1 w - - 1 41 
int MaterialDraw(const BOARD *pos) {

	ASSERT(Boardcheck(pos));
	
    if (!pos->NumofPieces[wR] && !pos->NumofPieces[bR] && !pos->NumofPieces[wQ] && !pos->NumofPieces[bQ]) {
	  if (!pos->NumofPieces[bB] && !pos->NumofPieces[wB]) {
	      if (pos->NumofPieces[wN] < 3 && pos->NumofPieces[bN] < 3) {  return TRUE; }
	  } else if (!pos->NumofPieces[wN] && !pos->NumofPieces[bN]) {
	     if (abs(pos->NumofPieces[wB] - pos->NumofPieces[bB]) < 2) { return TRUE; }
	  } else if ((pos->NumofPieces[wN] < 3 && !pos->NumofPieces[wB]) || (pos->NumofPieces[wB] == 1 && !pos->NumofPieces[wN])) {
	    if ((pos->NumofPieces[bN] < 3 && !pos->NumofPieces[bB]) || (pos->NumofPieces[bB] == 1 && !pos->NumofPieces[bN]))  { return TRUE; }
	  }
	} else if (!pos->NumofPieces[wQ] && !pos->NumofPieces[bQ]) {
        if (pos->NumofPieces[wR] == 1 && pos->NumofPieces[bR] == 1) {
            if ((pos->NumofPieces[wN] + pos->NumofPieces[wB]) < 2 && (pos->NumofPieces[bN] + pos->NumofPieces[bB]) < 2)	{ return TRUE; }
        } else if (pos->NumofPieces[wR] == 1 && !pos->NumofPieces[bR]) {
            if ((pos->NumofPieces[wN] + pos->NumofPieces[wB] == 0) && (((pos->NumofPieces[bN] + pos->NumofPieces[bB]) == 1) || ((pos->NumofPieces[bN] + pos->NumofPieces[bB]) == 2))) { return TRUE; }
        } else if (pos->NumofPieces[bR] == 1 && !pos->NumofPieces[wR]) {
            if ((pos->NumofPieces[bN] + pos->NumofPieces[bB] == 0) && (((pos->NumofPieces[wN] + pos->NumofPieces[wB]) == 1) || ((pos->NumofPieces[wN] + pos->NumofPieces[wB]) == 2))) { return TRUE; }
        }
    }
  return FALSE;
}

#define ENDGAME_MAT (1 * ValueOfPiece[wR] + 2 * ValueOfPiece[wN] + 2 * ValueOfPiece[wP] + ValueOfPiece[wK])

int PosEval(const BOARD *pos) {

	ASSERT(Boardcheck(pos));

	int pce;
	int pceNum;
	int sq;
	int score = pos->material[WHITE] - pos->material[BLACK];
	
	if(!pos->NumofPieces[wP] && !pos->NumofPieces[bP] && MaterialDraw(pos) == TRUE) {
		return 0;
	}
	
	pce = wP;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(GETSQ64(sq)>=0 && GETSQ64(sq)<=63);
		score += PawnTable[GETSQ64(sq)];	
		
		if( (IsolatedMask[GETSQ64(sq)] & pos->pawns[WHITE]) == 0) {
			//printf("wP Iso:%s\n",PrSq(sq));
			score += PawnIsolated;
		}
		
		if( (WhitePassedMask[GETSQ64(sq)] & pos->pawns[BLACK]) == 0) {
			//printf("wP Passed:%s\n",PrSq(sq));
			score += PawnPassed[RanksBoard[sq]];
		}
		
	}	

	pce = bP;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(MIRROR64(GETSQ64(sq))>=0 && MIRROR64(GETSQ64(sq))<=63);
		score -= PawnTable[MIRROR64(GETSQ64(sq))];	
		
		if( (IsolatedMask[GETSQ64(sq)] & pos->pawns[BLACK]) == 0) {
			//printf("bP Iso:%s\n",PrSq(sq));
			score -= PawnIsolated;
		}
		
		if( (BlackPassedMask[GETSQ64(sq)] & pos->pawns[WHITE]) == 0) {
			//printf("bP Passed:%s\n",PrSq(sq));
			score -= PawnPassed[7 - RanksBoard[sq]];
		}
	}	
	
	pce = wN;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(GETSQ64(sq)>=0 && GETSQ64(sq)<=63);
		score += KnightTable[GETSQ64(sq)];
	}	

	pce = bN;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(MIRROR64(GETSQ64(sq))>=0 && MIRROR64(GETSQ64(sq))<=63);
		score -= KnightTable[MIRROR64(GETSQ64(sq))];
	}			
	
	pce = wB;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(GETSQ64(sq)>=0 && GETSQ64(sq)<=63);
		score += BishopTable[GETSQ64(sq)];
	}	

	pce = bB;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(MIRROR64(GETSQ64(sq))>=0 && MIRROR64(GETSQ64(sq))<=63);
		score -= BishopTable[MIRROR64(GETSQ64(sq))];
	}	

	pce = wR;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(GETSQ64(sq)>=0 && GETSQ64(sq)<=63);
		score += RookTable[GETSQ64(sq)];
		
		ASSERT(ValidFileRank(FilesBoard[sq]));
		
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBoard[sq]])) {
			score += RookOpenFile;
		} else if(!(pos->pawns[WHITE] & FileBBMask[FilesBoard[sq]])) {
			score += RookSemiOpenFile;
		}
	}	

	pce = bR;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(MIRROR64(GETSQ64(sq))>=0 && MIRROR64(GETSQ64(sq))<=63);
		score -= RookTable[MIRROR64(GETSQ64(sq))];
		ASSERT(ValidFileRank(FilesBoard[sq]));
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBoard[sq]])) {
			score -= RookOpenFile;
		} else if(!(pos->pawns[BLACK] & FileBBMask[FilesBoard[sq]])) {
			score -= RookSemiOpenFile;
		}
	}	
	
	pce = wQ;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(GETSQ64(sq)>=0 && GETSQ64(sq)<=63);
		ASSERT(ValidFileRank(FilesBoard[sq]));
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBoard[sq]])) {
			score += QueenOpenFile;
		} else if(!(pos->pawns[WHITE] & FileBBMask[FilesBoard[sq]])) {
			score += QueenSemiOpenFile;
		}
	}	

	pce = bQ;	
	for(pceNum = 0; pceNum < pos->NumofPieces[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SquareOnB(sq));
		ASSERT(GETSQ64(sq)>=0 && GETSQ64(sq)<=63);
		ASSERT(ValidFileRank(FilesBoard[sq]));
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBoard[sq]])) {
			score -= QueenOpenFile;
		} else if(!(pos->pawns[BLACK] & FileBBMask[FilesBoard[sq]])) {
			score -= QueenSemiOpenFile;
		}
	}	
	//8/p6k/6p1/5p2/P4K2/8/5pB1/8 b - - 2 62 
	pce = wK;
	sq = pos->pList[pce][0];
	ASSERT(SquareOnB(sq));
	ASSERT(GETSQ64(sq)>=0 && GETSQ64(sq)<=63);
	
	if( (pos->material[BLACK] <= ENDGAME_MAT) ) {
		score += KingE[GETSQ64(sq)];
	} else {
		score += KingO[GETSQ64(sq)];
	}
	
	pce = bK;
	sq = pos->pList[pce][0];
	ASSERT(SquareOnB(sq));
	ASSERT(MIRROR64(GETSQ64(sq))>=0 && MIRROR64(GETSQ64(sq))<=63);
	
	if( (pos->material[WHITE] <= ENDGAME_MAT) ) {
		score -= KingE[MIRROR64(GETSQ64(sq))];
	} else {
		score -= KingO[MIRROR64(GETSQ64(sq))];
	}
	
	if(pos->NumofPieces[wB] >= 2) score += BishopPair;
	if(pos->NumofPieces[bB] >= 2) score -= BishopPair;
	
	if(pos->side == WHITE) {
		return score;
	} else {
		return -score;
	}	
}


















