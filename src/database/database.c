#include "database.h"

#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>

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

Database* CreateDatabase(const char* path   )
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

static uint32_t GetBestFit(Database *db, uint32_t size, uint32_t *p, uint32_t *b)
{
    *p = INVALID;
    *b = INVALID;

    uint32_t next = db->header.queueHead;
    uint32_t curr = INVALID;
    uint32_t prev = INVALID;
    uint32_t bestSize = UINT32_MAX;

    while(next != INVALID) {
        uint32_t readSize;

        prev = curr;
        curr = next;

        // Le o tamanho
        fseek(db->dataFile, curr, SEEK_SET);
        fread(&readSize, sizeof(uint32_t), 1, db->dataFile);

        // Le o offset do proximo registro apagado
        fseek(db->dataFile, 4, SEEK_CUR); // Pula os flags
        fread(&next, sizeof(uint32_t), 1, db->dataFile);

        // Checa se achou uma posição melhor.
        if(readSize > size && readSize < bestSize) {
            bestSize = readSize;
            *b = curr;
            *p = prev;
            if(bestSize == size) return 0;
        }
    }
    return bestSize - size;
}

int InsertTweet(Database* db, const Tweet* t)
{
    fflush(db->dataFile); // Garante que o arquivo esta atualizado antes de inserir

    uint32_t prev, best;
    uint32_t len = (uint32_t) SizeOfTweet(t);
    uint32_t spaceLeft = GetBestFit(db, len, &prev, &best);

    if(best == INVALID) { // Inserção normal
        fseek(db->dataFile, 0, SEEK_END);
        return WriteTweet(db->dataFile, t);
    } else { // Reaproveita espaço
        // Lê o offset do proximo apagado
        uint32_t next;
        fseek(db->dataFile, (long) (best + 8), SEEK_SET);
        fread(&next, sizeof(uint32_t), 1, db->dataFile);

        // Sobrescreve o tweet apagado
        fseek(db->dataFile, (long) best, SEEK_SET);
        if(WriteTweet(db->dataFile, t) != 0) return -1;

        // Arruma as ligações da fila de tweets apagados
        if(prev == INVALID) { // Precisa atualizar o header do DB
            int flag = db->header.queueEnd == db->header.queueHead;
            if(spaceLeft > 24) { // Sobrou espaço suficiente para reutilizar novamente
                db->header.queueHead = best + sizeof(uint32_t) + len; // Reajusta o início da posição livre
            } else { // Não sobrou espaço suficiente para reutilizar
                db->header.queueHead = next;
            }
            if(flag) db->header.queueEnd = db->header.queueHead;
        } else { // Precisa atualizar o registro vazio anterior
            fseek(db->dataFile, (long) (prev + 8), SEEK_SET);
            if(spaceLeft > 24) { // Sobrou espaço suficiente para reutilizar novamente
                uint32_t tmp = best + sizeof(uint32_t) + len;
                fwrite(&tmp, sizeof(uint32_t), 1, db->dataFile);
            } else { // Não sobrou espaço suficiente para reutilizar
                fwrite(&next, sizeof(uint32_t), 1, db->dataFile);
            }
        }
    }

    ++(db->header.activeCounter);
    return 0;
}

int RemoveTweet(Database* db, uint32_t offset)
{
    // Posiciona no registro a ser removido
    FAIL(fseek(db->dataFile, offset, SEEK_SET) == 0, -1);

    // Verifica se existe ou se já não está removido
    Tweet *t = CreateTweet(); FAIL(t, -1);
    ReadTweet(db->dataFile, t);
    if(!t) {
        return -1;
    } else if(GET_BIT(t->flags, ACTIVE_BIT) == 0) {
        FreeTweet(t);
        return 1;
    }

    // Marca a remoção lógica
    CLEAR_BIT(t->flags, ACTIVE_BIT);

    // Escreve a remoção.
    FAIL(fseek(db->dataFile, offset + 4, SEEK_SET) == 0, -1);
    fwrite(&(t->flags), sizeof(uint32_t), 1, db->dataFile);
    uint32_t inv = INVALID; fwrite(&inv, sizeof(uint32_t), 1, db->dataFile);

    // Checa se é a primeira deleção
    if(db->header.queueEnd == INVALID) {
        db->header.queueHead = offset;
    } else { // Se já exixte uma fila
        // Faz o atual fim da fila apontar pra esse registro
        FAIL(fseek(db->dataFile, db->header.queueEnd + 8, SEEK_SET) == 0, -1);
        fwrite(&offset, sizeof(uint32_t), 1, db->dataFile);
    }

    db->header.queueEnd = offset;
    FreeTweet(t);
    --(db->header.activeCounter);
    return 0;
}

size_t GetSize(const Database* db)
{
    return db->header.activeCounter;
}