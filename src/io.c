#include "io.h"
#include "utils.h"

#include <stdlib.h>

char* readUntil(FILE* src, char stop)
{
    size_t len = 0;
    char *str = NULL;
    int readChar;

    while((readChar = getc(src)) != EOF && readChar != stop) {
        str = realloc(src, len + 2);
        FATAL(str, 1);
        str[len++] = readChar;
    }

    if(str) str[len] = 0;
    return str;
}