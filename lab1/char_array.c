#include <stdio.h>
#include <string.h>

char *phrases[2] = {"Hello MAJhhh", "Whats Up"};
int SIZE = 2;
int main()
{


	int i, j;
	for(i = 0; i < SIZE; i ++)
	{	

		for(j = 0; j < strlen(phrases[i]) + 1; j++)
		{
			printf("%c", phrases[i][j]);
		}

		printf("\n");
	}
	return 0;
}