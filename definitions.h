#ifndef DEFS_H
#define DEFS_H

#include "stdlib.h"
#include "stdio.h"

// #define DEBUG
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed",#n); \
printf("On %s ",__DATE__); \
printf("At %s ",__TIME__); \
printf("In File %s ",__FILE__); \
printf("At Line %d\n",__LINE__); \
exit(1);}
#endif

typedef unsigned long long BITS64;

#define NAME "Impala 1.0"
#define AUTHOR "Vipul Sankhe"
#define BOARDSQN 120
#define MAXH 1024
#define MAXGM 2048
#define MAXPM 256
#define MAXDEPTH 64
#define STARTFEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define INFINITE 30000
#define ISMATE (INFINITE - MAXDEPTH)

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK  };//0,1,2,3,4,5,6,7,8,9,10,11,12

enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };//0,1,2,3,4,5,6,7,8

enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };//0,1,2,3,4,5,6,7,8

enum { WHITE, BLACK, BOTH };//0,1,2

enum { UCIMODE, XBOARDMODE, CONSOLEMODE };//0,1,2

enum {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};//21,22,23,24,25,26,27,28,29,30, NO_SQ=99, OFFBOARD=100

enum { FALSE, TRUE };//0,1

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };



typedef struct {//struct for move
int move;
int score;
} MOVE;


typedef struct {//struct for movelist

int count;
MOVE moves[MAXPM];
} MOVELIST;


enum {HFNONE, HFALPHA, HFBETA, HFEXACT};


typedef struct {//struct for hashentry
int depth;
int flags;
BITS64 posKey;
int move;
int score;
} HASHENTRY;


typedef struct {//struct for hashtable
int hit;
int cut;
int newWrite;
int overWrite;
HASHENTRY *pTable;
int numEntries;

} HASHTABLE;


typedef struct {//for undoing move
int enPas;
int fiftyMove;
BITS64 posKey;
int move;
int castlePerm;

} UNDO;

typedef struct {//structure for main board position

int pieces[BOARDSQN];
BITS64 pawns[3];
int NumofPieces[13];
int pieceBig[2];
int pieceMaj[2];
int pieceMin[2];
int material[2];

int KingSq[2];

int side;
int enPas;
int fiftyMove;

int ply;
int historyPly;

int castlePerm;

BITS64 posKey;

int searchHistory[13][BOARDSQN];
int searchKillers[2][MAXDEPTH];



UNDO history[MAXGM];

// piece list
int pList[13][10];

HASHTABLE HashTable[1];
int PvArray[MAXDEPTH];

} BOARD;//s board array created usually to be used as pointer



typedef struct {//struct for search information
    
float fh;
float fhf;
int nullCut;

int GAME_MODE;

int timeset;
int movestogo;
long nodes;

int starttime;
int stoptime;
int depth;

int quit;
int stopped;

int POST_THINKING;
} SEARCHINFO;




//macros for moves
#define FROMSQIM(m) ((m) & 0x7F) //for move, finds from sq
#define TOSQIM(m) (((m)>>7) & 0x7F)//for move, finds to sq
#define CAPTUREDIM(m) (((m)>>14) & 0xF) //for move, finds captured piece
#define PROMOTEDIM(m) (((m)>>20) & 0xF) //for move, finds promoted piece
#define MFLAGEPIM 0x40000  //flag ep
#define MFLAGPSIM 0x80000 // flag ps
#define MFLAGCAIM 0x1000000 //flap castle
#define MFLAGCAPIM 0x7C000
#define MFLAGPROMIM 0xF00000



#define NOMOVE 0


//Macros
#define FILERANK2SQ(f,r) ( (21 + (f) ) + ( (r) * 10 ) ) //file and rank from sq
#define GETSQ64(sq120) (Sq120ToSq64[(sq120)]) // 64 sq from 120 sq
#define GETSQ120(sq64) (Sq64ToSq120[(sq64)]) // 120 sq from 64 sq
#define POP(b) PopBit(b) //pops first 1 bit with index
#define COUNT(b) CountBits(b) // counts 1 bits
#define CLEARBIT(bb,sq) ((bb) &= ClearMask[(sq)]) // clears bit
#define SETBIT(bb,sq) ((bb) |= SetMask[(sq)])  //sets bit
#define IsBISQN(p) (BishopQueenP[(p)])
#define IsRKQN(p) (RookQueenP[(p)])
#define IsKNT(p) (KnightP[(p)])
#define IsKING(p) (KingP[(p)])
#define MIRROR64(sq) (Mirror64[(sq)])

