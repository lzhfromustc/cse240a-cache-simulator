//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"
#include <stdio.h>

//
// TODO:Student Information
//
const char *studentName = "Ziheng Liu";
const char *studentID   = "A59010078";
const char *email       = "zil060@ucsd.edu";

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;     // Number of sets in the I$
uint32_t icacheAssoc;    // Associativity of the I$
uint32_t icacheHitTime;  // Hit Time of the I$

uint32_t dcacheSets;     // Number of sets in the D$
uint32_t dcacheAssoc;    // Associativity of the D$
uint32_t dcacheHitTime;  // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive

uint32_t blocksize;      // Block/Line size
uint32_t memspeed;       // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;       // I$ references
uint64_t icacheMisses;     // I$ misses
uint64_t icachePenalties;  // I$ penalties

uint64_t dcacheRefs;       // D$ references
uint64_t dcacheMisses;     // D$ misses
uint64_t dcachePenalties;  // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

// Utilities
// #define DEBUG

// L1 I-cache

typedef struct icache_block_struct {
  uint32_t tag;
  uint8_t valid;
  uint32_t lru; // when a block is used, set its lru to 0, and all other blocks in the same set have their lru ++.
                // TODO: when a block's lru is too large, decrease lru
} icache_block;

typedef struct icache_set_struct {
  icache_block *blocks;
} icache_set;

typedef struct icache_struct {
  icache_set *sets;
} icache;

icache icacheObj;
icache dcacheObj;
icache l2cacheObj;


//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Utilities
uint32_t my_pow2(uint32_t input) {
  return 1 << input;
}

uint32_t my_log2(uint32_t input) {
  for (uint32_t i = 0; i < 100; i++) {
    uint32_t try = 1 << i;
    if (try == input) {
      return i;
    }
  }
  return 0;
}

uint32_t get_index(uint32_t addr, uint32_t num_sets) {
  // and addr with 00000...111..000
  uint32_t offset_bit = my_log2(blocksize);
  uint32_t index_bit = my_log2(num_sets);

  uint32_t mask = (1 << (offset_bit + index_bit)) - 1 - ((1 << offset_bit) - 1);
  return (addr & mask) >> offset_bit;
}

uint32_t get_tag(uint32_t addr, uint32_t num_sets) {
  // and addr with 11111...0000
  uint32_t offset_bit = my_log2(blocksize);
  uint32_t index_bit = my_log2(num_sets);
  // uint32_t tag_bit = 32 - offset_bit - index_bit;
  
  // uint32_t mask = (1 << tag_bit) - 1;
  // mask = mask << (offset_bit + index_bit);
  // printf("\tTest:mask is %u\n", mask);
  return (addr) >> (offset_bit + index_bit);
}

void lru_increase_except(icache_set set, uint32_t assoc, int except) {
  for (int i = 0; i < assoc; i++) {
    if (i == except) {
      continue;
    }
    set.blocks[i].lru ++;
    if (set.blocks[i].lru == UINT32_MAX - 1) {
      printf("Error in lru_increase_except: lru is equal to UINT32_MAX - 1\n");
    }
  }
}

void set_overwrite_block(icache_set set, int block_id, uint32_t assoc, uint32_t tag) {
  set.blocks[block_id].tag = tag;
  set.blocks[block_id].lru = 0;
  set.blocks[block_id].valid = 1;
  lru_increase_except(set, assoc, block_id);
}

void test_my_util() {
  if (my_log2(2048) != 11) {
    printf("\tError in Test util: my_log2(2048) == %u\n", my_log2(2048));
  }
  if (my_pow2(13) != 8192) {
    printf("\tError in Test util: my_pow2(13) == %u\n", my_pow2(13));
  }
  uint32_t addr = 41120;  // 1010 0000 1010 0000 // 10 0 10 0 // 10 * 16^3 + 10 * 16^1 // 41120
  uint32_t index = get_index(addr, icacheSets); // should be 0100 0001 // 4 1 // 65
  uint32_t tag = get_tag(addr, icacheSets); // should be 10
  if (index != 65 || tag != 2) {
    printf("\tError in Test: index of 41120 should be 65, and we get %u\n", index);
    printf("\tError in Test: tag of 41120 should be 2, and we get %u\n", tag);
  }

  uint32_t addr2 = 3490043330; // 110100000000010111 0101011 1000010
  uint32_t index2 = get_index(addr2, icacheSets); // should be 010 1011 // 2 * 16 + 11 // 43
  uint32_t tag2 = get_tag(addr2, icacheSets); // should be 110100000000010111 // 213015
  if (index2 != 43 || tag2 != 213015) {
    printf("\tError in Test: index of 3490043330 should be 43, and we get %u\n", index2);
    printf("\tError in Test: tag of 3490043330 should be 213015, and we get %u\n", tag2);
  }
  
  
}

