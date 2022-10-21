#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

// #include <sys/types.h>
// #include <stdbool.h>
// #include <unistd.h>
// #include <limits.h>

#include "hashtable.h"
#include "trove_search.h"
#include "trove_utilities.h"

#define DEFINE_HASHTABLE
#include "global.h"

#define MINIMUM_ARGUMENTS 4

int main(int argc, char *argv[])
{
    // Used for getopt() function
    int opt;

    // The default minimum length when -l is not passed into command line
    int min_len = 4;

    // Dynamically allocating space for the filename incase this changes via a commandline input
    char *filename = (char *)malloc(strlen("trove") + 1);

    // Check to see if no arguments have been passed in and returns a statement
    // prompting the user how to use ./trove
    if (argc < 2)
    {
        printf("\nUsage: ./trove [-f filename] [-b | -r | -u] [-l length] filelist\nor     ./trove [-f filename] word\n\nwhere options are:\n-b		build a new trove-file\n-f filename	provide the name of the trove-file to be built or searched\n-l length	specify the minimum-length of words added to the trove-file\n-r		remove information from the trove-file\n-u		update information in the trove-file\n\n");
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "f:l:r:u:b:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            // Reallocate the memory to = the length of the filename passed in + 1 for the null byte
            filename = (char *)realloc(filename, strlen(optarg) + 1);
            strcpy(filename, optarg);
            break;
        case 'l':
            // Get the length from the arguments
            // MAY NEED TO ADD A CHECK IN TO MAKE SURE MIN LENGTH IS >= 1
            min_len = atoi(optarg);
            if (min_len < 1)
            {
                printf("Must provide a positive integer for -l argument!");
                return EXIT_FAILURE;
            }
            break;
        case 'r':
            // Checking if first argument is a directory or not
            // inside of is_dir, if it is, then we traverse through the dictionary and delete
            // printf("%s\n", optarg);
            delete_path(filename, optarg);
            for (; optind < argc; optind++)
            {
                delete_path(filename, argv[optind]);
            }
            free(filename);
            exit(EXIT_SUCCESS);
            break;
        case 'u':
            // Create the hashtable
            ht = ht_create();

            // Add to the hashtable
            is_dir(optarg, min_len);
            for (; optind < argc; optind++)
            {
                is_dir(argv[optind], min_len);
            }
            // Build the temp trove file
            build_trove(ht, "temp-update");

            if (strlen(filename) == 0)
            {
                update_trove("/tmp/trove", "temp-update");
            }
            else
            {
                update_trove(filename, "temp-update");
            }

            free(filename);
            exit(EXIT_SUCCESS);
            break;
        case 'b':
            // Retrieve the first argument for filename from list of files
            ht = ht_create();
            is_dir(optarg, min_len);
            for (; optind < argc; optind++)
            {
                is_dir(argv[optind], min_len);
            }
            // Trove file handler
            if (strlen(filename) == 0)
            {
                printf("The trove-file name to be built or searched is: trove\n");
                // Go into the tmp directory
                // save file as trove
                build_trove(ht, "/tmp/trove");
            }
            else
            {
                char *path;
                if ((path = malloc(strlen("./") + strlen(filename) + 1)) != NULL)
                {
                    path[0] = '\0';
                    strcat(path, "./");
                    strcat(path, filename);
                    printf("Path is %s\n",path);
                    build_trove(ht, path);
                }
                else
                {
                    exit(EXIT_FAILURE);
                }
                free(path);
            }
            free(filename);
            exit(EXIT_SUCCESS);
            break;
        case '?':
            printf("Invalid argument!\n");
            exit(EXIT_FAILURE);
            break;
        default:
            printf("ERROR!!\n");
            exit(EXIT_FAILURE);
            break;
        }
        // If it makes it to this point we have just -f filename [arg] where arg will be the word
        search_word(filename, argv[argc - 1]);
    }

    return 0;
}