#include "definitions.h"
#include "stdio.h"
#include "stdlib.h"

#define RAND_64 	((BITS64)rand() | \
			(BITS64)rand() << 15 | \
			(BITS64)rand() << 30 | \
			(BITS64)rand() << 45 | \
			((BITS64)rand() & 0xf) << 60 )

int Sq120ToSq64[BOARDSQN];
int Sq64ToSq120[64];

BITS64 SetMask[64];
BITS64 ClearMask[64];

BITS64 PieceKeys[13][120];
BITS64 SideKey;
BITS64 CastleKeys[16];

int FilesBoard[BOARDSQN];
int RanksBoard[BOARDSQN];

BITS64 FileBBMask[8];
BITS64 RankBBMask[8];

BITS64 BlackPassedMask[64];
BITS64 WhitePassedMask[64];
BITS64 IsolatedMask[64];

void EvalMaskInit() {

	int sq, tempsq, r, f;

	for(sq = 0; sq < 8; ++sq) {
        FileBBMask[sq] = 0ULL;
		RankBBMask[sq] = 0ULL;
	}

	for(r = RANK_8; r >= RANK_1; r--) {
        for (f = FILE_A; f <= FILE_H; f++) {
            sq = r * 8 + f;
            FileBBMask[f] |= (1ULL << sq);
            RankBBMask[r] |= (1ULL << sq);
        }
	}

	for(sq = 0; sq < 64; ++sq) {
		IsolatedMask[sq] = 0ULL;
		WhitePassedMask[sq] = 0ULL;
		BlackPassedMask[sq] = 0ULL;
    }

	for(sq = 0; sq < 64; ++sq) {
		tempsq = sq + 8;

        while(tempsq < 64) {
            WhitePassedMask[sq] |= (1ULL << tempsq);
            tempsq += 8;
        }

        tempsq = sq - 8;
        while(tempsq >= 0) {
            BlackPassedMask[sq] |= (1ULL << tempsq);
            tempsq -= 8;
        }

        if(FilesBoard[GETSQ120(sq)] > FILE_A) {
            IsolatedMask[sq] |= FileBBMask[FilesBoard[GETSQ120(sq)] - 1];

            tempsq = sq + 7;
            while(tempsq < 64) {
                WhitePassedMask[sq] |= (1ULL << tempsq);
                tempsq += 8;
            }

            tempsq = sq - 9;
            while(tempsq >= 0) {
                BlackPassedMask[sq] |= (1ULL << tempsq);
                tempsq -= 8;
            }
        }

        if(FilesBoard[GETSQ120(sq)] < FILE_H) {
            IsolatedMask[sq] |= FileBBMask[FilesBoard[GETSQ120(sq)] + 1];

            tempsq = sq + 9;
            while(tempsq < 64) {
                WhitePassedMask[sq] |= (1ULL << tempsq);
                tempsq += 8;
            }

            tempsq = sq - 7;
            while(tempsq >= 0) {
                BlackPassedMask[sq] |= (1ULL << tempsq);
                tempsq -= 8;
            }
        }
	}
}

void FRBoardInit() {

	int i = 0;
	int file = FILE_A;
	int rank = RANK_1;
	int sq = A1;

	for(i = 0; i < BOARDSQN; ++i) {
		FilesBoard[i] = OFFBOARD;
		RanksBoard[i] = OFFBOARD;
	}

	for(rank = RANK_1; rank <= RANK_8; ++rank) {
		for(file = FILE_A; file <= FILE_H; ++file) {
			sq = FILERANK2SQ(file,rank);
			FilesBoard[sq] = file;
			RanksBoard[sq] = rank;
		}
	}
}

void InitHashKeys() {

	int i = 0;
	int j = 0;
	for(i = 0; i < 13; ++i) {
		for(j = 0; j < 120; ++j) {
			PieceKeys[i][j] = RAND_64;
		}
	}
	SideKey = RAND_64;
	for(i = 0; i < 16; ++i) {
		CastleKeys[i] = RAND_64;
	}

}

void InitBitMasks() {
	int i = 0;

	for(i = 0; i < 64; i++) {
		SetMask[i] = 0ULL;
		ClearMask[i] = 0ULL;
	}

	for(i = 0; i < 64; i++) {
		SetMask[i] |= (1ULL << i);
		ClearMask[i] = ~SetMask[i];
	}
}

void InitSq120To64() {

	int i = 0;
	int file = FILE_A;
	int rank = RANK_1;
	int sq = A1;
	int sq64 = 0;
	for(i = 0; i < BOARDSQN; ++i) {
		Sq120ToSq64[i] = 65;
	}

	for(i = 0; i < 64; ++i) {
		Sq64ToSq120[i] = 120;
	}

	for(rank = RANK_1; rank <= RANK_8; ++rank) {
		for(file = FILE_A; file <= FILE_H; ++file) {
			sq = FILERANK2SQ(file,rank);
			ASSERT(SquareOnB(sq));
			Sq64ToSq120[sq64] = sq;
			Sq120ToSq64[sq] = sq64;
			sq64++;
		}
	}
}

void AllInit() {
InitSq120To64();
InitBitMasks();
InitHashKeys();
FRBoardInit();
EvalMaskInit();
InitMostvalvicLeavalatck();
}
