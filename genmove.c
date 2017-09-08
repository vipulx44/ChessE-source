#include "stdio.h"
#include "definitions.h"

#define MOVE(from,to,ca,pro,fl) ( (from) | ((to) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define SQOFFBOARD(square) (FilesBoard[(square)]==OFFBOARD)

const int SlidePieceLoop[8] = {
 wB, wR, wQ, 0, bB, bR, bQ, 0
};

const int NonSlidePieceLoop[6] = {
 wN, wK, 0, bN, bK, 0
};

const int SlideIndexLoop[2] = { 0, 4 };
const int NonSlideIndexLoop[2] = { 0, 3 };

const int PieceDirections[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,1, 10, 0, 0, 0, 0 },
	{ -1, -10,1, 10, -9, -11, 11, 9 },
	{ -1, -10,1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,1, 10, 0, 0, 0, 0 },
	{ -1, -10,1, 10, -9, -11, 11, 9 },
	{ -1, -10,1, 10, -9, -11, 11, 9 }
};

const int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

/*
PV Move
Cap -> MvvLVA
Killers
HistoryScore

*/
const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };
static int MvvLvaScores[13][13];

void InitMostvalvicLeavalatck() {
	int Attacker;
	int Victim;
	for(Attacker = wP; Attacker <= bK; ++Attacker) {
		for(Victim = wP; Victim <= bK; ++Victim) {
			MvvLvaScores[Victim][Attacker] = VictimScore[Victim] + 6 - ( VictimScore[Attacker] / 100);
		}
	}
}

int DoesMoveExist(BOARD *pos, const int move) {

    MOVELIST list[1];
    GenAllMoves(pos,list);

    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        TakeMove(pos);
	if(list->moves[MoveNum].move == move) {
		return TRUE;
	}
    }
	return FALSE;
}

static void AddNormalMove( const BOARD *pos, int move, MOVELIST *list ) {

	ASSERT(SquareOnB(FROMSQIM(move)));
	ASSERT(SquareOnB(TOSQIM(move)));
	ASSERT(Boardcheck(pos));
	ASSERT(pos->ply >=0 && pos->ply < MAXDEPTH);

	list->moves[list->count].move = move;

	if(pos->searchKillers[0][pos->ply] == move) {
		list->moves[list->count].score = 900000;
	} else if(pos->searchKillers[1][pos->ply] == move) {
		list->moves[list->count].score = 800000;
	} else {
		list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQIM(move)]][TOSQIM(move)];
	}
	list->count++;
}

static void AddCaptureMove( const BOARD *pos, int move, MOVELIST *list ) {

	ASSERT(SquareOnB(FROMSQIM(move)));
	ASSERT(SquareOnB(TOSQIM(move)));
	ASSERT(ValidPiece(CAPTUREDIM(move)));
	ASSERT(Boardcheck(pos));

	list->moves[list->count].move = move;
	list->moves[list->count].score = MvvLvaScores[CAPTUREDIM(move)][pos->pieces[FROMSQIM(move)]] + 1000000;
	list->count++;
}

static void AddEnPassantMove( const BOARD *pos, int move, MOVELIST *list ) {

	ASSERT(SquareOnB(FROMSQIM(move)));
	ASSERT(SquareOnB(TOSQIM(move)));
	ASSERT(Boardcheck(pos));
	ASSERT((RanksBoard[TOSQIM(move)]==RANK_6 && pos->side == WHITE) || (RanksBoard[TOSQIM(move)]==RANK_3 && pos->side == BLACK));

	list->moves[list->count].move = move;
	list->moves[list->count].score = 105 + 1000000;
	list->count++;
}

static void AddWhitePawnCapMove( const BOARD *pos, const int from, const int to, const int cap, MOVELIST *list ) {

	ASSERT(PieceValidEmpty(cap));
	ASSERT(SquareOnB(from));
	ASSERT(SquareOnB(to));
	ASSERT(Boardcheck(pos));

	if(RanksBoard[from] == RANK_7) {
		AddCaptureMove(pos, MOVE(from,to,cap,wQ,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wR,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wB,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wN,0), list);
	} else {
		AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
	}
}

static void AddWhitePawnMove( const BOARD *pos, const int from, const int to, MOVELIST *list ) {

	ASSERT(SquareOnB(from));
	ASSERT(SquareOnB(to));
	ASSERT(Boardcheck(pos));

	if(RanksBoard[from] == RANK_7) {
		AddNormalMove(pos, MOVE(from,to,EMPTY,wQ,0), list);
		AddNormalMove(pos, MOVE(from,to,EMPTY,wR,0), list);
		AddNormalMove(pos, MOVE(from,to,EMPTY,wB,0), list);
		AddNormalMove(pos, MOVE(from,to,EMPTY,wN,0), list);
	} else {
		AddNormalMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
	}
}