// Initialize the Cache Hierarchy
//
void
init_cache()
{
  // Initialize cache stats
  icacheRefs        = 0;
  icacheMisses      = 0;
  icachePenalties   = 0;
  dcacheRefs        = 0;
  dcacheMisses      = 0;
  dcachePenalties   = 0;
  l2cacheRefs       = 0;
  l2cacheMisses     = 0;
  l2cachePenalties  = 0;
  
  //
  //TODO: Initialize Cache Simulator Data Structures
  //

  // test my utilities
  #ifdef DEBUG
  test_my_util();
  #endif // DEBUG

  // if I-cache exists, initialize it
  if (icacheSets) {
    icacheObj.sets = (icache_set*)malloc(icacheSets * sizeof(icache_set));
    for (int i = 0; i < icacheSets; i++) {
      icacheObj.sets[i].blocks = (icache_block*)malloc(icacheAssoc * sizeof(icache_block));
    }
    #ifdef DEBUG
    printf("\tLog: Init state: I-Cache is initialized\n");
    #endif
  }

  // if D-cache exists, initialize it
  if (dcacheSets) {
    dcacheObj.sets = (icache_set*)malloc(dcacheSets * sizeof(icache_set));
    for (int i = 0; i < dcacheSets; i++) {
      dcacheObj.sets[i].blocks = (icache_block*)malloc(dcacheAssoc * sizeof(icache_block));
    }
    #ifdef DEBUG
    printf("\tLog: Init state: D-Cache is initialized\n");
    #endif
  }

  // if l2-cache exists, initialize it
  if (l2cacheSets) {
    l2cacheObj.sets = (icache_set*)malloc(l2cacheSets * sizeof(icache_set));
    for (int i = 0; i < l2cacheSets; i++) {
      l2cacheObj.sets[i].blocks = (icache_block*)malloc(l2cacheAssoc * sizeof(icache_block));
    }
    #ifdef DEBUG
    printf("\tLog: Init state: L2-Cache is initialized\n");
    #endif
  }
  
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{

  // if icache doesn't exist, go to l2cache
  if (icacheSets == 0) {
    return l2cache_access(addr);
  }

  // if icache exist, use index to find the set
  icacheRefs++;

  uint32_t index = get_index(addr, icacheSets);
  uint32_t tag = get_tag(addr, icacheSets);
  icache_set set = icacheObj.sets[index];

  // for each block in the set, see if any hits
  // also records who has the highest lru
  uint32_t highest_lru = 0;
  int id_highest_lru = 0;

  for (int i = 0; i < icacheAssoc; i++) {
    if (set.blocks[i].lru > highest_lru) {
      highest_lru = set.blocks[i].lru;
      id_highest_lru = i;
    }

    if (set.blocks[i].tag == tag && set.blocks[i].valid == 1) {
      // hit. This block's lru set to 0, all others ++
      set.blocks[i].lru = 0;
      lru_increase_except(set, icacheAssoc, i);
      return icacheHitTime;
    }
  }

  // no hit
  icacheMisses++;

  uint32_t this_panelty = l2cache_access(addr);
  icachePenalties += this_panelty;

  // replace the block with the highest lru
  set_overwrite_block(set, id_highest_lru, icacheAssoc, tag);

  return this_panelty + icacheHitTime;
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  // if dcache doesn't exist, go to l2cache
  if (dcacheSets == 0) {
    return l2cache_access(addr);
  }

  // if dcache exist, use index to find the set
  dcacheRefs++;

  uint32_t index = get_index(addr, dcacheSets);
  uint32_t tag = get_tag(addr, dcacheSets);
  icache_set set = dcacheObj.sets[index];

  // for each block in the set, see if any hits
  // also records who has the highest lru
  uint32_t highest_lru = 0;
  int id_highest_lru = 0;

  for (int i = 0; i < dcacheAssoc; i++) {
    if (set.blocks[i].lru > highest_lru) {
      highest_lru = set.blocks[i].lru;
      id_highest_lru = i;
    }

    if (set.blocks[i].tag == tag && set.blocks[i].valid == 1) {
      // hit. This block's lru set to 0, all others ++
      set.blocks[i].lru = 0;
      lru_increase_except(set, dcacheAssoc, i);
      return dcacheHitTime;
    }
  }

  // no hit
  dcacheMisses++;

  uint32_t this_panelty = l2cache_access(addr);
  dcachePenalties += this_panelty;

  // replace the block with the highest lru
  set_overwrite_block(set, id_highest_lru, dcacheAssoc, tag);

  return this_panelty + dcacheHitTime;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  // if l2cache doesn't exist, return the time of main memory
  if (l2cacheSets == 0) {
    return memspeed;
  }

  // if l2cache exist, use index to find the set
  l2cacheRefs++;

  uint32_t index = get_index(addr, l2cacheSets);
  uint32_t tag = get_tag(addr, l2cacheSets);
  icache_set set = l2cacheObj.sets[index];

  // for each block in the set, see if any hits
  // also records who has the highest lru
  uint32_t highest_lru = 0;
  int id_highest_lru = 0;

  for (int i = 0; i < l2cacheAssoc; i++) {
    if (set.blocks[i].lru > highest_lru) {
      highest_lru = set.blocks[i].lru;
      id_highest_lru = i;
    }

    if (set.blocks[i].tag == tag && set.blocks[i].valid == 1) {
      // hit. This block's lru set to 0, all others ++
      set.blocks[i].lru = 0;
      lru_increase_except(set, l2cacheAssoc, i);
      return l2cacheHitTime;
    }
  }

  // no hit
  l2cacheMisses++;

  uint32_t this_panelty = memspeed;
  l2cachePenalties += this_panelty;

  // replace the block with the highest lru
  set_overwrite_block(set, id_highest_lru, l2cacheAssoc, tag);

  return this_panelty + l2cacheHitTime;
}
