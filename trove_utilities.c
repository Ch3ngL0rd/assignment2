#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "hashtable.h"
#include "global.h"

void build_trove(ht_t *hashtable, char *basePath)
{

    char path[PATH_MAX];
    realpath(basePath, path);
    // open directory
    struct stat st;
    stat(path, &st);

    FILE *f = fopen(path, "wb");
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

        while (true)
        {

            fprintf(f, "%s\\%s\n", entry->word, entry->path);
            // fprintf(f, "%s\n", );

            if (entry->next == NULL)
            {
                break;
            }

            entry = entry->next;
        }
    }
    fclose(f);
}

// Remove absolute path from the line
// Append to new file
// Delete old file
// Copy name to new file
// NOTE - THIS ASSUMPTION IS THAT THE FILE WILL BE IN THE SAME DIRECTORY AS ./trove EXECUTABLE
// ^^ this makes sense because we are passing into the command line one single file not an absolute path to the file
// via the -f flag
void delete_path(char *filename, char *path)
{
    FILE *filein = fopen(filename, "r"); // file we are searching line by line
    FILE *fileout = fopen("temp", "w");  // file we are writing to
    char abs_path[PATH_MAX];
    realpath(path, abs_path);

    // Concatenated path with "\"
    char *concat_path = (char *)malloc(strlen(abs_path) + 2);
    strcat(concat_path, "\\");
    strcat(concat_path, abs_path);

    if (filein == NULL || fileout == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Debug seeing if it drops any words from file
    int countin = 0;
    int countout = 0;

    while ((read = getline(&line, &len, filein)) != -1)
    {

        char *token;
        token = strtok(line, "\\");
        char new_line[100000]; // Dynamic won't work for some reason...
        strcpy(new_line, token);

        // Get the absolute path value and then enter while loop
        token = strtok(NULL, "\\");
        countin++;
        while (token != NULL)
        {
            if (!(strstr(token, abs_path)))
            {
                strcat(new_line, "\\");
                strcat(new_line, token);
            }
            token = strtok(NULL, "\\");
        }

        if (strstr(new_line, "\\"))
        {
            countout++;
            // Add string to file out
            fprintf(fileout, "%s", new_line);
        }
    }

    printf("Number of lines file in: %-30d Number of lines file out: %d\n", countin, countout);

    // Freeing memory and closing files
    free(concat_path);
    // free(new_line);
    if (line)
        free(line);
    fclose(filein);
    fclose(fileout);

    // Delete old file with absolute path
    remove(filename); // do we need a check here?

    // Change temp file to existing filename
    rename("temp", filename);
}

// Search trove file for word
// Prints out the absolute paths corresponding to the word
void search_word(char *filename, char *word)
{
    FILE *f = fopen(filename, "rb");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    while ((read = getline(&line, &len, f)) != -1)
    {
        // printf("Retrieved line of length %zu:\n", read);
        // printf("%s", line);
        line[strcspn(line, "\n")] = 0;
        const char s[2] = "\\";
        char *token;

        // AT THE MOMENT THIS RETURNS THE WORD AS WELL AS THE ABSOLUTE PATHS

        token = strtok(line, s);
        if (strcmp(token, word) == 0)
        {
            token = strtok(NULL, s);
            while (token != NULL)
            {
                printf("%s\n", token);
                token = strtok(NULL, s);
            }
        }
    }

    fclose(f);
    if (line)
        free(line);
    // exit(EXIT_SUCCESS);
}

void update_trove(char *trove_path, char *update_path)
{
    char abs_trove[PATH_MAX];
    char abs_update[PATH_MAX];

    realpath(trove_path, abs_trove);
    realpath(update_path, abs_update);

    FILE *troveptr = fopen(abs_trove, "r+");
    FILE *updateptr = fopen(abs_update, "r");
    FILE *new_file = fopen("new_file", "w");
    // NEED TO DO A CHECK FOR FAILURE TO OPEN

    char *line_update = NULL;
    size_t len_update = 0;
    ssize_t read_update;

    while ((read_update = getline(&line_update, &len_update, updateptr)) != -1)
    {
        // // Retrieve the first word from the line
        // Make a copy of line_trove for the strtok to be used on
        char *tmp1 = (char *)malloc(len_update + 1);
        strcpy(tmp1, line_update);
        char *token_update = strtok(tmp1, "\\");

        bool found = false; // check to see if word is found or not

        char *line_trove = NULL;
        size_t len_trove = 0;
        ssize_t read_trove;
        while ((read_trove = getline(&line_trove, &len_trove, troveptr)) != -1)
        {
            char *tmp2 = (char *)malloc(len_trove + 1);
            strcpy(tmp2, line_trove);
            char *token_trove = strtok(tmp2, "\\");

            if (strcmp(token_trove, token_update) == 0)
            {
                found = true;
                // printf("OLD STRING FOR WORD %s WAS: %s", token_trove, line_trove);
                char buffer[100000]; // Taking into account large file paths - would like to set this dynamically but keep running into issues
                line_update[strcspn(line_update, "\n")] = 0;
                strcpy(buffer, line_update);

                printf("------------------------------------------\n");
                printf("OLD STRING FOR WORD %s IS: %s\n", token_update, line_update);
                // printf("This is the trove line: %s", line_trove);
                // printf("This is the word we are comparing from trove: %s\n", token_trove);
                // printf("This is the word we are comparing from update: %s\n\n", token_update);
                // printf("Successfully found word... scanning through now:\n");

                // Scan through to next absolute path
                // token_trove = strtok(NULL, "\\");
                while (token_trove != NULL)
                {
                    token_trove[strcspn(token_trove, "\n")] = 0;
                    if (!strstr(line_update, token_trove))
                    {
                        token_trove[strcspn(token_trove, "\n")] = 0;
                        printf("\nCOMPARING TO: %s\n", line_update);
                        printf("FOUND PATH NOT CONTAINED IN UPDATE FILE FROM TROVE FILE%s\n\n", token_trove);
                        buffer[strcspn(buffer, "\n")] = 0;
                        strcat(buffer, "\\");
                        strcat(buffer, token_trove); // append absolute path to end of buffer
                    }

                    token_trove = strtok(NULL, "\\");
                }
                printf("NEW STRING FOR WORD %s IS: %s\n", token_update, buffer);
                fprintf(new_file, "%s\n", buffer);
                free(tmp2);
                break; // no need to keep iterating through
            }
            free(tmp2);
        }
        // // Didn't find the word in update folder so append the current line being searched to new file
        if (found == false)
        {
            fprintf(new_file, "%s", line_update);
        }
        free(tmp1);
        rewind(troveptr);
    }
    fclose(new_file);
    new_file = fopen("new_file", "r+"); // opening file in read now

    // Very inefficient but now we need to traverse through the trove file and make sure we have not missed any words
    char *line_trove = NULL;
    size_t len_trove = 0;
    ssize_t read_trove;
    while ((read_trove = getline(&line_trove, &len_trove, troveptr)) != -1)
    {
        char *tmp1 = (char *)malloc(len_trove + 1);
        strcpy(tmp1, line_trove);
        char *token_trove = strtok(tmp1, "\\"); // get the word

        printf("%s\n", token_trove);

        char *line_new = NULL;
        size_t len_new = 0;
        ssize_t read_new;

        bool found = false;

        while ((read_new = getline(&line_new, &len_new, new_file)) != -1)
        {
            char *tmp2 = (char *)malloc(len_new + 1);
            strcpy(tmp2, line_new);
            char *token_new = strtok(tmp2, "\\");

            if (strcmp(token_new, token_trove) == 0)
            {
                found = true;
                break;
            }
        }
        if (found == false)
        {
            fprintf(new_file, "%s", line_trove);
        }
        rewind(new_file);
    }

    fclose(troveptr);
    fclose(updateptr);
    fclose(new_file);

    remove(trove_path);
    remove(update_path);
    rename("new_file", trove_path);
}