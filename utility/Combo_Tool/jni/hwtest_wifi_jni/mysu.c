#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("error!\n");
		return -1;
	}
	
	int i = 0;
	char Cmd[126] = "";
	char *pCur = NULL;
	pCur = Cmd;
	
	for (i = 1; i < argc; i++)
	{
		strcat(pCur, argv[i]);
		strcat(pCur, " ");
	}
	
	system(Cmd);
	
	printf("%s", Cmd);
	
	return 0;
}