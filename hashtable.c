#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "hashtable.h"
// typedef struct entry_t
// {
//     char *key;
//     char *value;
//     struct entry_t *next;
// } entry_t;

// typedef struct
// {
//     entry_t **entries;
// } ht_t;

// typedef struct ht_t ht_t;


// Hash function for string
unsigned int hash(const char *key)
{
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < key_len; ++i)
    {
        value = value * 37 + key[i];
    }

    // make sure value is 0 <= value < TABLE_SIZE
    value = value % TABLE_SIZE;

    return value;
}

// Creates an entry for hashtable
entry_t *ht_pair(const char *key, const char *value)
{
    // Allocate memory for struct & struct values
    entry_t *entry = malloc(sizeof(entry_t));
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1); // THIS NEEDS TO BE CHANGED

    // Assign values
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // next starts out NULL but may be set later on
    entry->next = NULL;
    return entry;
}

// Creates hashtable
ht_t *ht_create(void)
{
    // Allocate memory
    ht_t *hashtable = malloc(sizeof(ht_t));
    hashtable->entries = malloc(sizeof(entry_t *) * TABLE_SIZE);

    // Set each to null
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

// Sets a key-value pair for a hashtable
// Hash collision is resolved by linear probing
void ht_set(ht_t *hashtable, const char *key, const char *value)
{
    unsigned int slot = hash(key);

    entry_t *entry = hashtable->entries[slot];

    // Assign if slot is empty
    // Returns if hashtable contains string
    if (entry == NULL)
    {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }
    else if (strstr(entry->value, value))
    {
        return;
    }

    entry_t *prev;

    // Linear probe until null entry is found
    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            char *temp;
            size_t newlen;

            newlen = strlen(entry->value) + strlen(value) + 2;
            temp = (char *)malloc(newlen);

            strcpy(temp, entry->value);
            strcat(temp, " ");
            strcat(temp, value);

            entry->value = realloc(entry->value, newlen);
            strcpy(entry->value, temp);
            return;
        }
        prev = entry;
        entry = prev->next;
    }

    prev->next = ht_pair(key,value);
}

char *ht_get(ht_t *hashtable, const char *key) {
    unsigned int slot = hash(key);

    entry_t *entry = hashtable->entries[slot];

    // If key is empty, hashtable does not contain key
    if (entry == NULL) {
        return NULL;
    }

    // Linear probe until key is found
    while (entry != NULL) {
        if (strcmp(entry->key,key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    // Return NULL if key not found
    return NULL;
}