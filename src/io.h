#ifndef _IO_H_
#define _IO_H_

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
char *readUntil(FILE *src, char stop);

/**
 * Lê um tweet da entrada padrão de maneira interativa.
 *
 * \param t Tweet onde serão gravados os dados lidos.
 *
 * \return 0 Em caso de sucesso, < 0 em caso de falha.
 */
int readTweet(Tweet *t);

/**
 * Imprime o tweet dado na saida padrão.
 */
void printTweet(const Tweet *t);

#endif
