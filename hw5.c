#include <stdio.h>

#define cacheSize 64
#define blockSize 8
#define n         8 // number of blocks per set (for fullyAssoc, n = cacheSize/blockSize)

int addresses[] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72,
  76, 80, 0, 4, 8, 12, 16, 71, 3, 41, 81, 39, 38, 71, 15, 39, 11, 51, 57, 41};

// represents a row (block) of cache
struct cacheLine {
	int valid;
	int tag;
};

// represents a set of cache blocks
struct cacheSet {
	int oldestLine;
	struct cacheLine rows[n];
};

int isHitOrMiss(struct cacheSet * cache, int k, int tag, int set);
int findLine(struct cacheSet * cache, int k, int set);
int loadBlock(struct cacheSet * cache, int k, int tag, int set, int row);
void initializeCache(struct cacheSet * cache, int setSize, int numSets);

int main()
{
	int numBlocks = cacheSize / blockSize;
	int numSets   = numBlocks / n;
	
	struct cacheSet cache[numSets];

	initializeCache(cache, n, numSets);

	int count;
	for(count = 0; count < sizeof(addresses) / sizeof(int); count++) {
		int address = addresses[count];
		int blockNumber = address / blockSize;
		int offset = address % blockSize;
		int set    = blockNumber % numSets;
		int tag    = blockNumber / numSets;
		int answer = isHitOrMiss(cache, n, tag, set);
		printf("%4d: Hit: %d (Tag/Set#/Offset: %d/%d/%d)\n", address, answer, tag, set, offset);
		if(!answer) { // if cache miss
			int cacheLine = findLine(cache, n, set);  // which block should we replace?
			loadBlock(cache, n, tag, set, cacheLine); // load the new block
		}
	}
	printf("Cache contents:\n");
	for(count=0;count<numBlocks;count++) {
		int set = count/n;
		int row = count%n;
		printf("%4d: Valid: %d; Tag: %d (Set #: %3d)\n", count, cache[set].rows[row].valid, 
			cache[set].rows[row].tag, set);
	}
}

// is there a valid line with given tag in the given set?
int isHitOrMiss(struct cacheSet * cache, int k, int tag, int set)
{
	int row;
	for(row = 0; row < k; row++)
		if(cache[set].rows[row].valid && cache[set].rows[row].tag == tag)
			return 1;
	return 0;
}

// determines which line in the given set should be replaced next
int findLine(struct cacheSet * cache, int k, int set)
{
	int row;
	for(row = 0; row < k; row++)
		if(!cache[set].rows[row].valid)
			return row;
	return cache[set].oldestLine;
}

// loads block with given tag, updates the oldest line in the set
int loadBlock(struct cacheSet * cache, int k, int tag, int set, int row)
{
	cache[set].rows[row].tag = tag;
	cache[set].rows[row].valid = 1;
	cache[set].oldestLine = (cache[set].oldestLine + 1) % k;
}

void initializeCache(struct cacheSet * cache, int setSize, int numSets)
{
	int set, row;
	for(set = 0; set < numSets; set++) {
		cache[set].oldestLine = 0;
		for(row = 0; row < setSize; row++) {
			cache[set].rows[row].valid  = 0;
			cache[set].rows[row].tag    = 0;
		}
	}
}
