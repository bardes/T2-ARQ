#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"io.h"
#include"tweet.h"

#define SEP ((char)0)

/**
 * Libera o tweet da memória.
 */
void freeTweet(Tweet *t)
{
	free(t);
}

/**
 * Cria um tweet de maneira interativa, usando a entrada padrão.
 *
 * \return Ponteiro para o novo tweet ou NULL em caso de falha.
 */
Tweet *composeTweet()
{
	
}

/**
 * Tenta ler um tweet à partir da posição atual do arquivo dado.
 *
 * \param f Arquivo a ser lido.
 *
 * \return Pt[0].a = 0;
	t[1].a = 1;
	t[2].a = 2;ponteiro para o tweet lido, ou NULL em caso de falha.
 */
Tweet *readTweet(FILE *f)
{
	int tweetLen;
	
	Tweet *tw;
	tw = (Tweet*) malloc(sizeof(Tweet));
	
	fread(&tweetLen, sizeof(int), 1, f);
	
	fread(&tw->flags, sizeof(int), 1, f);
	fread(&tw->nextFreeEntry, sizeof(int), 1, f);
	fread(&tw->favs, sizeof(int), 1, f);
	fread(&tw->views, sizeof(int), 1, f);
	fread(&tw->retweets, sizeof(int), 1, f);
	
	tw->text = readUntil(f, SEP);
	tw->user = readUntil(f, SEP);
	tw->coordinates = readUntil(f, SEP);
	tw->language = readUntil(f, SEP);
	
	return = tw;	
}

/**
 * Tenta escreveer o tweet no arquivo dado.
 *
 * \param tw Tweet.
 *
 * \return Tamanho do tweet.
 */
int sizeOfTweet(Tweet tw)
{
// 	5 eh a quantidade de delimtadores usados
	int twSize = 4 * sizeof(SEP);

	twSize = 5 * sizeof(uint32_t);
	twSize += strlen(tw.text) + strlen(tw.user) + strlen(tw.coordinates)+ strlen(tw.language);	
	
	return twSize;
}

/**
 * Tenta escreveer o tweet no arquivo dado.
 *
 * \param f Arquivo de destino.
 *
 * \return 0 em caso de sucesso, < 0 em caso de erros.
 */
int writeTweet(FILE *f, Tweet tw)
{
	uint32_t twSize = (uint32_t) sizeOfTweet(tw);
	
	fwrite(&twSize, sizeof(int), 1, f);
	fwrite(&tw->flags, sizeof(int), 1, f);
	fwrite(&tw->nextFreeEntry, sizeof(int), 1, f);
	fwrite(&tw->favs, sizeof(int), 1, f);
	fwrite(&tw->views, sizeof(int), 1, f);
	fwrite(&tw->retweets, sizeof(int), 1, f);
		
	fwrite(&tw->text, sizeof(int), 1, f);
	fwrite(&SEP, sizeof(char), 1, f);
	
	fwrite(&tw->user, sizeof(int), 1, f);
	fwrite(&SEP, sizeof(char), 1, f);
	
	fwrite(&tw->coordinates, sizeof(int), 1, f);
	fwrite(&SEP, sizeof(char), 1, f);
	
	fwrite(&tw->language, sizeof(int), 1, f);
	fwrite(&SEP, sizeof(char), 1, f);
	
	return 0;
}



























