#include"tweet.h"

#include <stdio.h>
#include <stdlib.h>

#include"io.h"
#include "utils.h"

#define SEP ((char)0)

void FreeTweet(Tweet *t)
{
    if(!t) return;

    free(t->text);
    free(t->user);
    free(t->language);
    free(t->coordinates);
    free(t);
}

Tweet *ComposeTweet()
{
}

Tweet *CreateTweet()
{
    // Aloca o tweet
    Tweet *tw = malloc(sizeof(Tweet));
    FAIL(tw, NULL);

    // Inicializa os valores
    tw->text = tw->user
             = tw->coordinates
             = tw->language = NULL;

    tw->byteOffset = tw->favs
                   = tw->flags
                   = tw->nextFreeEntry
                   = tw->retweets
                   = tw->views = INVALID;

}

int ReadTweet(FILE *f, Tweet *tw)
{
    uint32_t tweetLen;

    // Aloca um tweet nulo
    Tweet *tw = CreateTweet();
    tw->byteOffset = (uint32_t) ftell(f);

    // Le o tamanho do registro
    if(fread(&tweetLen, sizeof(uint32_t), 1, f) != 1) {
        FAIL_MSG(0, NULL, "Falha ao ler tweet!");
    }

    // Le os flags
    if(fread(&tw->flags, sizeof(int), 1, f) != 1) {
        FAIL_MSG(0, NULL, "Falha ao ler tweet!");
    }

    // Se estiver apagado lê apenas o "ponteiro" e avança a posição do arquivo
    if(GET_BIT(tw->flags, ACTIVE_BIT) == 0) {
        // Le o byte offset do proximo registro livre
        if(fread(&tw->nextFreeEntry, sizeof(int), 1, f) != 1) {
            FAIL_MSG(0, NULL, "Falha ao ler tweet!");
        }
        fseek(f, (long) (tweetLen - 2 * sizeof(uint32_t)), SEEK_CUR);
    } else { // Senão continua lendo os outros campos
        // Le a contagem de favoritos
        if(fread(&tw->favs, sizeof(int), 1, f) != 1) {
            FAIL_MSG(0, NULL, "Falha ao ler tweet!");
        }

        // Le a contagem de views
        if(fread(&tw->views, sizeof(int), 1, f) != 1) {
            FAIL_MSG(0, NULL, "Falha ao ler tweet!");
        }

        // Le a contagem de retweets
        if(fread(&tw->retweets, sizeof(int), 1, f) != 1) {
            FAIL_MSG(0, NULL, "Falha ao ler tweet!");
        }

        // Le os campos de tamanho variável
        tw->text = readUntil(f, SEP);
        tw->user = readUntil(f, SEP);
        tw->coordinates = readUntil(f, SEP);
        tw->language = readUntil(f, SEP);

        // Verifica se não houve nenhuma falha
        if(!(tw->text && tw->user && tw->coordinates && tw->language)) {
            FAIL_MSG(0, NULL, "Falha ao ler tweet!");
        }
    }

    return tweetLen;
}

/**
 * Calcula o tamanho em disco de um tweet.
 */
static size_t SizeOfTweet(const Tweet *tw)
{
    return 4 * sizeof(uint32_t) +
            + strlen(tw.text) + 1
            + strlen(tw.user) + 1
            + strlen(tw.coordinates) + 1
            + strlen(tw.language) + 1;
}

int WriteTweet(FILE *f, const Tweet *tw)
{
    // Não escreve tweets apagados
    if(GET_BIT(tw->flags, ACTIVE_BIT) == 0) return -1;

    uint32_t twSize = (uint32_t) SizeOfTweet(tw);

    // Escreve o indicador de tamanho
    if(fwrite(&twSize, sizeof(uint32_t), 1, f) != 1)
        FAIL_MSG(0, -1, "Falha ao escrever tweet!");

    // Seguido pelos dados de tamanho fixo
    if(fwrite(&(tw->flags), sizeof(uint32_t), 1, f) != 1)
        FAIL_MSG(0, -1, "Falha ao escrever tweet!");
    if(fwrite(&(tw->favs), sizeof(uint32_t), 1, f) != 1)
        FAIL_MSG(0, -1, "Falha ao escrever tweet!");
    if(fwrite(&(tw->views), sizeof(uint32_t), 1, f) != 1)
        FAIL_MSG(0, -1, "Falha ao escrever tweet!");
    if(fwrite(&(tw->retweets), sizeof(uint32_t), 1, f) != 1)
        FAIL_MSG(0, -1, "Falha ao escrever tweet!");

    // E agora os de tamanho variável, separados por '\0's
    FAIL_MSG(fputs(tw->text, f) != EOF, -1, "Falha ao escrever tweet!");
    FAIL_MSG(fputs(tw->user, f) != EOF, -1, "Falha ao escrever tweet!");
    FAIL_MSG(fputs(tw->coordinates, f) != EOF, -1, "Falha ao escrever tweet!");
    FAIL_MSG(fputs(tw->language, f) != EOF, -1, "Falha ao escrever tweet!");

    return 0;
}



























