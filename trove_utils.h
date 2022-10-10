#ifndef trove_header
#define trove_header

char** find_files(char* directory_name);
bool index_file(char* file_name, char* trove_file, int min_length);
void remove_index(char* file_name, char* trove_file);

#endif
