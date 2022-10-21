#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "hashtable.h"

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
entry_t *ht_pair(const char *word, const char *path)
{
    // Allocate memory for struct & struct values
    entry_t *entry = malloc(sizeof(entry_t));
    entry->word = malloc(strlen(word) + 1);
    entry->path = malloc(strlen(path) + 1); // THIS NEEDS TO BE CHANGED

    // Assign values
    strcpy(entry->word, word);
    strcpy(entry->path, path);

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
void ht_set(ht_t *hashtable, const char *word, const char *path)
{
    unsigned int slot = hash(word);

    entry_t *entry = hashtable->entries[slot];

    // Assign if slot is empty
    // Returns if hashtable contains string
    if (entry == NULL)
    {
        hashtable->entries[slot] = ht_pair(word, path);
        return;
    }
    else if (strstr(entry->path, path))
    {
        return;
    }

    entry_t *prev;

    // Linear probe until null entry is found
    while (entry != NULL)
    {
        if (strcmp(entry->word, word) == 0)
        {
            char *temp;
            size_t newlen;

            newlen = strlen(entry->path) + strlen(path) + 2;
            temp = (char *)malloc(newlen);

            strcpy(temp, entry->path);
            strcat(temp, " ");
            strcat(temp, path);

            entry->path = realloc(entry->path, newlen);
            strcpy(entry->path, temp);
            return;
        }
        prev = entry;
        entry = prev->next;
    }

    prev->next = ht_pair(word,path);
}

char *ht_get(ht_t *hashtable, const char *word) {
    unsigned int slot = hash(word);

    entry_t *entry = hashtable->entries[slot];

    // If key is empty, hashtable does not contain key
    if (entry == NULL) {
        return NULL;
    }

    // Linear probe until key is found
    while (entry != NULL) {
        if (strcmp(entry->word,word) == 0) {
            return entry->path;
        }
        entry = entry->next;
    }

    // Return NULL if key not found
    return NULL;
}