static void AddBlackPawnCapMove( const BOARD *pos, const int from, const int to, const int cap, MOVELIST *list ) {

	ASSERT(PieceValidEmpty(cap));
	ASSERT(SquareOnB(from));
	ASSERT(SquareOnB(to));
	ASSERT(Boardcheck(pos));

	if(RanksBoard[from] == RANK_2) {
		AddCaptureMove(pos, MOVE(from,to,cap,bQ,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bR,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bB,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bN,0), list);
	} else {
		AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
	}
}

static void AddBlackPawnMove( const BOARD *pos, const int from, const int to, MOVELIST *list ) {

	ASSERT(SquareOnB(from));
	ASSERT(SquareOnB(to));
	ASSERT(Boardcheck(pos));

	if(RanksBoard[from] == RANK_2) {
		AddNormalMove(pos, MOVE(from,to,EMPTY,bQ,0), list);
		AddNormalMove(pos, MOVE(from,to,EMPTY,bR,0), list);
		AddNormalMove(pos, MOVE(from,to,EMPTY,bB,0), list);
		AddNormalMove(pos, MOVE(from,to,EMPTY,bN,0), list);
	} else {
		AddNormalMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
	}
}

void GenAllMoves(const BOARD *pos, MOVELIST *list) {

	ASSERT(Boardcheck(pos));

	list->count = 0;

	int piece = EMPTY;
	int side = pos->side;
	int sq = 0; int t_sq = 0;
	int pieceNum = 0;
	int dir = 0;
	int i = 0;
	int pieceI = 0;

	if(side == WHITE) {

		for(pieceNum = 0; pieceNum < pos->NumofPieces[wP]; ++pieceNum) {
			sq = pos->pList[wP][pieceNum];
			ASSERT(SquareOnB(sq));

			if(pos->pieces[sq + 10] == EMPTY) {
				AddWhitePawnMove(pos, sq, sq+10, list);
				if(RanksBoard[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
					AddNormalMove(pos, MOVE(sq,(sq+20),EMPTY,EMPTY,MFLAGPSIM),list);
				}
			}

			if(!SQOFFBOARD(sq + 9) && ColourOfPiece[pos->pieces[sq + 9]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
			}
			if(!SQOFFBOARD(sq + 11) && ColourOfPiece[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
			}

			if(pos->enPas != NO_SQ) {
				if(sq + 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEPIM), list);
				}
				if(sq + 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEPIM), list);
				}
			}
		}

		if(pos->castlePerm & WKCA) {
			if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
				if(!Attackedsquare(E1,BLACK,pos) && !Attackedsquare(F1,BLACK,pos) ) {
					AddNormalMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCAIM), list);
				}
			}
		}

		if(pos->castlePerm & WQCA) {
			if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
				if(!Attackedsquare(E1,BLACK,pos) && !Attackedsquare(D1,BLACK,pos) ) {
					AddNormalMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCAIM), list);
				}
			}
		}

	} else {

		for(pieceNum = 0; pieceNum < pos->NumofPieces[bP]; ++pieceNum) {
			sq = pos->pList[bP][pieceNum];
			ASSERT(SquareOnB(sq));

			if(pos->pieces[sq - 10] == EMPTY) {
				AddBlackPawnMove(pos, sq, sq-10, list);
				if(RanksBoard[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
					AddNormalMove(pos, MOVE(sq,(sq-20),EMPTY,EMPTY,MFLAGPSIM),list);
				}
			}

			if(!SQOFFBOARD(sq - 9) && ColourOfPiece[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
			}

			if(!SQOFFBOARD(sq - 11) && ColourOfPiece[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
			}
			if(pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEPIM), list);
				}
				if(sq - 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEPIM), list);
				}
			}
		}

		// castling
		if(pos->castlePerm &  BKCA) {
			if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
				if(!Attackedsquare(E8,WHITE,pos) && !Attackedsquare(F8,WHITE,pos) ) {
					AddNormalMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCAIM), list);
				}
			}
		}

		if(pos->castlePerm &  BQCA) {
			if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
				if(!Attackedsquare(E8,WHITE,pos) && !Attackedsquare(D8,WHITE,pos) ) {
					AddNormalMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCAIM), list);
				}
			}
		}
	}

	/* Loop for slide pieces */
	pieceI = SlideIndexLoop[side];
	piece = SlidePieceLoop[pieceI++];
	while( piece != 0) {
		ASSERT(ValidPiece(piece));

		for(pieceNum = 0; pieceNum < pos->NumofPieces[piece]; ++pieceNum) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SquareOnB(sq));

			for(i = 0; i < NumDir[piece]; ++i) {
				dir = PieceDirections[piece][i];
				t_sq = sq + dir;

				while(!SQOFFBOARD(t_sq)) {
					
					if(pos->pieces[t_sq] != EMPTY) {
						if( ColourOfPiece[pos->pieces[t_sq]] == (side ^ 1)) {
							AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
						}
						break;
					}
					AddNormalMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
					t_sq += dir;
				}
			}
		}

		piece = SlidePieceLoop[pieceI++];
	}

	/* Loop for non slide */
	pieceI = NonSlideIndexLoop[side];
	piece = NonSlidePieceLoop[pieceI++];

	while( piece != 0) {
		ASSERT(ValidPiece(piece));

		for(pieceNum = 0; pieceNum < pos->NumofPieces[piece]; ++pieceNum) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SquareOnB(sq));

			for(i = 0; i < NumDir[piece]; ++i) {
				dir = PieceDirections[piece][i];
				t_sq = sq + dir;

				if(SQOFFBOARD(t_sq)) {
					continue;
				}

				// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
				if(pos->pieces[t_sq] != EMPTY) {
					if( ColourOfPiece[pos->pieces[t_sq]] == (side ^ 1)) {
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}
				AddNormalMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
			}
		}

		piece = NonSlidePieceLoop[pieceI++];
	}

    ASSERT(MoveListOk(list,pos));
}


