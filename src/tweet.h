/**
 * Este arquvio define todas as funções e estruturas de dados diretamente
 * relacionadas à criação e manipulação de tweets.
 */

#ifndef _TWEET_H_
#define _TWEET_H_

#include <stdint.h>
#include <stdio.h>

/**
 * Flags usados para guardar meta informações.
 */
enum flags {
	ACTIVE_BIT = 0 //! Bit usado para indicar remoção lógica.
};

/**
 * Estrutura de dado usada para represntar um tweet.
 */
typedef struct {
    uint32_t flags;         /* Flags desse tweet */
    uint32_t nextFreeEntry; /* Byte offset da proxima posição livre */
    uint32_t favs;          /* Quantidade de favoritos */
    uint32_t views;         /* Contador de vizualizações */
    uint32_t retweets;      /* Contador de retweets */
    char *text;             /* Texto do tweet */
    char *user;             /* Nome de usuário do autor */
    char *coordinates;      /* Localização geográfica do tweet */
    char *language;         /* Língua do tweet */
} Tweet;

/**
 * Libera o tweet da memória.
 */
void freeTweet(Tweet *t);

/**
 * Cria um tweet de maneira interativa, usando a entrada padrão.
 *
 * \return Ponteiro para o novo tweet ou NULL em caso de falha.
 */
Tweet *composeTweet();

/**
 * Tenta ler um tweet à partir da posição atual do arquivo dado.
 *
 * \param f Arquivo a ser lido.
 *
 * \return Ponteiro para o tweet lido, ou NULL em caso de falha.
 */
Tweet *readTweet(FILE *f);

/**
 * Tenta escreveer o tweet no arquivo dado.
 *
 * \param f Arquivo de destino.
 *
 * \return 0 em caso de sucesso, < 0 em caso de erros.
 */
int writeTweet(FILE *f);

#endif /* _TWEET_H_ */
