#include "definitions.h"
#include "stdio.h"

#define HASH_PIECE(pce,sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_CASTLE (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPas)]))

const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static void ClearPiece(const int sq, BOARD *pos) {

	ASSERT(SquareOnB(sq));
	ASSERT(Boardcheck(pos));
	
    int pce = pos->pieces[sq];
	
    ASSERT(ValidPiece(pce));
	
	int col = ColourOfPiece[pce];
	int i = 0;
	int tempPieceNum = -1;
	
	ASSERT(ValidSide(col));
	
        HASH_PIECE(pce,sq);
	
    pos->pieces[sq] = EMPTY;
    pos->material[col] -= ValueOfPiece[pce];
	
	if(BigPiece[pce]) {
			pos->pieceBig[col]--;
		if(MajPiece[pce]) {
			pos->pieceMaj[col]--;
		} else {
			pos->pieceMin[col]--;
		}
	} else {
		CLEARBIT(pos->pawns[col],GETSQ64(sq));
		CLEARBIT(pos->pawns[BOTH],GETSQ64(sq));
	}
	
	for(i = 0; i < pos->NumofPieces[pce]; ++i) {
		if(pos->pList[pce][i] == sq) {
			tempPieceNum = i;
			break;
		}
	}
	
	ASSERT(tempPieceNum != -1);
	ASSERT(tempPieceNum>=0&&tempPieceNum<10);
	
	pos->NumofPieces[pce]--;		
	
	pos->pList[pce][tempPieceNum] = pos->pList[pce][pos->NumofPieces[pce]];
  
}


static void AddPiece(const int sq, BOARD *pos, const int pce) {

    ASSERT(ValidPiece(pce));
    ASSERT(SquareOnB(sq));
	
	int colour = ColourOfPiece[pce];
	ASSERT(ValidSide(colour));

    HASH_PIECE(pce,sq);
	
	pos->pieces[sq] = pce;

    if(BigPiece[pce]) {
			pos->pieceBig[colour]++;
		if(MajPiece[pce]) {
			pos->pieceMaj[colour]++;
		} else {
			pos->pieceMin[colour]++;
		}
	} else {
		SETBIT(pos->pawns[colour],GETSQ64(sq));
		SETBIT(pos->pawns[BOTH],GETSQ64(sq));
	}
	
	pos->material[colour] += ValueOfPiece[pce];
	pos->pList[pce][pos->NumofPieces[pce]++] = sq;
	
}

static void MovePiece(const int from, const int to, BOARD *pos) {

    ASSERT(SquareOnB(from));
    ASSERT(SquareOnB(to));
	
	int i = 0;
	int pce = pos->pieces[from];	
	int col = ColourOfPiece[pce];
	ASSERT(ValidSide(col));
        ASSERT(ValidPiece(pce));
	
#ifdef DEBUG
	int t_PieceNum = FALSE;
#endif

	HASH_PIECE(pce,from);
	pos->pieces[from] = EMPTY;
	
	HASH_PIECE(pce,to);
	pos->pieces[to] = pce;
	
	if(!BigPiece[pce]) {
		CLEARBIT(pos->pawns[col],GETSQ64(from));
		CLEARBIT(pos->pawns[BOTH],GETSQ64(from));
		SETBIT(pos->pawns[col],GETSQ64(to));
		SETBIT(pos->pawns[BOTH],GETSQ64(to));		
	}    
	
	for(i = 0; i < pos->NumofPieces[pce]; ++i) {
		if(pos->pList[pce][i] == from) {
			pos->pList[pce][i] = to;
#ifdef DEBUG
			t_PieceNum = TRUE;
#endif
			break;
		}
	}
	ASSERT(t_PieceNum);
}

