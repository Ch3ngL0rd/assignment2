#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "trove_utils.h"
#include "index_utils.h"
#include "linkedlist.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Please input more arguments\n");
        return EXIT_FAILURE;
    }
    index_file(argv[1],argv[1],4);
    return EXIT_SUCCESS;
}