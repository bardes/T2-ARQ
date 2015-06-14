#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "../utils.h"
#include "../database/database.h"

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
        "F - Buscar Tweets por FAVORITE_COUNT\n"
        "L - Buscar Tweets por LANGUAGE\n"
        "T - Buscar Tweets por FAVORITE_COUNT e LANGUAGE atraves do matching\n"
        "G - Buscar Tweets por FAVORITE_COUNT e LANGUAGE atraves do merging\n"
        "X - Apagar Tweet pelo FAVORITE_COUNT\n"
        "Q - Sair\n\n\n"
    );
}

int main(int argc, char *argv[])
{
    if(argc != 2) {
        printf("Usage: %s <database>\n", argv[0]);
        return 1;
    }

    Database *db = CreateDatabase("./", argv[1]);
    FAIL_MSG(db, 1, "Falha ao criar banco de dados!");

    char exit = 0;
    while(!exit) {
        char *uname;
        Tweet t, *result;
        size_t n;
        char *language;

        printMenu();
        printf("Escolha: ");

        switch(readOpt()) {
            case 'a':
                CLEAR();
                readTweet(&t);
                if(InsertTweet(db, &t) != 0)  {
                    FAIL_MSG(0, 1, "Falha ao inserir Tweet!");
                    readOpt();
                }
            break;

            case 'v':
                printf("Quantos? ");
                scanf("%zu", &n);
                for(size_t i = 0; i < n; ++i) {
                    CLEAR();
                    printf("(%zu/%zu)\n", i+1, n);
                    readTweet(&t);
                    if(InsertTweet(db, &t) != 0) {
                        FAIL_MSG(0, 1, "Falha ao inserir Tweet!");
                        readOpt();
                    }
                }
            break;

            case 'm':
                n = GetSize(db);
                for(size_t i = 0; i < n; ++i) {
                    CLEAR();
                    printf("(%zu/%zu)\n", i+1, n);
                    if(GetTweet(db, i, &t) == 0) {
                        printTweet(&t);
                    } else {
                        printf("==== APAGADO ====\n");
                    }
                    readOpt();
                }
            break;

            case 'u':
                printf("USER? ");
                readUntil(uname, TW_USER_LEN, '\n');
                if(GetTweetsByUser(db, uname, &result, &n) == 0 && n != 0) {
                    for(size_t i = 0; i < n; ++i) {
                        CLEAR();
                        printf("(%zu/%zu)\n", i+1, n);
                        printTweet(&result[i]);
                        readOpt();
                    }
                    free(result);
                } else {
                    CLEAR();
                    printf("Nenhum Tweet com USER \"%s\" encontrado.\n", uname);
                    readOpt();
                }
            break;

            case 'f':
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
            break;

            case 'q':
                exit = 1;
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