int MakeMove(BOARD *pos, int move) {

	ASSERT(Boardcheck(pos));
	
	int from = FROMSQIM(move);
        int to = TOSQIM(move);
        int side = pos->side;
	
	ASSERT(SquareOnB(from));
        ASSERT(SquareOnB(to));
        ASSERT(ValidSide(side));
        ASSERT(ValidPiece(pos->pieces[from]));
	ASSERT(pos->historyPly >= 0 && pos->historyPly < MAXGM);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	pos->history[pos->historyPly].posKey = pos->posKey;
	
	if(move & MFLAGEPIM) {
        if(side == WHITE) {
            ClearPiece(to-10,pos);
        } else {
            ClearPiece(to+10,pos);
        }
    } else if (move & MFLAGCAIM) {
        switch(to) {
            case C1:
                MovePiece(A1, D1, pos);
			break;
            case C8:
                MovePiece(A8, D8, pos);
			break;
            case G1:
                MovePiece(H1, F1, pos);
			break;
            case G8:
                MovePiece(H8, F8, pos);
			break;
            default: ASSERT(FALSE); break;
        }
    }	
	
	if(pos->enPas != NO_SQ) HASH_EP;
    
    HASH_CASTLE;
	
    pos->history[pos->historyPly].move = move;
    pos->history[pos->historyPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->historyPly].enPas = pos->enPas;
    pos->history[pos->historyPly].castlePerm = pos->castlePerm;

    
    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPas = NO_SQ;

    HASH_CASTLE;
	
    int captured = CAPTUREDIM(move);
    pos->fiftyMove++;
	
	if(captured != EMPTY) {
        ASSERT(ValidPiece(captured));
        ClearPiece(to, pos);
        pos->fiftyMove = 0;
    }
	
	pos->historyPly++;
	pos->ply++;
	
	ASSERT(pos->historyPly >= 0 && pos->historyPly < MAXGM);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	if(PawnP[pos->pieces[from]]) {
        pos->fiftyMove = 0;
        if(move & MFLAGPSIM) {
            if(side==WHITE) {
                pos->enPas=from+10;
                ASSERT(RanksBoard[pos->enPas]==RANK_3);
            } else {
                pos->enPas=from-10;
                ASSERT(RanksBoard[pos->enPas]==RANK_6);
            }
            HASH_EP;
        }
    }
	
    MovePiece(from, to, pos);
	
    int promPiece = PROMOTEDIM(move);
    if(promPiece != EMPTY)   {
        ASSERT(ValidPiece(promPiece) && !PawnP[promPiece]);
        ClearPiece(to, pos);
        AddPiece(to, pos, promPiece);
    }
	
	if(KingP[pos->pieces[to]]) {
        pos->KingSq[pos->side] = to;
    }
	
	pos->side ^= 1;
    HASH_SIDE;

    ASSERT(Boardcheck(pos));
	
		
	if(Attackedsquare(pos->KingSq[side],pos->side,pos))  {
        TakeMove(pos);
        return FALSE;
    }
	
	return TRUE;
	
}

void TakeMove(BOARD *pos) {
	
	ASSERT(Boardcheck(pos));
	
	pos->historyPly--;
    pos->ply--;
	
	ASSERT(pos->historyPly >= 0 && pos->historyPly < MAXGM);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
    int move = pos->history[pos->historyPly].move;
    int from = FROMSQIM(move);
    int to = TOSQIM(move);	
	
	ASSERT(SquareOnB(from));
    ASSERT(SquareOnB(to));
	
	if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CASTLE;

    pos->castlePerm = pos->history[pos->historyPly].castlePerm;
    pos->fiftyMove = pos->history[pos->historyPly].fiftyMove;
    pos->enPas = pos->history[pos->historyPly].enPas;

    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CASTLE;

    pos->side ^= 1;
    HASH_SIDE;
	
	if(MFLAGEPIM & move) {
        if(pos->side == WHITE) {
            AddPiece(to-10, pos, bP);
        } else {
            AddPiece(to+10, pos, wP);
        }
    } else if(MFLAGCAIM & move) {
        switch(to) {
            case C1: MovePiece(D1, A1, pos); break;
            case C8: MovePiece(D8, A8, pos); break;
            case G1: MovePiece(F1, H1, pos); break;
            case G8: MovePiece(F8, H8, pos); break;
            default: ASSERT(FALSE); break;
        }
    }
	
	MovePiece(to, from, pos);
	
	if(KingP[pos->pieces[from]]) {
        pos->KingSq[pos->side] = from;
    }
	
	int captured = CAPTUREDIM(move);
    if(captured != EMPTY) {
        ASSERT(ValidPiece(captured));
        AddPiece(to, pos, captured);
    }
	
	if(PROMOTEDIM(move) != EMPTY)   {
        ASSERT(ValidPiece(PROMOTEDIM(move)) && !PawnP[PROMOTEDIM(move)]);
        ClearPiece(from, pos);
        AddPiece(from, pos, (ColourOfPiece[PROMOTEDIM(move)] == WHITE ? wP : bP));
    }
	
    ASSERT(Boardcheck(pos));

}


void MakeNullMove(BOARD *pos) {

    ASSERT(Boardcheck(pos));
    ASSERT(!Attackedsquare(pos->KingSq[pos->side],pos->side^1,pos));

    pos->ply++;
    pos->history[pos->historyPly].posKey = pos->posKey;

    if(pos->enPas != NO_SQ) HASH_EP;

    pos->history[pos->historyPly].move = NOMOVE;
    pos->history[pos->historyPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->historyPly].enPas = pos->enPas;
    pos->history[pos->historyPly].castlePerm = pos->castlePerm;
    pos->enPas = NO_SQ;

    pos->side ^= 1;
    pos->historyPly++;
    HASH_SIDE;
   
    ASSERT(Boardcheck(pos));
	ASSERT(pos->historyPly >= 0 && pos->historyPly < MAXGM);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);

    return;
} // MakeNullMove

void TakeNullMove(BOARD *pos) {
    ASSERT(Boardcheck(pos));

    pos->historyPly--;
    pos->ply--;

    if(pos->enPas != NO_SQ) HASH_EP;

    pos->castlePerm = pos->history[pos->historyPly].castlePerm;
    pos->fiftyMove = pos->history[pos->historyPly].fiftyMove;
    pos->enPas = pos->history[pos->historyPly].enPas;

    if(pos->enPas != NO_SQ) HASH_EP;
    pos->side ^= 1;
    HASH_SIDE;
  
    ASSERT(Boardcheck(pos));
	ASSERT(pos->historyPly >= 0 && pos->historyPly < MAXGM);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
}













