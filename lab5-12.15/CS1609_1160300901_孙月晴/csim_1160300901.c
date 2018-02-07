#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "cachelab.h"

//#define DEBUG_ON
#define ADDRESS_LENGTH 64

/* Type: Memory address */
typedef unsigned long long int mem_addr_t;

/* Type: Cache line
   LRU is a counter used to implement LRU replacement policy  */
typedef struct cache_line {
    bool valid;
    mem_addr_t tag;
    mem_addr_t lru;
} cache_line_t;

typedef cache_line_t *cache_set_t;
typedef cache_set_t *cache_t;

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Derived from command line args */
unsigned int S; /* number of sets */
unsigned int B; /* block size (bytes) */

/* Counters used to record cache statistics */
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;

/* The cache we are simulating */
cache_t cache;
mem_addr_t set_index_mask;

/*
 * initCache - Allocate memory, write 0's for valid and tag and LRU
 * also computes the set_index_mask
 */
void initCache()
{
    if ((cache = calloc(S, sizeof(cache_set_t))) == NULL) /* 组初始化 */
    {
        perror("Failed to calloc entry_of_sets");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < S; ++i)  /* 每组的行初始化*/
    {
        if ((cache[i] = calloc(E, sizeof(cache_line_t))) == NULL)
        {
            perror("Failed to calloc line in sets");
        }
    }

}


/*
 * freeCache - free allocated memory
 */
void freeCache()
{
    for (int i = 0; i < S; ++i)
    {
        free(cache[i]);
    }
    free(cache);
}


/*
 * accessData - Access data at memory address addr.
 *   If it is already in cache, increast hit_count
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase eviction_count if a line is evicted.
 *
 *accessData - 访问内存地址addr处的数据。
 * 如果它已经在高速缓存中，则增加hit_count，如果它不在高速缓存中，则将其放入高速缓存中，增加未中数。
 *  如果一条线被驱逐，也增加eviction_count。
 */
void accessData(mem_addr_t addr)
{
    mem_addr_t set_index = (addr >> b) & set_index_mask;
    mem_addr_t tag = (addr >> b) >> s;
    cache_set_t search_line = cache[set_index];
    mem_addr_t furthest = INT_MAX;
    mem_addr_t Recent = 0;
    int oldest_block = INT_MAX;
    bool hit_flag = false;

    for (int i = 0; i < E; ++ i)
    {
        if (search_line[i].tag == tag && search_line[i].valid) /* 命中 */
        {
            hit_flag = true;
            ++hit_count;
            ++search_line[i].lru; /* 更新LRU */
            break;
        }
    }

    if (!hit_flag)  /* 不命中 */
    {
        ++miss_count;
        int i;
        for (i = 0; i < E; ++i)    /* 寻找时间最远的块 */
        {
            if (search_line[i].lru < furthest)
            {
                furthest = search_line[i].lru;
                oldest_block = i;
            }
            if (search_line[i].lru > Recent)    /* 更新 */
            {
                Recent = search_line[i].lru;
            }
        }

        search_line[oldest_block].lru = Recent + 1;
        search_line[oldest_block].tag = tag;

        if (search_line[oldest_block].valid) /* 有效块被驱逐*/
        {
            ++eviction_count;
        }
        else
        {
            search_line[oldest_block].valid = true;
        }
    }
}


/*
 * replayTrace - replays the given trace file against the cache
 */
void replayTrace(char* trace_fn)
{
    char buf[1000];
    mem_addr_t addr=0;
    unsigned int len=0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if(!trace_fp){
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while( fgets(buf, 1000, trace_fp) != NULL) {
        if(buf[1]=='S' || buf[1]=='L' || buf[1]=='M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);

            if(verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            accessData(addr);

            /* If the instruction is R/W then access again */
            if(buf[1]=='M')
                accessData(addr);

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * main - Main routine
 */
int main(int argc, char* argv[])
{
    char c;

    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
            case 's':
                s = atoi(optarg); //字符串转整数
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            case 'h':
                printUsage(argv);
                exit(0);
            default:
                printUsage(argv);
                exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

    /* Compute S, E and B from command line args */
    S = 1 << s;
    B = 1 << b;
    set_index_mask = (1 << s) - 1;
    /* Initialize cache */
    initCache();

//#ifdef DEBUG_ON
    printf("DEBUG: S:%u E:%u B:%u trace:%s\n", S, E, B, trace_file);
    printf("DEBUG: set_index_mask: %llu\n", set_index_mask);
//#endif

    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
