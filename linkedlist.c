#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Node Node;
typedef struct LinkedList LinkedList;

Node *create_node(char *word);

struct Node
{
    char *word;
    Node *next;
};

struct LinkedList
{
    Node *current;
    Node *head;
};
// Creates a linked_list
LinkedList *create_linked_list()
{
    LinkedList *list = malloc(sizeof(LinkedList));
    list->current = NULL;
    list->head = NULL;
    return list;
}

// Inserts a word into the linkedlist if not in
// O(N) time lol
void insert(LinkedList *list, char *word)
{
    // If empty
    if (list->head == NULL)
    {
        list->head = create_node(word);
        list->current = list->head;
        return;
    }
    // Check for matching values
    Node *traversal = list->head;
    while (traversal != NULL)
    {
        if (strcmp(traversal->word, word) == 0) return;
        traversal = traversal->next;
    }
    // We do not have the same word
    Node *new = create_node(word);
    list->current->next = new;
    list->current = new;
}

// Consumes the linkedlist
char *read_next(LinkedList *list)
{
    if (list->head == NULL)
    {
        return NULL;
    }

    Node *current_head = list->head;
    char *word = strdup(current_head->word);
    list->head = current_head->next;
    free(current_head);
    return word;
}

void sort(LinkedList *list)
{
    return;
}

Node *create_node(char *word)
{
    Node *node = malloc(sizeof(Node));
    node->word = word;
    node->next = NULL;
    return node;
}