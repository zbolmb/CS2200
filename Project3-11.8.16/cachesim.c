#include "cachesim.h"

#define TRUE 1
#define FALSE 0

/**
 * The stuct that you may use to store the metadata for each block in the L1 and L2 caches
 */
typedef struct block_t {
    uint64_t tag; // The tag stored in that block
    uint8_t valid; // Valid bit
    uint8_t dirty; // Dirty bit

    /**************** TODO ******************/

    /*
        Add another variable here to perform the LRU replacement. Look into using a counter
        variable that will keep track of the oldest block in a set
    */
    uint64_t timestamp;

} block;


/**
 * A struct for storing the configuration of both the L1 and L2 caches as passed in the
 * cache_init function. All values represent number of bits. You may add any parameters
 * here, however I strongly suggest not removing anything from the config struct
 */
typedef struct config_t {
    uint64_t C1; /* Size of cache L1 */
    uint64_t C2; /* Size of cache L2 */
    uint64_t S; /* Set associativity of L2 */
    uint64_t B; /* Block size of both caches */
} config;


/****** Do not modify the below function headers ******/
static uint64_t get_tag(uint64_t address, uint64_t C, uint64_t B, uint64_t S);
static uint64_t get_index(uint64_t address, uint64_t C, uint64_t B, uint64_t S);
static uint64_t convert_tag(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_index(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_tag_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_index_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);

/****** You may add Globals and other function headers that you may need below this line ******/
block* L1;
block* L2;
config* conf;
int counter = 0;

/**
 * Subroutine for initializing your cache with the passed in arguments.
 * You may initialize any globals you might need in this subroutine
 *
 * @param C1 The total size of your L1 cache is 2^C1 bytes
 * @param C2 The tatal size of your L2 cache is 2^C2 bytes
 * @param S The total number of blocks in a line/set of your L2 cache are 2^S
 * @param B The size of your blocks is 2^B bytes
 */
void cache_init(uint64_t C1, uint64_t C2, uint64_t S, uint64_t B)
{
    conf->C1 = C1;
    conf->C2 = C2;
    conf->S = S;
    conf->B = B;
    L1 = (block*) malloc(1 << (C1 - B) * sizeof(*L1));
    L2 = (block*) malloc(1 << (C2 - B) * sizeof(*L2));
}

/**
 * Subroutine that simulates one cache event at a time.
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 */
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats)
{
    /*
        Suggested approach:
            -> Find the L1 tag and index of the address that is being passed in to the function
            -> Check if there is a hit in the L1 cache
            -> If L1 misses, check the L2 cache for a hit (Hint: If L2 hits, update L1 with new values)
            -> If L2 misses, need to get values from memory, and update L2 and L1 caches

            * We will leave it upto you to decide what must be updated and when
     */
    stats->accesses++;
    if (rw == 'r') {
        stats->reads++;
    } else {
        stats->writes++;
    }

    int l1index = get_index(address, conf->C1, conf->B, 1);
    int l2index = get_index(address, conf->C2, conf->B, conf->S);
    int l1tag = get_tag(address, conf->C1, conf->B, 1);
    int l2tag = get_tag(address, conf->C2, conf->B, conf->S);
    block l1block = L1[l1index];
    block l2block;
    //int l2block = L2[l2index];
    if (l1block.valid && l1block.tag == l1tag) {
        l1block.timestamp = counter;
    } else {
        if (rw == 'r') {
            stats->l1_read_misses++;
        } else {
            stats->l1_write_misses++;
        }
        int hit = 0;
        int i;
        for (i = 0; i < (1 << conf->S); i++) {
            l2block = L2[i * l2index];
            if (l2block.valid && l2block.tag == l2tag) {
                hit = 1;
                l2block.timestamp = counter;
                l1block.tag = l2tag;
                l1block.dirty = 1;
                l1block.valid = 1;
                l1block.timestamp = counter;
                break;
            }
        }
        //missed
        if (hit == 0) {
            if (rw == 'r') {
                stats->l2_read_misses++;
            } else {
                stats->l2_write_misses++;
            }
            int leastUsed = counter;
            block leastUsedBlock = L2[l2index];
            int replaced = 0;
            for (i = 0; i < (1 << conf->S); i++) {
                l2block = L2[i * l2index];
                if (l2block.valid == 0) {
                    replaced = 1;
                    l2block.valid = 1;
                    l2block.tag = l2tag;
                    l2block.timestamp = counter;
                    break;
                } else {
                    if (l2block.timestamp < leastUsed) {
                        leastUsed = l2block.timestamp;
                        leastUsedBlock = l2block;
                    }
                }
            }
            if (replaced == 0) {
                leastUsedBlock.valid = 1;
                leastUsedBlock.tag = l2tag;
                leastUsedBlock.timestamp = counter;
            }
            l1block.valid = 1;
            l1block.dirty = 1;
            l1block.tag = l2tag;
            l1block.timestamp = counter;
        }
    }
    counter++;

    //loop through all sets of L2
    //check valid && tag
    //writeback++ only when L2 -> L1
	// } else if () { //L2, use convert_tag / index here if writeback l
 //        l1block.dirty = 1;
 //    } else {
 //    //get values from mem
 //        miss++;
 //    }
}

