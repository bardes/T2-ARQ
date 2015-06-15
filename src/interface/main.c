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
        "F - Buscar Tweets por FAVORITE_COUNT\n"
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
    size_t i = 0;
    for(;;) {
        CLEAR();
        printf("(%zu/%zu)\n", i+1, s.length);
        PrintTweet(s.seq + i);
        printf("pressione <enter> para avançar.\n");
        printf("    'v' + <enter> para voltar.\n");
        printf("    'q' + <enter> para voltar ao menu.\n");

        switch(readOpt())
        {
            case 'v': i = i == 0 ? s.length - 1 : i - 1; break;
            case 'q': return;
            default: ++i; i %= s.length;;
        }
    }
}

static uint32_t ChooseSeq(TweetSeq s)
{
    size_t i = 0;
    for(;;) {
        CLEAR();
        printf("(%zu/%zu)\n", i+1, s.length);
        PrintTweet(s.seq + i);
        printf("\n\n\nOpções:\n\t    'x' - apagar.\n");
        printf("\t    'q' - volta sem apagar.\n");
        printf("\t<enter> - volta sem apagar.\n");
        printf("\n\nEscolha: ");
        switch(readOpt())
        {
            case 'x': return s.seq[i].byteOffset;
            case 'q': return INVALID;
            default: break;
        }

        ++i; i %= s.length;
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
        Tweet t;
        size_t n;
        TweetSeq s;

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
                scanf("%zu", &n); fgetc(stdin);
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
                        printf("(%zu/%zu) (Offset: 0x%08X)\n", i+1, n, t.byteOffset);
                        PrintTweet(&t);
                        printf("\nPressione <enter> para Avançar...");
                        if(readOpt() == 'q') break;
                    }
                    FreeIterator(itr);
                }
            break;

            case 'u':
                printf("USER? "); uname = readUntil(stdin, '\n');
                s = FindByUser(db, uname);
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

            case 'f':
                printf("FAVORITE_COUNT? ");
                scanf(" %zu", &n); fgetc(stdin);
                s = FindByFav(db, (uint32_t) n);
                if(s.seq) {
                    ShowSeq(s);
                } else {
                    CLEAR();
                    printf("Nenhum Tweet encontrado.\n");
                    readOpt();
                }
                FreeTweetSeq(s);
            break;
/*
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
                break;*/

            case 'x':
                printf("FAVORITE_COUNT? ");
                scanf(" %zu", &n); fgetc(stdin);
                s = FindByFav(db, (uint32_t) n);
                if(s.seq) {
                    if(RemoveTweet(db, ChooseSeq(s))) {
                        CLEAR();
                        printf("Não removeu o tweet.\n");
                    } else {
                        CLEAR();
                        printf("Tweet removido com sucesso.\n");
                    }
                } else {
                    CLEAR();
                    printf("Nenhum Tweet encontrado.\n");
                }
                readOpt();
                FreeTweetSeq(s);
            break;

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
