#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

#define TABLE_SIZE 10000

typedef struct value_t
{
    char *value;
    struct value_t *next;
} value_t; // COME BACK TO THIS MAY BE ABLE TO TURN VALUE INTO A LINKED LIST

typedef struct entry_t
{
    char *key;
    char *value; // Need to change this to include 1. # of paths containing the word, and 2. Continuous string containing the values separated by
    struct entry_t *next;
} entry_t;

typedef struct
{
    entry_t **entries;
} ht_t;

ht_t *ht;

void *memmem(const void *haystack, size_t hlen, const void *needle, size_t nlen)
{
    int needle_first;
    const void *p = haystack;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = *(unsigned char *)needle;

    while (plen >= nlen && (p = memchr(p, needle_first, plen - nlen + 1)))
    {
        if (!memcmp(p, needle, nlen))
            return (void *)p;

        p++;
        plen = hlen - (p - haystack);
    }

    return NULL;
}

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

entry_t *ht_pair(const char *key, const char *value)
{
    // allocate the entry
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1); // THIS NEEDS TO BE CHANGED

    // copy the key and value in place
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // next starts out null but may be set later on
    entry->next = NULL;

    return entry;
}

ht_t *ht_create(void)
{
    // allocate table
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t *) * TABLE_SIZE);

    // set each to null (needed for proper operation)
    int i = 0;
    for (; i < TABLE_SIZE; ++i)
    {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t *hashtable, const char *key, const char *value)
{
    unsigned int slot = hash(key);

    // try to look up an entry set
    entry_t *entry = hashtable->entries[slot];

    // no entry means slot empty, insert immediately
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

    // walk through each entry until either the end is
    // reached or a matching key is found
    while (entry != NULL)
    {
        // check key
        if (strcmp(entry->key, key) == 0)
        {
            // match found, replace value
            // NEED TO CHANGE THIS INSTEAD OF REPLACING THE VALUE - REALLOCATE THE SPACE
            // AND ADD TO ARRAY
            // free(entry->value);
            // entry->value = malloc(strlen(value) + 1);
            // strcpy(entry->value, value);
            // return;

            // Checks if a word is found in the file - if it is currently just prints the
            // file name and the absolute path to the file
            // will be able to alter this after and change it to check the hashmap
            // NOTE: Need to check if I need to include the sizeof(char) or not

            char *temp;
            size_t newlen;

            newlen = strlen(entry->value) + strlen(value) + 2; // Adding + 2 to take into account for the space and the new null byte
            temp = (char *)malloc(newlen);

            strcpy(temp, entry->value);
            strcat(temp, " ");
            strcat(temp, value);

            // free(entry->value);

            entry->value = realloc(entry->value, newlen);
            strcpy(entry->value, temp);
            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = ht_pair(key, value);
}

char *ht_get(ht_t *hashtable, const char *key)
{
    unsigned int slot = hash(key);

    // try to find a valid slot
    entry_t *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL)
    {
        return NULL;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL)
    {
        // return value if found
        if (strcmp(entry->key, key) == 0)
        {
            return entry->value;
        }

        // proceed to next key if available
        entry = entry->next;
    }

    // reaching here means there were >= 1 entries but no key match
    return NULL;
}

void ht_del(ht_t *hashtable, const char *key)
{
    unsigned int bucket = hash(key);

    // try to find a valid bucket
    entry_t *entry = hashtable->entries[bucket];

    // no bucket means no entry
    if (entry == NULL)
    {
        return;
    }

    entry_t *prev;
    int idx = 0;

    // walk through each entry until either the end is reached or a matching key is found
    while (entry != NULL)
    {
        // check key
        if (strcmp(entry->key, key) == 0)
        {
            // first item and no next entry
            if (entry->next == NULL && idx == 0)
            {
                hashtable->entries[bucket] = NULL;
            }

            // first item with a next entry
            if (entry->next != NULL && idx == 0)
            {
                hashtable->entries[bucket] = entry->next;
            }

            // last item
            if (entry->next == NULL && idx != 0)
            {
                prev->next = NULL;
            }

            // middle item
            if (entry->next != NULL && idx != 0)
            {
                prev->next = entry->next;
            }

            // free the deleted entry
            free(entry->key);
            free(entry->value);
            free(entry);

            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;

        ++idx;
    }
}

void ht_dump(ht_t *hashtable)
{
    for (int i = 0; i < TABLE_SIZE; ++i)
    {
        entry_t *entry = hashtable->entries[i];

        if (entry == NULL)
        {
            continue;
        }

        printf("slot[%4d]: ", i);

        for (;;)
        {
            printf("%s=%s ", entry->key, entry->value);

            if (entry->next == NULL)
            {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}

void add_words(FILE *f, int min_len, char *path)
{
    int character;   // integer representation of character we will store in the file
    char word[1000]; // where we store the character each iteration

    while (!feof(f))
    {
        int index = 0;
        while ((character = fgetc(f)) != EOF && isalnum(character))
        {
            word[index] = character;
            index++;
        }
        word[index] = '\0';

        // If the word is greater than or = to the min length then we add it to the hashmap
        if (strlen(word) >= min_len)
        {
            ht_set(ht, word, path);
        }
    }
}

void myfilerecursive(char *basePath)
{
    char path[1000]; // Need to make this dynamic somehow!
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            // Check if this is a file or a directory - this is where my program was
            // breaking last time

            FILE *f = fopen(path, "rb"); // File pointer to the start of path

            // Adding values to the stat struct so we can retrieve the file contents
            struct stat sb;
            if (stat(path, &sb) == -1)
            {
                perror("stat");
                exit(EXIT_FAILURE);
            }
            else if (S_ISDIR(sb.st_mode))
            {
                // Checks if a folder - if it is we continue as we only need to scan
                // file contents
                fclose(f);
                myfilerecursive(path);
            }
            else
            {
                add_words(f, 4, path);
                fclose(f);
                myfilerecursive(path);
            }

            // char *file_contents = malloc(sb.st_size); // Setting the size of the file to the size
            // fread(file_contents, sb.st_size, 1, f);   // Adding contents to file_contents

            // // Checks if a word is found in the file - if it is currently just prints the
            // // file name and the absolute path to the file
            // // will be able to alter this after and change it to check the hashmap
            // // NOTE: Need to check if I need to include the sizeof(char) or not
            // // if (memmem(file_contents, sb.st_size * sizeof(char), "while", strlen("while")))
            // // {
            // //     printf("DIRECTORY/FILE NAME: %-35s ABSOLUTE PATH NAME: %s\n", dp->d_name, path);
            // // }
            // // printf("DIRECTORY/FILE NAME: %-35s ABSOLUTE PATH NAME: %s\n", dp->d_name, path);

            // free(file_contents);
        }
    }

    closedir(dir);
}

void build_trove(ht_t *hashtable)
{
    FILE *f = fopen("trove-file.txt", "wb");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    for (int i = 0; i < TABLE_SIZE; ++i)
    {
        entry_t *entry = hashtable->entries[i];

        if (entry == NULL)
        {
            continue;
        }

        for (;;)
        {
            // printf("%s=%s ", entry->key, entry->value);

            fprintf(f, "%s\n", entry->key);
            fprintf(f, "%s\n", entry->value);

            if (entry->next == NULL)
            {
                break;
            }

            entry = entry->next;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[])
{

    // --------------- CREATION OF HASHMAP AND INSERTION OF RANDOM VALS ------------- //
    ht = ht_create();

    // ht_set(ht, "name1", "em");
    // ht_set(ht, "name2", "russian");
    // ht_set(ht, "name3", "pizza");
    // ht_set(ht, "name4", "doge");
    // ht_set(ht, "name5", "pyro");
    // ht_set(ht, "name6", "joost");
    // ht_set(ht, "name7", "kalix");
    // ht_set(ht, "name7", "fdgfdgd");

    //-- -- -- -- -- -- -RECURSIVE FILE SEARCH PROMPT 1 -- -- -- -- -- -- -- - //
    myfilerecursive(argv[1]);
    ht_dump(ht);
    // Proves that works for collisions!
    // 1. Set table number to low number and find print statement from ht_dump where there
    // is a collision
    // 2. Use the ht_get function - this will show we can still retrieve the value as it
    // is set up as a linkedlist
    printf("%s\n", ht_get(ht, "DECLARE"));
    printf("%s\n", ht_get(ht, "Instead")); // these two have the same hash value - check SLOT 9348

    build_trove(ht);

    return 0;
}
