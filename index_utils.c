#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

void to_lowercase(char *word)
{
    for (int i = 0; word[i]; i++)
    {
        word[i] = tolower(word[i]);
    }
}

bool valid_word(char *word, int word_length, int min_length)
{
    if (word_length < min_length)
    {
        return false;
    }
    for (int i = 0; i < word_length; i++)
    {
        if (isalpha(word[i]) == 0)
        {
            printf("Bad letter: %c %d, index %d\n",word[i],word[i],i);
            return false;
        }
    }
    return true;
}