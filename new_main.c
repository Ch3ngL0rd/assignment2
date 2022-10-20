#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    // int opt;

    if (argc < 2)
    {
        printf("\nUsage: ./trove [-f filename] [-b | -r | -u] [-l length] filelist\nor     ./trove [-f filename] word\n\nwhere options are:\n-b		build a new trove-file\n-f filename	provide the name of the trove-file to be built or searched\n-l length	specify the minimum-length of words added to the trove-file\n-r		remove information from the trove-file\n-u		update information in the trove-file\n\n");
        exit(EXIT_FAILURE);
    }

    char *filename = (char *)malloc(strlen("trove") + 1);
    strcpy(filename, "trove");

    ht = ht_create();
    return EXIT_SUCCESS;
}