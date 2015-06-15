/* Grupo: 2
 Ana Caroline Fernandes Spengler 8532356
 Paulo Bardes                    8531932
 Renato Goto                     8516692
*/

#include "io.h"
#include "utils.h"

#include <stdlib.h>

char* readUntil(FILE* src, int stop)
{
    size_t len = 0;
    char *str = NULL;
    int readChar;

    while((readChar = getc(src)) != EOF && readChar != stop) {
        str = realloc(str, len + 2);
        FATAL(str, 1);
        str[len++] = readChar;
    }

    if(str) str[len] = 0;
    return str;
}