/**
 * Subroutine for freeing up memory, and performing any final calculations before the statistics
 * are outputed by the driver
 */
void cache_cleanup (struct cache_stats_t *stats)
{
    /*
        Make sure to free up all the memory you malloc'ed here. To check if you have freed up the
        the memory, run valgrind. For more information, google how to use valgrind.
    */
    stats->read_misses = stats->l1_read_misses + stats->l2_read_misses;
    stats->write_misses = stats->l1_write_misses + stats->l2_write_misses;
    stats->misses = stats->read_misses + stats->write_misses;
    stats->l1_miss_rate = (stats->l1_read_misses + stats->l1_write_misses)/stats->accesses;
    stats->l2_miss_rate = (stats->l2_read_misses + stats->l2_write_misses)/stats->accesses;
    stats->l2_avg_access_time = stats->l2_access_time + stats->l2_miss_rate + stats->memory_access_time;
    stats->avg_access_time = stats->l1_access_time + stats->l1_miss_rate + stats->l2_avg_access_time;
    free(L1);
    free(L2);
}

/**
 * Subroutine to compute the Tag of a given address based on the parameters passed in
 *
 * @param address The address whose tag is to be computed
 * @param C The size of the cache in bits (i.e. Size of cache is 2^C)
 * @param B The size of the cache block in bits (i.e. Size of block is 2^B)
 * @param S The set associativity of the cache in bits (i.e. Set-Associativity is 2^S)
 *
 * @return The computed tag
 */
static uint64_t get_tag(uint64_t address, uint64_t C, uint64_t B, uint64_t S)
{
    int t = 64 - (C - B - S);
    return ((1 << t) - 1) << (64 - t) | address;
}

/**
 * Subroutine to compute the Index of a given address based on the parameters passed in
 *
 * @param address The address whose tag is to be computed
 * @param C The size of the cache in bits (i.e. Size of cache is 2^C)
 * @param B The size of the cache block in bits (i.e. Size of block is 2^B)
 * @param S The set associativity of the cache in bits (i.e. Set-Associativity is 2^S)
 *
 * @return The computed index
 */
static uint64_t get_index(uint64_t address, uint64_t C, uint64_t B, uint64_t S)
{
    return ((1 << C - B - S) - 1) | address;
}


/**** DO NOT MODIFY CODE BELOW THIS LINE UNLESS YOU ARE ABSOLUTELY SURE OF WHAT YOU ARE DOING ****/

/*
    Note:   The below functions will be useful in converting the L1 tag and index into corresponding L2
            tag and index. These should be used when you are evicitng a block from the L1 cache, and
            you need to update the block in L2 cache that corresponds to the evicted block.

            The newly added functions will be useful for converting L2 indecies ang tags into the corresponding
            L1 index and tags. Make sure to understand how they are working.
*/

/**
 * This function converts the tag stored in an L1 block and the index of that L1 block into corresponding
 * tag of the L2 block
 *
 * @param tag The tag that needs to be converted (i.e. L1 tag)
 * @param index The index of the L1 cache (i.e. The index from which the tag was found)
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 */
static uint64_t convert_tag(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S)
{
    uint64_t reconstructed_address = (tag << (C1 - B)) | index;
    return reconstructed_address >> (C2 - B - S);
}

/**
 * This function converts the tag stored in an L1 block and the index of that L1 block into corresponding
 * index of the L2 block
 *
 * @param tag The tag stored in the L1 index
 * @param index The index of the L1 cache (i.e. The index from which the tag was found)
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 */
static uint64_t convert_index(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B,  uint64_t S)
{
    // Reconstructed address without the block offset bits
    uint64_t reconstructed_address = (tag << (C1 - B)) | index;
    // Create index mask for L2 without including the block offset bits
    return reconstructed_address & ((1 << (C2 - S - B)) - 1);
}

/**
 * This function converts the tag stored in an L2 block and the index of that L2 block into corresponding
 * tag of the L1 cache
 *
 * @param l2_tag The L2 tag
 * @param l2_index The index of the L2 block
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 * @return The L1 tag linked to the L2 index and tag
 */
static uint64_t convert_tag_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S) {
    uint64_t reconstructed_address = (l2_tag << (C2 - B - S)) | l2_index;
    return reconstructed_address >> (C1 - B);
}

/**
 * This function converts the tag stored in an L2 block and the index of that L2 block into corresponding
 * index of the L1 block
 *
 * @param l2_tag The L2 tag
 * @param l2_index The index of the L2 block
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 * @return The L1 index of the L2 block
 */
static uint64_t convert_index_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S) {
    uint64_t reconstructed_address = (l2_tag << (C2 - B - S)) | l2_index;
    return reconstructed_address & ((1 << (C1 - B)) - 1);
}
