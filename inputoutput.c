#include "stdio.h"
#include "definitions.h"

char *PrintSquare(const int sq) {

	static char SqStr[3];

	int file = FilesBoard[sq];
	int rank = RanksBoard[sq];

	sprintf(SqStr, "%c%c", ('a'+file), ('1'+rank));

	return SqStr;

}

char *PrintMove(const int move) {

	static char MvStr[6];

	int ff = FilesBoard[FROMSQIM(move)];
	int rf = RanksBoard[FROMSQIM(move)];
	int ft = FilesBoard[TOSQIM(move)];
	int rt = RanksBoard[TOSQIM(move)];

	int promoted = PROMOTEDIM(move);

	if(promoted) {
		char pchar = 'q';
		if(IsKNT(promoted)) {
			pchar = 'n';
		} else if(IsRKQN(promoted) && !IsBISQN(promoted))  {
			pchar = 'r';
		} else if(!IsRKQN(promoted) && IsBISQN(promoted))  {
			pchar = 'b';
		}
		sprintf(MvStr, "%c%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt), pchar);
	} else {
		sprintf(MvStr, "%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt));
	}

	return MvStr;
}

int ParseMove(char *ptr, BOARD *pos) {

	ASSERT(Boardcheck(pos));

    if(ptr[1] > '8' || ptr[1] < '1') return NOMOVE;
    if(ptr[3] > '8' || ptr[3] < '1') return NOMOVE;
    if(ptr[0] > 'h' || ptr[0] < 'a') return NOMOVE;
    if(ptr[2] > 'h' || ptr[2] < 'a') return NOMOVE;

    int from = FILERANK2SQ(ptr[0] - 'a', ptr[1] - '1');
    int to = FILERANK2SQ(ptr[2] - 'a', ptr[3] - '1');

    ASSERT(SquareOnB(from) && SquareOnB(to));

    MOVELIST list[1];
    GenAllMoves(pos,list);
    int MoveNum = 0;
	int Move = 0;
	int PromPce = EMPTY;

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		Move = list->moves[MoveNum].move;
		if(FROMSQIM(Move)==from && TOSQIM(Move)==to) {
			PromPce = PROMOTEDIM(Move);
			if(PromPce!=EMPTY) {
				if(IsRKQN(PromPce) && !IsBISQN(PromPce) && ptr[4]=='r') {
					return Move;
				} else if(!IsRKQN(PromPce) && IsBISQN(PromPce) && ptr[4]=='b') {
					return Move;
				} else if(IsRKQN(PromPce) && IsBISQN(PromPce) && ptr[4]=='q') {
					return Move;
				} else if(IsKNT(PromPce)&& ptr[4]=='n') {
					return Move;
				}
				continue;
			}
			return Move;
		}
    }

    return NOMOVE;
}

void PrintGenMoves(const MOVELIST *list) {
	int i = 0;
	int score = 0;
	int move = 0;
	printf("MoveList:\n");

	for(i = 0; i < list->count; ++i) {

		move = list->moves[i].move;
		score = list->moves[i].score;

		printf("Move:%d > %s (score:%d)\n",i+1,PrintMove(move),score);
	}
	printf("MoveList Total %d Moves:\n\n",list->count);
}














