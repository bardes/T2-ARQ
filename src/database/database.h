/**
 * Este arquivo define todas as funções necessárias para interagir com o
 * banco de dados. Salvo casos onde os comentários indicam um comportamento
 * específico, todas as funções que retornam inteiros seguem a seguinte
 * convenção: 0: Sucesso, <0: Falha, >0: Aviso
 */

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "../tweet.h"

/**
 * Protótipo do banco de dados.
 *
 * \see database.c
 */
typedef struct _Database_t Database;

/**
 * Cria um banco de dados no caminho especificado.
 *
 * If the given path+name already exists then the existing contents will be
 * loaded.
 *
 * \param path Caminho completo para o arquivo onde o banco deve ser criado.
 *
 * \return NULL em caso de falha.
 */
Database *CreateDatabase(const char *path);

/**
 * Fecha e libera o banco de dados da memória (RAM).
 */
void FreeDatabase(Database *db);

/**
 * Insere um tweet no banco de dados.
 */
int InsertTweet(Database *db, const Tweet *t);

/**
 * Pega a quantidade de registros ativos no banco de dados.
 */
size_t GetSize(const Database *db);

/**
 * Remove um tweet com base em seu byte offset.
 */
int RemoveTweet(Database *db, uint32_t offset);

#endif /* _DATABASE_H_ */
