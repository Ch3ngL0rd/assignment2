#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#include "trove_utils.h"
#include "index_utils.h"
#include "linkedlist.h"

void slice(const char *str, char *result, size_t start, size_t end);

// Recursively searches a directory and returns all files
char **find_files(char *directory_name)
{
}

// Indexes file and adds to 'trove_file' (-b)
// returns true if success
bool index_file(char *file_name, char *trove_file, int min_length)
{
    char file_path[PATH_MAX];

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    struct LinkedList *ll = create_linked_list();

    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        printf("Error opening %s\n", file_name);
        return false;
    }

    realpath(file_name, file_path);
    printf("File name is %s\n", file_path);

    // Gets file line by line
    int x = 0;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        x++;
        int current = 0;
        for (int i = 0; i < len; i++)
        {
            if (isalnum(line[i]) == 0)
            {
                if (i - current > min_length)
                {
                    printf("trigger: %c | ", line[i]);
                    char word[i - current - 1];
                    slice(line, word, current + 1, i);
                    if (valid_word(word, i - current - 1, min_length) == true)
                    {
                        to_lowercase(word);
                        printf("adding %s: %d-%d\n", word, current + 1, i);
                        insert(ll, word);
                    }
                }
                current = i;
            }
            else
            {
                // printf("%d %d\n", current, i);
            }
        }
        if (x == 6)
        {
            break;
        }
    }

    fclose(fp);
    if (line)
        free(line);

    return true;
}

// Removes all information from trove-file (-r)
void remove_index(char *file_name, char *trove_file)
{
}

void slice(const char *str, char *result, size_t start, size_t end)
{
    strncpy(result, str + start, end - start);
}