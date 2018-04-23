#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int logtwo(int x);

struct reference
{
	int address;
	char RorW;
};

int main(void)
{
	int i;
	int endindex;
	int blocksize;
	int sets;
	int ass;
	int offset;
	int index;
	int tag;
	int totalblocks;
	//struct reference memref[100];
	scanf("%d", &blocksize);
	//printf("%d", blocksize);
	scanf("%d", &sets);
	//printf("%d", sets);
	scanf("%d", &ass);
	//printf("%d", ass);


	//for (i = 0; i < 100; i++)
	{
	//	if (scanf("%c %d",memref[i].RorW, memref[i].address) == 0) break;
	}
	//endindex = i;

	offset = logtwo(blocksize);
	index = logtwo(sets);
	tag = 32 - offset - index;
	totalblocks = sets * ass;

	printf("Block size: %d\n", blocksize);
	printf("Number of sets: %d\n", sets);
	printf("Associativity: %d\n", ass);
	printf("Number of offset bits: %d\n", offset);
	printf("Number of index bits: %d\n", index);
	printf("Number of tag bits: %d\n", tag);
}

int logtwo(int x)
{
	int i = 1;
	int y = 1;
	while (1)
	{
		if (y >= x) break;
		y = y * 2;
		i++;
	}

	return i - 1;
}