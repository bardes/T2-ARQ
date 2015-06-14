/**
 * Este arquvio define todas as funções e estruturas de dados diretamente
 * relacionadas à criação e manipulação de tweets.
 */

#ifndef _TWEET_H_
#define _TWEET_H_

#include <stdint.h>
#include <stdio.h>

#define INVALID ((uint32_t)~0) // Offset reservado para indica posição inválida

#define ACTIVE_BIT 0 // Bit dos flags para indicar deleção lógica

/**
 * Estrutura de dado usada para represntar um tweet.
 */
typedef struct {
    char *text;             /* Texto do tweet */
    char *user;             /* Nome de usuário do autor */
    char *coordinates;      /* Localização geográfica do tweet */
    char *language;         /* Língua do tweet */
    uint32_t flags;         /* Flags desse tweet */
    uint32_t nextFreeEntry; /* Byte offset da proxima posição livre */
    uint32_t favs;          /* Quantidade de favoritos */
    uint32_t views;         /* Contador de vizualizações */
    uint32_t retweets;      /* Contador de retweets */
    uint32_t byteOffset;    /* Posição desse tweet no arquivo de dados */
} Tweet;

typedef struct {
    Tweet *seq;
    size_t length;
} TweetSeq;

/**
 * Cria um tweet nulo.
 */
Tweet *CreateTweet();

/**
 * Libera o tweet da memória.
 */
void FreeTweet(Tweet *t);

/**
 * Cria um tweet de maneira interativa, usando a entrada padrão.
 *
 * \return Ponteiro para o novo tweet ou NULL em caso de falha.
 */
Tweet *ComposeTweet();

/**
 * Imprime o tweet dado na saida padrão.
 */
void PrintTweet(const Tweet *t);

/**
 * Tenta ler um tweet à partir da posição atual do arquivo dado.
 *
 * \param f Arquivo a ser lido.
 * \param tw Tweet onde serão escritos os dados.
 *
 * \return Tamanho em disco do tweet lido. ou < 0 em caso de falhas.
 */
int ReadTweet(FILE *f, Tweet *tw);

/**
 * Tenta escreveer o tweet no arquivo dado.
 *
 * \param f Arquivo de destino.
 * \param tw Tweet a ser escrito
 *
 * \return 0 em caso de sucesso, < 0 em caso de erros.
 */
int WriteTweet(FILE *f, const Tweet *tw);

/**
 * Calcula o tamanho em disco de um tweet.
 */
size_t SizeOfTweet(const Tweet *tw);

#endif /* _TWEET_H_ */
