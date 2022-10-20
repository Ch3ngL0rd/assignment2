#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "hashtable.h"
#include "global.h"

void build_trove(struct ht_t *hashtable, char *basePath)
{
    char path[PATH_MAX];

    struct stat st;
    stat(path, &st);

    FILE *f = fopen(path, "wb");
    if (f == NULL)
    {
        printf("Error opening trove file.\n");
        exit(EXIT_FAILURE);
    }

    // Go through hashtable and add entries to trove file
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        entry_t *entry = hashtable->entries[i];
        if (entry == NULL)
        {
            continue;
        }
        while (1)
        {
            fprintf(f, "%s\\%s\n", entry->key, entry->value);
            if (entry->next == NULL)
            {
                break;
            }
            entry = entry->next;
        }
    }
    fclose(f);
}

// Creates a temp file that copies the undeleted file paths
void delete_path(char *filename, char *path)
{
    FILE *filein = fopen(filename, "r");
    FILE *fileout = fopen("temp", "w");

    char abs_path[PATH_MAX];

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

    FILE *troveptr = fopen(abs_trove, "r+");
    FILE *updateptr = fopen(abs_update, "r");
    FILE *temptrove = fopen("temporarytrovefile", "w");
    // NEED TO DO A CHECK FOR FAILURE TO OPEN

    char *line_update = NULL;
    size_t len_update = 0;
    ssize_t read_update;

    while ((read_update = getline(&line_update, &len_update, updateptr)) != -1)
    {
        char new_line[100000];
        // Retrieve the first word from the line
        char *token_update = strtok(line_update, "\\");

        // Need a flag to check if word is not found??

        char *line_trove = NULL;
        size_t len_trove = 0;
        ssize_t read_trove;
        // Read lines of trove file to see if word exists
        while ((read_trove = getline(&line_trove, &len_trove, troveptr)) != -1)
        {
            // Retrieve first word from the line of trove file
            char *token_trove = strtok(line_trove, "\\");
            memcpy(new_line, line_trove, len_trove);
            new_line[len_trove-1] = '\0';
            // If first word of trove file == first word of update file....
            // while (token_trove != NULL)
            // {
            //     printf("%s\n", token_trove);
            //     token_trove = strtok(NULL, "\\");
            // }
            if (strcmp(token_trove, token_update) == 0)
            {
                // look for first backslash - start of the paths
                int index = 0;
                while (index < len_trove)
                {
                    if (line_trove[index] == '\\')
                    {
                        break;
                    }
                    index++;
                }
                index++;
                // Now we are located at the first character of the string
                int current_index = index;
                while (current_index < len_trove)
                {
                    // Checks for last path of the list
                    if (current_index == len_trove - 1)
                    {
                        int length = current_index - index;
                        char path_name[length];
                        strncpy(path_name, line_trove + index, length);
                        if (strstr(line_trove, path_name))
                        {
                            strcat(new_line, "\\");
                            strcat(new_line, path_name);
                        }
                        index = current_index;
                    }
                    else if (line_trove[current_index] == '\\')
                    {
                        // Double check my math? - minus one since actual character
                        int length = current_index - index - 1;
                        char path_name[length];
                        strncpy(path_name, line_trove + index, length);
                        if (strstr(line_trove, path_name))
                        {
                            strcat(new_line, "\\");
                            strcat(new_line, path_name);
                        }
                        index = current_index;
                    }
                    current_index++;
                }
            }

            // if (strcmp(token_trove, token_update) == 0)
            // {
            //     // Scan through the absolute paths and add them to the end of the trove file
            //     // May need to remove the \n character using similar function to below
            //     // get next absolute path
            //     printf("%s\n", token_update);
            //     token_update = strtok(NULL, "\\");
            //     while (token_update != NULL)
            //     {
            //         // See if absolute path already occurs in the line_trove
            //         printf("%s\n", token_update);
            //         token_update = strtok(NULL, "\\");
            //     }
            // }
            // else
            // {
            //     // Word not the same
            //     continue;
            // }
        }
        rewind(troveptr);
        strcat(new_line,"\n");
        fputs(new_line, temptrove);
    }
    fclose(temptrove);
    fclose(updateptr);
    fclose(troveptr);
    remove(abs_trove);
    rename("temporarytrovefile",abs_trove);
    printf("Trove path: %s\t Update_path: %s\n", abs_trove, abs_update);
}