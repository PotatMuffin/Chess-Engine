#ifndef TRANSPOSITIONTABLE
#define TRANSPOSITIONTABLE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "eval.h"

typedef enum {
    PV = 0,
    CUT = 1,
    ALL = 2
} NodeType;

typedef struct {
    uint64_t key;
    int32_t depth;
    int32_t rootDistance;
    int32_t value;
    uint16_t type;
    Move move;
} Transposition;

typedef struct {
    size_t capacity;
    Transposition *items;
} TranspositionTable;

void InitTranspositionTable(TranspositionTable *table, size_t capacity)
{
    table->capacity = capacity;
    table->items = calloc(capacity, sizeof(Transposition));
    if(table->items == NULL) 
    {
        fprintf(stderr, "Couldn't allocate Transposition table\nGet more ram xD\n");
        exit(1);
    }
}

void ClearTranspositionTable(TranspositionTable *table)
{
    memset(table->items, 0, table->capacity*sizeof(Transposition));
}

void AddTransposition(TranspositionTable *table, int depth, int rootDistance, uint64_t key, Move move, int value, NodeType type)
{
    uint64_t index = key % table->capacity;
    // Transposition *entry = &table->items[index]; 
    // if(entry->key != 0 && key != entry->key) printf("collisions :(\n");
    table->items[index] = (Transposition){ .key = key, .depth = depth, .rootDistance = rootDistance, .value = value, .type = type, .move = move };
}

#define sign(x) ((x) > 0) - ((x) < 0)
Transposition *GetTranspositionEntry(TranspositionTable *table, int depth, int rootDistance, uint64_t key)
{
    uint64_t index = key % table->capacity;
    Transposition *entry = &table->items[index];
    if(entry->key == 0) return NULL; 
    int s = sign(entry->value);
    if(s*entry->value >= (mateScore-200)) entry->value = s*(mateScore - rootDistance);
    if(entry->key == key && entry->depth >= depth) return entry;
    else return NULL;
}

#endif