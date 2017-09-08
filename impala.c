#include "stdio.h"
#include "definitions.h"
#include "stdlib.h"
#include "string.h"

int main() {

AllInit();

	
BOARD pos[1];  //new board pointer
SEARCHINFO info[1]; // new search pointer
    
	
info->quit = FALSE;
pos->HashTable->pTable = NULL;//hashtable first entry points null
HashInit(pos->HashTable, 64);  //hash table memory initialized, pTable points to first address.
	
setbuf(stdin, NULL);
setbuf(stdout, NULL);
	
printf("Welcome to Impala! Type 'console' for console mode...\n");

char line[256];
while (TRUE) {
	memset(&line[0], 0, sizeof(line));

	fflush(stdout);
	if (!fgets(line, 256, stdin))
	continue;
	if (line[0] == '\n')
	continue;
	if (!strncmp(line, "uci",3)) {
	UCI(pos, info);
	if(info->quit == TRUE) break;
	continue;
	} else if (!strncmp(line, "xboard",6))	{
	XBOARD(pos, info);
	if(info->quit == TRUE) break;
	continue;
	} else if (!strncmp(line, "console",4))	{
	CONSOLE(pos, info);
	if(info->quit == TRUE) break;
	continue;
	} else if(!strncmp(line, "quit",4))	{
	break;
	}
	}

free(pos->HashTable->pTable);
	
return 0;
}








