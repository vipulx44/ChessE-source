all:
	gcc xboard.c impala.c uci.c evaluate.c pvtab.c init.c bb.c poskeys.c myboard.c data.c sqattack.c inputoutput.c genmove.c val.c makemove.c perft.c search.c extra.c -o finalimpala -O2 -s