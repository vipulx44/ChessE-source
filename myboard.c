#include "stdio.h"
#include "definitions.h"

int PieceListOk(const BOARD *pos) {
	int piece = wP;
	int sq;
	int num;
	for(piece = wP; piece <= bK; ++piece) {
		if(pos->NumofPieces[piece]<0 || pos->NumofPieces[piece]>=10) return FALSE;
	}

	if(pos->NumofPieces[wK]!=1 || pos->NumofPieces[bK]!=1) return FALSE;

	for(piece = wP; piece <= bK; ++piece) {
		for(num = 0; num < pos->NumofPieces[piece]; ++num) {
			sq = pos->pList[piece][num];
			if(!SquareOnB(sq)) return FALSE;
		}
	}
    return TRUE;
}

int Boardcheck(const BOARD *pos) {

	int tempPNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int tempBigPiece[2] = { 0, 0};
	int tempMajPiece[2] = { 0, 0};
	int tempMinPiece[2] = { 0, 0};
	int tempMaterial[2] = { 0, 0};

	int sq64,temppiece,tempPceNum,sq120,colour,pcount;

	BITS64 temppawns[3] = {0ULL, 0ULL, 0ULL};

	temppawns[WHITE] = pos->pawns[WHITE];
	temppawns[BLACK] = pos->pawns[BLACK];
	temppawns[BOTH] = pos->pawns[BOTH];

	// check piece lists
	for(temppiece = wP; temppiece <= bK; ++temppiece) {
		for(tempPceNum = 0; tempPceNum < pos->NumofPieces[temppiece]; ++tempPceNum) {
			sq120 = pos->pList[temppiece][tempPceNum];
			ASSERT(pos->pieces[sq120]==temppiece);
		}
	}

	// check piece count and other counters
	for(sq64 = 0; sq64 < 64; ++sq64) {
		sq120 = GETSQ120(sq64);
		temppiece = pos->pieces[sq120];
		tempPNum[temppiece]++;
		colour = ColourOfPiece[temppiece];
		if( BigPiece[temppiece] == TRUE) tempBigPiece[colour]++;
		if( MinPiece[temppiece] == TRUE) tempMinPiece[colour]++;
		if( MajPiece[temppiece] == TRUE) tempMajPiece[colour]++;

		tempMaterial[colour] += ValueOfPiece[temppiece];
	}

	for(temppiece = wP; temppiece <= bK; ++temppiece) {
		ASSERT(tempPNum[temppiece]==pos->NumofPieces[temppiece]);
	}

	// check bitboards count
	pcount = COUNT(temppawns[WHITE]);
	ASSERT(pcount == pos->NumofPieces[wP]);
	pcount = COUNT(temppawns[BLACK]);
	ASSERT(pcount == pos->NumofPieces[bP]);
	pcount = COUNT(temppawns[BOTH]);
	ASSERT(pcount == (pos->NumofPieces[bP] + pos->NumofPieces[wP]));

	// check bitboards squares
	while(temppawns[WHITE]) {
		sq64 = POP(&temppawns[WHITE]);
		ASSERT(pos->pieces[GETSQ120(sq64)] == wP);
	}

	while(temppawns[BLACK]) {
		sq64 = POP(&temppawns[BLACK]);
		ASSERT(pos->pieces[GETSQ120(sq64)] == bP);
	}

	while(temppawns[BOTH]) {
		sq64 = POP(&temppawns[BOTH]);
		ASSERT( (pos->pieces[GETSQ120(sq64)] == bP) || (pos->pieces[GETSQ120(sq64)] == wP) );
	}

	ASSERT(tempMaterial[WHITE]==pos->material[WHITE] && tempMaterial[BLACK]==pos->material[BLACK]);
	ASSERT(tempMinPiece[WHITE]==pos->pieceMin[WHITE] && tempMinPiece[BLACK]==pos->pieceMin[BLACK]);
	ASSERT(tempMajPiece[WHITE]==pos->pieceMaj[WHITE] && tempMajPiece[BLACK]==pos->pieceMaj[BLACK]);
	ASSERT(tempBigPiece[WHITE]==pos->pieceBig[WHITE] && tempBigPiece[BLACK]==pos->pieceBig[BLACK]);

	ASSERT(pos->side==WHITE || pos->side==BLACK);
	ASSERT(GenKey(pos)==pos->posKey);

	ASSERT(pos->enPas==NO_SQ || ( RanksBoard[pos->enPas]==RANK_6 && pos->side == WHITE)
		 || ( RanksBoard[pos->enPas]==RANK_3 && pos->side == BLACK));

	ASSERT(pos->pieces[pos->KingSq[WHITE]] == wK);
	ASSERT(pos->pieces[pos->KingSq[BLACK]] == bK);

	ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

	ASSERT(PieceListOk(pos));

	return TRUE;
}

void UpdateMaterial(BOARD *pos) {

	int piece,sq,i,colour;

	for(i = 0; i < BOARDSQN; ++i) {
		sq = i;
		piece = pos->pieces[i];
		ASSERT(PceValidEmptyOffbrd(piece));
		if(piece!=OFFBOARD && piece!= EMPTY) {
			colour = ColourOfPiece[piece];
			ASSERT(ValidSide(colour));

		    if( BigPiece[piece] == TRUE) pos->pieceBig[colour]++;
		    if( MinPiece[piece] == TRUE) pos->pieceMin[colour]++;
		    if( MajPiece[piece] == TRUE) pos->pieceMaj[colour]++;

			pos->material[colour] += ValueOfPiece[piece];

			ASSERT(pos->NumofPieces[piece] < 10 && pos->NumofPieces[piece] >= 0);

			pos->pList[piece][pos->NumofPieces[piece]] = sq;
			pos->NumofPieces[piece]++;


			if(piece==wK) pos->KingSq[WHITE] = sq;
			if(piece==bK) pos->KingSq[BLACK] = sq;

			if(piece==wP) {
				SETBIT(pos->pawns[WHITE],GETSQ64(sq));
				SETBIT(pos->pawns[BOTH],GETSQ64(sq));
			} else if(piece==bP) {
				SETBIT(pos->pawns[BLACK],GETSQ64(sq));
				SETBIT(pos->pawns[BOTH],GETSQ64(sq));
			}
		}
	}
}

