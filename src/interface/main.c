#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "../utils.h"
#include "../database/database.h"
#include "../io.h"

static char readOpt()
{
    char opt = getchar();
    if(opt != '\n') while(getchar() != '\n');

    return tolower(opt);
}

static void printMenu()
{
    CLEAR();
    printf("\n\n      _______       _ _   _            \n");
    printf("     |__   __|     (_) | | |           \n");
    printf("        | |_      ___| |_| |_ ___ _ __ \n");
    printf("        | \\ \\ /\\ / / | __| __/ _ \\ '__|\n");
    printf("        | |\\ V  V /| | |_| ||  __/ |   \n");
    printf("        |_| \\_/\\_/ |_|\\__|\\__\\___|_|   \n");

    printf("\n\n"
        "A - Adicionar Tweet\n"
        "V - Adicionar varios Tweets\n"
        "M - Mostrar todos os Tweets em ordem\n"
        "U - Buscar Tweets por USER\n"
        "f - Buscar Tweets por FAVORITE_COUNT\n"
        "l - Buscar Tweets por LANGUAGE\n"
        "t - Buscar Tweets por FAVORITE_COUNT e LANGUAGE (matching)\n"
        "g - Buscar Tweets por FAVORITE_COUNT ou LANGUAGE (merging)\n"
        "X - Apagar Tweet (FAVORITE_COUNT)\n"
        "Q - Sair\n\n\n"
    );
}

// TODO permitir ir e voltar
static void ShowSeq(TweetSeq s)
{
    for(size_t i = 0; i < s.length; ++i) {
        CLEAR();
        printf("(%zu/%zu)\n", i+1, s.length);
        PrintTweet(s.seq + i);
        readOpt();
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2) {
        printf("Usage: %s <database_name>\n", argv[0]);
        return 1;
    }

    Database *db = CreateDatabase(argv[1]);
    FAIL_MSG(db, 1, "Falha ao criar banco de dados!");

    char exitFlag = 0;
    while(!exitFlag) {
        char *uname;
        Tweet t, *result;
        size_t n;
        char *language;

        printMenu();
        printf("Escolha: ");

        switch(readOpt()) {
            case 'a':
                CLEAR();
                if(InsertTweet(db, ComposeTweet()) != 0)  {
                    CLEAR();
                    printf("Não inseriu tweet!\n");
                } else {
                    CLEAR();
                    printf("Tweet inserido com sucesso!\n");
                }
                readOpt();
            break;

            case 'v':
                printf("Quantos? ");
                scanf("%zu", &n);
                for(size_t i = 0; i < n; ++i) {
                    CLEAR();
                    printf("(%zu/%zu)\n", i+1, n);
                    if(InsertTweet(db, ComposeTweet()) != 0)  {
                        CLEAR();
                        printf("Não inseriu tweet!\n");
                    } else {
                        CLEAR();
                        printf("Tweet inserido com sucesso!\n");
                    }
                    readOpt();
                }
            break;

            case 'm':
                n = GetSize(db);
                if(n == 0) {
                    CLEAR();
                    printf("Banco de dados vazio!\n");
                    readOpt();
                } else {
                    DatabaseItr *itr = GetIterator(db);
                    FATAL_MSG(itr, 1, "Falha ao criar iterador!");
                    for(size_t i = 0; GetNextTweet(itr, &t) == 0; ++i) {
                        CLEAR();
                        printf("(%zu/%zu)\n", i+1, n);
                        PrintTweet(&t);
                        readOpt();
                    }
                    FreeIterator(itr);
                }
            break;

            case 'u':
                printf("USER? "); uname = readUntil(stdin, '\n');
                TweetSeq s = FindByUser(db, uname);
                if(s.seq) {
                    ShowSeq(s);
                } else {
                    CLEAR();
                    printf("Nenhum Tweet com USER \"%s\" encontrado.\n", uname);
                    readOpt();
                }
                free(uname);
                FreeTweetSeq(s);
            break;

            case 'f': /*
                printf("FAVORITE_COUNT? ");
                scanf("%zu", &n);
                CLEAR();
                if(GetTweet(db, n, &t) == 0) {
                    printf("(FAVORITE_COUNT: %zu)\n", n);
                    printTweet(&t);
                } else {
                    printf("FAVORITE_COUNT Não encontrado!\n");
                }
                readOpt();
                readOpt();
            break;

            case 'l':
                printf("LANGUAGE? ");
                scanf("%s", &language);
                CLEAR();
                if(GetTweet(db, n, &t) == 0) {
                    printf("(LANGUAGE: %s)\n", language);
                    printTweet(&t);
                } else {
                    printf("LANGUAGE Não encontrado!\n");
                }
                readOpt();
                readOpt();
                break;

            case 't':
                printf("Qual FAVORITE_COUNT? ");
                scanf("%zu", &n);
                printf("Qual LANGUAGE? ");
                scanf("%s", &language);
                CLEAR();
                if(GetTweet(db, n, &t) == 0) {
                    printf("(FAVORITE_COUNT: %d)\n", n);
                    printf("(LANGUAGE: %s)\n", language);
                    printTweet(&t);
                } else {
                    printf("FAVORITE_COUNT e LANGUAGE Não encontrados!\n");
                }
                readOpt();
                readOpt();
                break;


            case 'g':
                printf("Qual FAVORITE_COUNT? ");
                scanf("%zu", &n);
                printf("Qual LANGUAGE? ");
                scanf("%s", &language);
                CLEAR();
                if(GetTweet(db, n, &t) == 0) {
                    printf("(FAVORITE_COUNT: %d)\n", n)
                    printf("(LANGUAGE: %s)\n", language);
                    printTweet(&t);
                } else {
                    printf("FAVORITE_COUNT e LANGUAGE Não encontrados!\n");
                }
                readOpt();
                readOpt();
                break;

            case 'x':
                printf("FAVORITE_COUNT? ");
                scanf("%zu", &n);
                CLEAR();
                if()
                {
                }
                else
                    printf("FAVORITE_COUNT Nao encontrado!\n");
                readOpt();
                readOpt();
            break;*/

            case 'q':
                exitFlag = 1;
            break;

            case '\n': break;
            default:
                CLEAR();
                printf("OPÇÃO INVÁLIDA!\n");
                readOpt();
        }
    }

    FreeDatabase(db);
    return 0;
}
