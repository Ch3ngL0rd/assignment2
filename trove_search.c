#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "global.h"
#include "hashtable.h"

void add_words(FILE *f, int min_len, char *path);
void scan_file(char *abs_path, int min_len);

// Recursively searches through a directory and scans files
void dir_search(char *basePath, int min_len)
{
    char path[PATH_MAX];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
    {
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            // Path is equal to directory/file_name
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            struct stat st;

            if (stat(path, &st) == 0)
            {
                if (S_ISDIR(st.st_mode))
                {
                    dir_search(path, min_len);
                }
                else
                {
                    scan_file(path, min_len);
                }
            }
        }
    }
    closedir(dir);
}

// Scans through a file and adds words to hashtable
void scan_file(char *abs_path, int min_len)
{
    FILE *f = fopen(abs_path, "rb");
    add_words(f, min_len, abs_path); // Searches through the file for all words >= min_len
    fclose(f);
}

void add_words(FILE *f, int min_len, char *path)
{
    int character;   // integer representation of character we will store in the file
    char word[1000]; // where we store the character each iteration - currently hardcoded at the moment

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
            ht_set(ht, word, path); // Adds values to the hashtable
        }
    }
}

void is_dir(char *base_path, int min_len)
{
    char buffer[PATH_MAX];
    char *res = realpath(base_path, buffer);

    if (res)
    {
        struct stat st;
        stat(buffer, &st);
        DIR *dir = opendir(buffer);

        if (S_ISDIR(st.st_mode))
        {
            closedir(dir);
            dir_search(buffer, min_len);
        }
        else
        {
            scan_file(buffer, min_len);
        }
    }
}