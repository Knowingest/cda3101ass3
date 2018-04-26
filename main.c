#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct reference
{
    unsigned int index;
    unsigned int tag;
    char RorW;
};

struct cache_block
{
    int tag;
    int lastused;
    int written;
};

int logtwo(int x);  //cmath is for closers

int main(void)
{
	int i, y;
	int endindex;
	int blocksize;
	int sets;
	int ass;
	int offset;
	int index;
	int tag;
	int totalblocks;
    unsigned int addresses[100];
    int totalrefs, hits, misses, memrefs;
    char RorW[200];
    struct cache_block* cache;
    struct reference reftable[100];
    int cycle;
    int incache;
    int setoffset;
    int lru;
    int lru_index;
	//struct reference memref[100];
	scanf("%d", &blocksize);
	//printf("%d", blocksize);
	scanf("%d", &sets);                //read in first three parameters
	//printf("%d", sets);
	scanf("%d", &ass);
	//printf("%d", ass);

    y = 0;

	for (i = 0; i < 100; i++)
	{
    //printf("scanf loop running %d times\n", ++y);
	   if (scanf("%s", &RorW[i]) == EOF) break;        //populate arrays
       scanf("%d", &addresses[i]);
	}
	endindex = i;

    for (;i < 100; i++)
    {
        RorW[i] = '\0';     //zero out unused portion of the arrays, just in case.
        addresses[i] = -1;
    }

    //printf("ending index = %d\n", endindex);

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

    printf("Printing reference data...\n");
    for (i = 0; i < endindex; i++)
    {
        //printf("iteration %d\n", i);
        printf("%c %d\n", RorW[i], addresses[i]);
    }

    /*printf("RorW data:\n");
    for (i = 0; i < endindex; i++)
        {
            printf("Character[%d] = %c\n", i, RorW[i]);
        }*/

    for (i = 0; i < endindex; i++)  //populate broken down reference table
    {
        reftable[i].RorW = RorW[i];
        reftable[i].index = ((addresses[i] << (tag)) >> (tag)) >> offset;
        reftable[i].tag = addresses[i] >> (offset + index);
    }

    printf("printing detailed reference data....\n");
    for (i = 0; i < endindex; i++)
    {
        printf("Address: %d\n", addresses[i]);
        printf("RorW: %c\n", reftable[i].RorW);
        printf("index: %d\n", reftable[i].index);
        printf("tag: %d\n", reftable[i].tag);
    }


    cache = (struct cache_block*) malloc (totalblocks * sizeof(struct cache_block));
    for (i = 0; i < totalblocks; i++)
    {
        cache[i].tag = -1;
        cache[i].lastused = -1;
        cache[i].written = 0;
    }
    ///////////////////////////////////
    //write through no write allocate//
    ///////////////////////////////////
    totalrefs = 0;
    memrefs = 0;
    hits = 0;
    misses = 0;
    incache = 0;
    i = 0;
    setoffset = 0;


    for (cycle = 0; cycle < endindex; cycle++)
    {
        lru = 101;
        incache = 0;            //find if block is in cache

        //printf("cache[38].lastused = %d [39] = %d\n", cache[38].lastused, cache[39].lastused);

        for (i = reftable[cycle].index * ass; i < (reftable[cycle].index * ass) + ass; i++)
        {
            //printf("checking cache[%d] for index %d tag %d\n", i, reftable[cycle].index, reftable[cycle].tag);
            //reftable[cycle].index * ass == the raw array index inside the cache
            if (cache[i].tag == reftable[cycle].tag)
            {
                incache = 1;    //set flag
                break;
            }
        }

        //if (!incache) printf("index: %d tag: %d not found in cache\n", reftable[cycle].index, reftable[cycle].tag);

        if (reftable[cycle].RorW == 'R')        //READ
        {
            //printf("FOUND READ\n");
            if (incache)
            {
                printf("READ HIT\n");
                hits++; 
                cache[i].lastused = cycle;  //increment hit counter and update when block was last used
                continue;
            }
            else
            {           //start at beginning of set, and end at end of set
                //printf("READ MISS\n");
                lru_index = reftable[cycle].index * ass;
                for (i = reftable[cycle].index * ass; i < (reftable[cycle].index * ass) + ass; i++)
                    if (cache[i].lastused < lru)    //lru starts at 101 each cycle so this will always be true once
                        {
                            //printf("%d is less than %d\n", cache[i].lastused, lru);
                            lru = cache[i].lastused;  //basically we're finding which element of the set was least recently used
                            lru_index = i;
                        }
                cache[lru_index].tag = reftable[cycle].tag; //we found lru so replace it with new value
                misses++;
                memrefs++;      //update counters
                cache[lru_index].lastused = cycle;
                //printf("index: %d tag: %d put in cache\n", reftable[cycle].index, reftable[cycle].tag);
                //printf("cache[%d].tag = %d\n", lru_index, cache[lru_index].tag);
                continue;
            }
        }

        else        //WRITE
        {
            if (incache)
            {
                hits++;
                memrefs++;
                cache[i].lastused = cycle;
                continue;
            }
            else
            {
                misses++;
                memrefs++;
                continue;
            }
        }
    }



    free(cache);
    cache = (struct cache_block*) malloc (totalblocks * sizeof(struct cache_block));
    for (i = 0; i < totalblocks; i++)
    {
        cache[i].tag = -1;
        cache[i].lastused = -1;
        cache[i].written = 0;
    }

    printf("****************************************\nWrite-through with No Write Allocate\n****************************************\n");
    printf("Total number of references: %d\n", endindex);
    printf("Hits: %d\n", hits);
    printf("Misses: %d\n", misses);
    printf("Memory References: %d\n", memrefs);
    
    ///////////////////////////////////
    //write back with write allocate //
    ///////////////////////////////////

    totalrefs = 0;
    memrefs = 0;
    hits = 0;
    misses = 0;
    incache = 0;
    i = 0;
    setoffset = 0;


    for (cycle = 0; cycle < endindex; cycle++)
    {
        lru = 101;
        incache = 0;            //find if block is in cache

        //printf("cache[38].lastused = %d [39] = %d\n", cache[38].lastused, cache[39].lastused);

        for (i = reftable[cycle].index * ass; i < (reftable[cycle].index * ass) + ass; i++)
        {
            //printf("checking cache[%d] for index %d tag %d\n", i, reftable[cycle].index, reftable[cycle].tag);
            //reftable[cycle].index * ass == the raw array index inside the cache
            if (cache[i].tag == reftable[cycle].tag)
            {
                incache = 1;    //set flag
                break;
            }
        }

        //if (!incache) printf("index: %d tag: %d not found in cache\n", reftable[cycle].index, reftable[cycle].tag);

        if (reftable[cycle].RorW == 'R')        //READ
        {
            //printf("FOUND READ\n");
            if (incache)
            {
                //printf("READ HIT\n");
                hits++; 
                cache[i].lastused = cycle;  //increment hit counter and update when block was last used
                continue;
            }
            else
            {           //start at beginning of set, and end at end of set
                //printf("READ MISS\n");
                lru_index = reftable[cycle].index * ass;
                for (i = reftable[cycle].index * ass; i < (reftable[cycle].index * ass) + ass; i++)
                    if (cache[i].lastused < lru)    //lru starts at 101 each cycle so this will always be true once
                        {
                            //printf("%d is less than %d\n", cache[i].lastused, lru);
                            lru = cache[i].lastused;  //basically we're finding which element of the set was least recently used
                            lru_index = i;
                        }
                if (cache[lru_index].written == 1) memrefs++;
                cache[lru_index].tag = reftable[cycle].tag; //we found lru so replace it with new value
                cache[lru_index].written = 0;
                misses++;
                memrefs++;      //update counters
                cache[lru_index].lastused = cycle;
                //printf("index: %d tag: %d put in cache\n", reftable[cycle].index, reftable[cycle].tag);
                //printf("cache[%d].tag = %d\n", lru_index, cache[lru_index].tag);
                continue;
            }
        }

        else        //WRITE
        {
            if (incache)
            {
                hits++;
                cache[i].lastused = cycle;
                cache[i].written = 1;
                continue;
            }
            else
            {
            	lru_index = reftable[cycle].index * ass;
                for (i = reftable[cycle].index * ass; i < (reftable[cycle].index * ass) + ass - 1; i++)
                    if (cache[i].lastused < lru)    //lru starts at 101 each cycle so this will always be true once
                        {
                            //printf("%d is less than %d\n", cache[i].lastused, lru);
                            lru = cache[i].lastused;  //basically we're finding which element of the set was least recently used
                            lru_index = i;
                        }
                if (cache[lru_index].written == 1) memrefs++;
                cache[lru_index].tag = reftable[cycle].tag;
                cache[lru_index].written = 1;
                cache[lru_index].lastused = cycle;
                misses++;
                memrefs++;

                continue;
            }
        }
    }

    printf("****************************************\nWrite-back with Write Allocate\n****************************************\n");
    printf("Total number of references: %d\n", endindex);
    printf("Hits: %d\n", hits);
    printf("Misses: %d\n", misses);
    printf("Memory References: %d\n", memrefs);
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