int FENparse(char *fen, BOARD *pos) {

	ASSERT(fen!=NULL);
	ASSERT(pos!=NULL);

    int  rank = RANK_8;
    int  file = FILE_A;
    int  piece = 0;
    int  count = 0;
    int  i = 0;
	int  sq64 = 0;
	int  sq120 = 0;

	ResetBoard(pos);

	while ((rank >= RANK_1) && *fen) {
	    count = 1;
		switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;

            default:
                printf("FEN error \n");
                return -1;
        }

	for (i = 0; i < count; i++) {
           sq64 = rank * 8 + file;
           sq120 = GETSQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
			file++;
        }
		fen++;
	}

	ASSERT(*fen == 'w' || *fen == 'b');

	pos->side = (*fen == 'w') ? WHITE : BLACK;
	fen += 2;

	for (i = 0; i < 4; i++) {
        if (*fen == ' ') {
            break;
        }
		switch(*fen) {
			case 'K': pos->castlePerm |= WKCA; break;
			case 'Q': pos->castlePerm |= WQCA; break;
			case 'k': pos->castlePerm |= BKCA; break;
			case 'q': pos->castlePerm |= BQCA; break;
			default:	     break;
        }
		fen++;
	}
	fen++;

	ASSERT(pos->castlePerm>=0 && pos->castlePerm <= 15);

	if (*fen != '-') {
		file = fen[0] - 'a';
		rank = fen[1] - '1';

		ASSERT(file>=FILE_A && file <= FILE_H);
		ASSERT(rank>=RANK_1 && rank <= RANK_8);

		pos->enPas = FILERANK2SQ(file,rank);
    }

	pos->posKey = GenKey(pos);

	UpdateMaterial(pos);

	return 0;
}

void ResetBoard(BOARD *pos) {

	int i = 0;

	for(i = 0; i < BOARDSQN; ++i) {
		pos->pieces[i] = OFFBOARD;
	}

	for(i = 0; i < 64; ++i) {
		pos->pieces[GETSQ120(i)] = EMPTY;
	}

	for(i = 0; i < 2; ++i) {
		pos->pieceBig[i] = 0;
		pos->pieceMaj[i] = 0;
		pos->pieceMin[i] = 0;
		pos->material[i] = 0;
	}

	for(i = 0; i < 3; ++i) {
		pos->pawns[i] = 0ULL;
	}

	for(i = 0; i < 13; ++i) {
		pos->NumofPieces[i] = 0;
	}

	pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;

	pos->side = BOTH;
	pos->enPas = NO_SQ;
	pos->fiftyMove = 0;

	pos->ply = 0;
	pos->historyPly = 0;

	pos->castlePerm = 0;

	pos->posKey = 0ULL;

}
void PrintBoard(const BOARD *pos) {

	int sq,file,rank,piece;

	printf("\nGame Board:\n\n");

	for(rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ",rank+1);
		for(file = FILE_A; file <= FILE_H; file++) {
			sq = FILERANK2SQ(file,rank);
			piece = pos->pieces[sq];
			printf("%3c",PieceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   ");
	for(file = FILE_A; file <= FILE_H; file++) {
		printf("%3c",'a'+file);
	}
	printf("\n");
	printf("side:%c\n",SideChar[pos->side]);
	printf("enPas:%d\n",pos->enPas);
	printf("castle:%c%c%c%c\n",
			pos->castlePerm & WKCA ? 'K' : '-',
			pos->castlePerm & WQCA ? 'Q' : '-',
			pos->castlePerm & BKCA ? 'k' : '-',
			pos->castlePerm & BQCA ? 'q' : '-'
			);
	printf("PosKey:%llX\n",pos->posKey);
}

void MirrorBoard(BOARD *pos) {

    int tempPiecesArray[64];
    int tempSide = pos->side^1;
	int SwapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
    int tempCastlePerm = 0;
    int tempEnPas = NO_SQ;

	int sq;
	int tp;

    if (pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
    if (pos->castlePerm & WQCA) tempCastlePerm |= BQCA;

    if (pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
    if (pos->castlePerm & BQCA) tempCastlePerm |= WQCA;

	if (pos->enPas != NO_SQ)  {
        tempEnPas = GETSQ120(Mirror64[GETSQ64(pos->enPas)]);
    }

    for (sq = 0; sq < 64; sq++) {
        tempPiecesArray[sq] = pos->pieces[GETSQ120(Mirror64[sq])];
    }

    ResetBoard(pos);

	for (sq = 0; sq < 64; sq++) {
        tp = SwapPiece[tempPiecesArray[sq]];
        pos->pieces[GETSQ120(sq)] = tp;
    }

	pos->side = tempSide;
    pos->castlePerm = tempCastlePerm;
    pos->enPas = tempEnPas;

    pos->posKey = GenKey(pos);

	UpdateMaterial(pos);

    ASSERT(Boardcheck(pos));
}