//globals
extern int Sq120ToSq64[BOARDSQN]; //120 to 64 array
extern int Sq64ToSq120[64]; // 64 to 120 array
extern BITS64 SetMask[64]; //setbit
extern BITS64 ClearMask[64]; // clearbit
extern BITS64 PieceKeys[13][120]; //generates 64 bit key for piece on particular sq
extern BITS64 SideKey; // key for side
extern BITS64 CastleKeys[16]; // key for casperm
extern char PieceChar[]; // char PNBRQKpnbrqk
extern char SideChar[];//character wb-
extern char RankChar[]; // char 12345678
extern char FileChar[]; // char abcdefgh
extern int BigPiece[13]; //big pieces
extern int MajPiece[13]; // major pieces
extern int MinPiece[13]; //minor pieces
extern int ValueOfPiece[13]; // piece values
extern int ColourOfPiece[13]; //piece colour
extern int PawnP[13]; //true for pawn
extern int FilesBoard[BOARDSQN]; //sq belongs to file
extern int RanksBoard[BOARDSQN]; //sq belongs to rank
extern int KnightP[13]; //true only for knight
extern int KingP[13];
extern int RookQueenP[13];
extern int BishopQueenP[13];
extern int SlidingPieces[13]; // whether sliding or not
extern int Mirror64[64];
extern BITS64 FileBBMask[8]; // file mask eval imp
extern BITS64 RankBBMask[8]; //rank mask eval imp
extern BITS64 BlackPassedMask[64]; //passes pawn
extern BITS64 WhitePassedMask[64];
extern BITS64 IsolatedMask[64]; //isolated pawn

//functions
//init.c
extern void AllInit(); //itiliazation of all funcs in init
//bb.c
extern void PrintBB(BITS64 bb); //prints bb
extern int PopBit(BITS64 *bb); //func for popbit
extern int CountBits(BITS64 b);//counts no of 1
// hashkeys.c
extern BITS64 GenKey(const BOARD *pos); // generates all poskeys
// board.c
extern void ResetBoard(BOARD *pos); 
extern int FENparse(char *fen, BOARD *pos); // parsing fen
extern void PrintBoard(const BOARD *pos);
extern void UpdateMaterial(BOARD *pos); // material vals updates
extern int Boardcheck(const BOARD *pos);
extern void MirrorBoard(BOARD *pos);
// attack.c
extern int Attackedsquare(const int sq, const int side, const BOARD *pos);
// io.c
extern char *PrintMove(const int move);
extern char *PrintSquare(const int sq);
extern void PrintGenMoves(const MOVELIST *list); //prints gen mov list
extern int ParseMove(char *ptrr, BOARD *pos);
//validate.c
extern int SquareOnB(const int sq);//chk ssq on board
extern int ValidSide(const int side);
extern int ValidFileRank(const int fr); //valid file and rank
extern int PieceValidEmpty(const int pce);
extern int ValidPiece(const int pce); // checks valid piece for checkboard
extern void MirrorEvalTest(BOARD *pos);
extern int SqIs120(const int sq);
extern int PceValidEmptyOffbrd(const int pce);//valid piece on empty or offboard
extern int MoveListOk(const MOVELIST *list,  const BOARD *pos);//for checkboard
extern void DebugAnalysisTest(BOARD *pos, SEARCHINFO *info);
// movegen.c
extern void GenAllMoves(const BOARD *pos, MOVELIST *list);//all moves generation
extern void GenAllCaps(const BOARD *pos, MOVELIST *list);
extern int DoesMoveExist(BOARD *pos, const int move);
extern void InitMostvalvicLeavalatck();// move ordering
// makemove.c
extern int MakeMove(BOARD *pos, int move);
extern void TakeMove(BOARD *pos); //undo move
extern void MakeNullMove(BOARD *pos);
extern void TakeNullMove(BOARD *pos);
// perft.c
extern void TestPerft(int depth, BOARD *pos); //perft debugging
// search.c
extern void SearchPositionITDEEP(BOARD *pos, SEARCHINFO *info); //iterative deepening
// misc.c
extern int GetTimeMs(); // gets comp time
extern void ReadInput(SEARCHINFO *info);
// pvtable.c
extern void HashInit(HASHTABLE *table, const int MB); // hash table init
extern void HashEstore(BOARD *pos, const int move, int score, const int flags, const int depth);
extern int HashEprobe(BOARD *pos, int *move, int *score, int alpha, int beta, int depth); // take hash entry
extern int ProbePvMove(const BOARD *pos);
extern int GetPvLine(const int depth, BOARD *pos);
extern void HashTableClear(HASHTABLE *table);
// evaluate.c
extern int PosEval(const BOARD *pos); //position evaluation
extern void MirrorEvalTest(BOARD *pos) ;
// uci.c
extern void UCI(BOARD *pos, SEARCHINFO *info); // uci prot
// xboard.c
extern void XBOARD(BOARD *pos, SEARCHINFO *info); // xboard prot
extern void CONSOLE(BOARD *pos, SEARCHINFO *info);


#endif















/* GAME MOVE */

/*
0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F
0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF
0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
*/
