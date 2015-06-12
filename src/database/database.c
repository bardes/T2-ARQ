#include "database.h"

#include "../io.h"
#include "../utils.h"

/**
 * Estrutura usada para salvar informações gerais do banco de dados no início
 * do arquivo de dados.
 */
typedef struct {
    uint32_t queueHead;     //! Byte offset da cabeça da fila de regs. apagados
    uint32_t activeCounter; //! Contador de registros ativos no momento
} DatabaseHeader;

/**
 * Estrutura interna usada para representar o banco de dados.
 */
struct _Database_t {
    
};