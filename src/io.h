#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>

/**
 * Envia um caracter de controle para limpar a tela.
 */
#define CLEAR() write(1,"\33[H\33[2J",7)

/**
 * Lê o arquivo dado até encontrar um caracter igual a 'stop'.
 *
 * Essa função aloca uma string com o tamanho necessário para retornar a string
 * lida. É responsabilidade de quem chamá-la liberar essa memoria depois.
 *
 * O caracter stop (se encontrado) não é colocado na string retornada.
 *
 * Para verificar se a função retornou por causa de um caracter stop ou porque
 * o arquivo acabou o usuário deve checar usando a função feof().
 *
 * \param src Arquivo a ser lido.
 * \param stop Caracter delimitador
 *
 * \return String com os dados lidos ou NULL em caso de falhas de memória.
 */
char *readUntil(FILE *src, int stop);

#endif
