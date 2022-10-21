trove: main.c hashtable.h trove_search.h trove_utilities.h
	gcc -o trove main.c hashtable.c trove_search.c trove_utilities.c -std=c11 -Wall -Werror