void GenAllCaps(const BOARD *pos, MOVELIST *list) {

	ASSERT(Boardcheck(pos));

	list->count = 0;

	int piece = EMPTY;
	int side = pos->side;
	int sq = 0; int t_sq = 0;
	int pieceNum = 0;
	int dir = 0;
	int i = 0;
	int pieceI = 0;

	if(side == WHITE) {

		for(pieceNum = 0; pieceNum < pos->NumofPieces[wP]; ++pieceNum) {
			sq = pos->pList[wP][pieceNum];
			ASSERT(SquareOnB(sq));

			if(!SQOFFBOARD(sq + 9) && ColourOfPiece[pos->pieces[sq + 9]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
			}
			if(!SQOFFBOARD(sq + 11) && ColourOfPiece[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
			}

			if(pos->enPas != NO_SQ) {
				if(sq + 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEPIM), list);
				}
				if(sq + 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEPIM), list);
				}
			}
		}

	} else {

		for(pieceNum = 0; pieceNum < pos->NumofPieces[bP]; ++pieceNum) {
			sq = pos->pList[bP][pieceNum];
			ASSERT(SquareOnB(sq));

			if(!SQOFFBOARD(sq - 9) && ColourOfPiece[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
			}

			if(!SQOFFBOARD(sq - 11) && ColourOfPiece[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
			}
			if(pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEPIM), list);
				}
				if(sq - 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEPIM), list);
				}
			}
		}
	}

	/* Loop for slide pieces */
	pieceI = SlideIndexLoop[side];
	piece = SlidePieceLoop[pieceI++];
	while( piece != 0) {
		ASSERT(ValidPiece(piece));

		for(pieceNum = 0; pieceNum < pos->NumofPieces[piece]; ++pieceNum) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SquareOnB(sq));

			for(i = 0; i < NumDir[piece]; ++i) {
				dir = PieceDirections[piece][i];
				t_sq = sq + dir;

				while(!SQOFFBOARD(t_sq)) {
					// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
					if(pos->pieces[t_sq] != EMPTY) {
						if( ColourOfPiece[pos->pieces[t_sq]] == (side ^ 1)) {
							AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
						}
						break;
					}
					t_sq += dir;
				}
			}
		}

		piece = SlidePieceLoop[pieceI++];
	}

	/* Loop for non slide */
	pieceI = NonSlideIndexLoop[side];
	piece = NonSlidePieceLoop[pieceI++];

	while( piece != 0) {
		ASSERT(ValidPiece(piece));

		for(pieceNum = 0; pieceNum < pos->NumofPieces[piece]; ++pieceNum) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SquareOnB(sq));

			for(i = 0; i < NumDir[piece]; ++i) {
				dir = PieceDirections[piece][i];
				t_sq = sq + dir;

				if(SQOFFBOARD(t_sq)) {
					continue;
				}

			
				if(pos->pieces[t_sq] != EMPTY) {
					if( ColourOfPiece[pos->pieces[t_sq]] == (side ^ 1)) {
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}
			}
		}

		piece = NonSlidePieceLoop[pieceI++];
	}
    ASSERT(MoveListOk(list,pos));
}




