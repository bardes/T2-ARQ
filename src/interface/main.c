#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "../utils.h"
#include "../database/database.h"
#include "io.h"

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
        "R - Buscar Tweets por RRN\n"
        "X - Apagar Tweet\n"
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
        char uname[TW_USER_LEN];
        Tweet t, *result;
        size_t n;

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

            case 'r':
                printf("RRN? ");
                scanf("%zu", &n);
                CLEAR();
                if(GetTweet(db, n, &t) == 0) {
                    printf("(RRN: %zu)\n", n);
                    printTweet(&t);
                } else {
                    printf("RRN Não encontrado!\n");
                }
                readOpt();
                readOpt();
            break;

            case 'x':
                printf("RRN? ");
                scanf("%zu", &n);
                CLEAR();
                int err = RemoveTweet(db, n);
                if(err == 0) {
                    printf("RRN %zu removido com sucesso!\n", n);
                } else if (err == 1) {
                    printf("Esse registro já está apagado!\n");
                } else {
                    printf("RRN Não encontrado!\n");
                }
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
