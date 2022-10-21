// Source: https://benhoyt.com/writings/hash-table-in-c/    

#pragma once

typedef struct entry_t
{
    char *word;
    char *path;
    struct entry_t *next;
} entry_t;

typedef struct ht_t
{
    entry_t **entries;
} ht_t;

unsigned int hash(const char *word);
entry_t *ht_pair(const char *word, const char *path);
ht_t *ht_create(void);
void ht_set(ht_t *hashtable, const char *word, const char *path);
char *ht_get(ht_t *hashtable, const char *word);