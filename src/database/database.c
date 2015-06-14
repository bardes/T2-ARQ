#include "database.h"

#include <sys/stat.h>

#include <fcntl.h>

#include "../io.h"
#include "../utils.h"

/**
 * Estrutura usada para salvar informações gerais do banco de dados no início
 * do arquivo de dados.
 */
typedef struct {
    uint32_t queueHead;     //! Byte offset da cabeça da fila de regs. apagados.
    uint32_t queueEnd;      //! Byte offset do ultimo elemento da fila.
    uint32_t activeCounter; //! Contador de registros ativos no momento.
} DatabaseHeader;

/**
 * Estrutura interna usada para representar o banco de dados.
 */
struct _Database_t {
    DatabaseHeader header;
    FILE *dataFile;
};

Database* CreateDatabase(const char* path, const char* name)
{
    Database *db = malloc(sizeof(Database));
    FAIL(db, NULL);

    // Abre/cria o arquivo de dados
    int fd = open(path, O_RDWR | O_CREAT);
    if(fd < 0) {
        free(db);
        FAIL(0, NULL);
    }

    // Transforma o descritor num ponteiro de arquivo
    db->dataFile = fdopen(fd, "r+");
    if(!db->dataFile) {
        free(db);
        FAIL(0, NULL);
    }

    // Lê o header
    if(fread(&(db->header), sizeof(DatabaseHeader), 1, db->dataFile) != 1) {
        FreeDatabase(db);
        FAIL(0, NULL);
    }

    return db;
}

void FreeDatabase(Database* db)
{
    fwrite(&(db->header), sizeof(DatabaseHeader), 1, db->dataFile);
    fclose(db->dataFile);
    free(db);
}

int InsertTweet(Database* db, const Tweet* t)
{
    return -1; // Nao esquecer de incrementar o activeCounter
}

int RemoveTweet(Database* db, uint32_t offset)
{
    // Posiciona no registro a ser removido
    FAIL(fseek(db->dataFile, offset, SEEK_SET) == 0, -1);

    // Verifica se existe ou se já não está removido
    Tweet *t = CreateTweet(); FAIL(t, -1);
    ReadTweet(db->dataFile, &t);
    if(!t) {
        return -1;
    } else if(GET_BIT(t1->flags, ACTIVE_BIT) == 0) {
        FreeTweet(t);
        return 1;
    }

    // Marca a remoção lógica
    CLEAR_BIT(t->flags, ACTIVE_BIT);

    // Faz o atual fim da fila apontar pra esse registro
    FAIL(fseek(db->dataFile, db->header.queueEnd + 8, SEEK_SET) == 0, -1);
    fwrite(&offset, sizeof(uint32_t), 1, db->dataFile);
    db->header.queueEnd = offset;

    // Escreve a remoção.
    FAIL(fseek(db->dataFile, offset + 4, SEEK_SET) == 0, -1);
    fwrite(&(t->flags), sizeof(uint32_t), 1, db->dataFile);

    FreeTweet(t);
    return 0;
}

size_t GetSize(const Database* db)
{
    return db->header.activeCounter;
}