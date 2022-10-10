// Insert
// Read
// Sort
// Create

#ifndef linked_header
#define linked_header

struct LinkedList;
void insert(struct LinkedList* list,char* word);
char* read_next(struct LinkedList* list);
struct LinkedList* create_linked_list();
void sort(struct LinkedList* list);

#endif
