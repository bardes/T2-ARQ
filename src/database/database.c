/* Grupo: 2
 * Ana Caroline Fernandes Spengler 8532356
 * Paulo Bardes                    8531932
 * Renato Goto                     8516692
*/

#include "database.h"

#include <sys/stat.h>

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

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
    char *path;
};

struct _DatabaseItr_t {
    FILE *data;
};

Database* CreateDatabase(const char* path)
{
    Database *db = malloc(sizeof(Database));
    FAIL(db, NULL);

    db->path = strdup(path);

    // Abre/cria o arquivo de dados
    int fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
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
        DMSG("Não conseguiu ler o header. Arquivo sera truncado!");
        db->header.activeCounter = 0;
        db->header.queueEnd = db->header.queueHead = INVALID;
        ftruncate(fd, 0);
        fwrite(&(db->header), sizeof(DatabaseHeader), 1, db->dataFile);
    }

    return db;
}

void FreeDatabase(Database* db)
{
    // Escreve o header antes de fechar o arquivo
    fseek(db->dataFile, 0, SEEK_SET);
    fwrite(&(db->header), sizeof(DatabaseHeader), 1, db->dataFile);
    fclose(db->dataFile);
    free(db->path);
    free(db);
}

DatabaseItr* GetIterator(const Database* db)
{
    fflush(db->dataFile);

    DatabaseItr *itr = malloc(sizeof(DatabaseItr));
    FATAL(itr, 1);

    int fd = dup(fileno(db->dataFile));
    FATAL(fd > 0, 1);

    itr->data = fdopen(fd, "r");
    FATAL(itr->data, 1);

    // Pula o header
    fseek(itr->data, sizeof(DatabaseHeader), SEEK_SET);
    DMSG("Criado iterador. (%p)", (void*) itr);
    return itr;
}

void FreeIterator(DatabaseItr* itr)
{
    fclose(itr->data);
    free(itr);
    DMSG("Liberado iterador. (%p)", (void*) itr);
}

int GetNextTweet(DatabaseItr* itr, Tweet* dest)
{
    do {
        if(ReadTweet(itr->data, dest) < 0) return -1;
    } while(GET_BIT(dest->flags, ACTIVE_BIT) == 0);

    return 0;
}

static uint32_t GetBestFit(Database *db, uint32_t size, uint32_t *p, uint32_t *b)
{
    DMSG("Procurando melhor espaço para inserir %u bytes.", size);
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
            if(bestSize == size) {
                DMSG("Achou um espaço do tamanho exato!");
                return 0;
            }
            DMSG("Cabe no offset: 0x%08X (%u bytes livres)", curr, readSize);
        }
    }

    if(*b == INVALID) {
        DMSG("Nenhuma posição livre achada com o tamanho mínimo desejado.");
        return 0;
    } else {
        DMSG("Melhor offset achado: 0x%08X", *b);
        return bestSize - size;
    }
}

static void WriteEmptyRegiter(FILE *f, uint32_t pos, uint32_t size, uint32_t next)
{
    uint32_t flags = 0;
    size -= sizeof(uint32_t); // Precisa de espaço para o indicador de tamanho!
    fseek(f, (long) pos, SEEK_SET);
    fwrite(&size, sizeof(uint32_t), 1, f);
    fwrite(&flags, sizeof(uint32_t), 1, f);
    fwrite(&next, sizeof(uint32_t), 1, f);
    DMSG("Registro vazio de %u bytes criado. (0x%08X --> 0x%08X)", size, pos, next);
}

int InsertTweet(Database* db, const Tweet* t)
{
    if(!(t && db)) return -1;

    DMSG("Inserindo tweet no banco de dados.");
    fflush(db->dataFile); // Garante que o arquivo esta atualizado antes de inserir

    // Acha a melhor posição vazia (se tiver)
    uint32_t prev, best;
    uint32_t len = (uint32_t) SizeOfTweet(t);
    uint32_t spaceLeft = GetBestFit(db, len, &prev, &best);

    if(best == INVALID) { // Inserção normal
        DMSG("Inserindo no fim do arquivo...");
        fseek(db->dataFile, 0, SEEK_END);
        if(WriteTweet(db->dataFile, t) != 0) return -1;
    } else { // Reaproveita espaço
        // Lê o offset do proximo apagado
        uint32_t next;
        fseek(db->dataFile, (long) (best + 8), SEEK_SET);
        fread(&next, sizeof(uint32_t), 1, db->dataFile);

        // Sobrescreve o tweet apagado
        DMSG("Escevendo sobre o offset 0x%08X.", best);
        fseek(db->dataFile, (long) best, SEEK_SET);
        if(WriteTweet(db->dataFile, t) != 0) return -1;

        // Arruma as ligações da fila de tweets apagados
        DMSG("Ajsutando a fila interna de registros apagados.");
        if(prev == INVALID) { // Precisa atualizar o header do DB
            DMSG("Cabeça da fila modificada.");
            int flag = db->header.queueEnd == db->header.queueHead;
            if(spaceLeft >= 24) { // Sobrou espaço suficiente para reutilizar novamente
                db->header.queueHead = best + sizeof(uint32_t) + len; // Reajusta o início da posição livre
                WriteEmptyRegiter(db->dataFile, db->header.queueHead, spaceLeft, next);
            } else { // Não sobrou espaço suficiente para reutilizar
                db->header.queueHead = next; // Aponta pro proximo espaço

                // Reajusta o tamanho do registro
                fseek(db->dataFile, (long) best, SEEK_SET);
                uint32_t newSize = ((uint32_t) SizeOfTweet(t)) + spaceLeft;
                fwrite(&newSize, sizeof(uint32_t), 1, db->dataFile);
                DMSG("Registro 0x%08X reajustado para %u bytes.", best, newSize);
            }
            DMSG("Nova cabeça: 0x%08X", db->header.queueHead);
            if(flag) {
                DMSG("Cabeça e fim da lista são o memso registro.");
                db->header.queueEnd = db->header.queueHead;
                DMSG("Ajustando o fim da fila também.");
            }
        } else { // Precisa atualizar o registro vazio anterior
            DMSG("Atualizando a referência do registro anterior na fila.");
            fseek(db->dataFile, (long) (prev + 8), SEEK_SET);
            if(spaceLeft >= 24) { // Sobrou espaço suficiente para reutilizar novamente
                uint32_t tmp = best + sizeof(uint32_t) + len;
                fwrite(&tmp, sizeof(uint32_t), 1, db->dataFile);
                WriteEmptyRegiter(db->dataFile, tmp, spaceLeft, next);
                DMSG("Registro atualizado: 0x%08X --> 0x%08X", prev, tmp);
                if(best == db->header.queueEnd) {
                    DMSG("Fim da fila alterado! Atualizando o cabeçalho.");
                    db->header.queueEnd = tmp;
                }
            } else { // Não sobrou espaço suficiente para reutilizar
                fwrite(&next, sizeof(uint32_t), 1, db->dataFile);
                DMSG("Registro atualizado: 0x%08X --> 0x%08X", prev, next);
                if(best == db->header.queueEnd) {
                    DMSG("Fim da fila alterado! Atualizando o cabeçalho.");
                    db->header.queueEnd = prev;
                }

                // Reajusta o tamanho do registro
                fseek(db->dataFile, (long) best, SEEK_SET);
                uint32_t newSize = ((uint32_t) SizeOfTweet(t)) + spaceLeft;
                fwrite(&newSize, sizeof(uint32_t), 1, db->dataFile);
                DMSG("Registro 0x%08X reajustado para %u bytes.", best, newSize);
            }
        }
    }

    ++(db->header.activeCounter);
    DMSG("Inserido com sucesso.");
    return 0;
}

int RemoveTweet(Database* db, uint32_t offset)
{
    if(offset == INVALID) return 1;
    DMSG("Deletendo registro no offset: 0x%08X", offset);
    // Posiciona no registro a ser removido
    FAIL(fseek(db->dataFile, offset, SEEK_SET) == 0, -1);

    // Verifica se existe ou se já não está removido
    Tweet *t = CreateTweet(); FAIL(t, -1);
    FATAL(t, 1);

    if(ReadTweet(db->dataFile, t) < 0) {
        DMSG("Nenhum tweet válido nesse offset!");
        return -1;
    } else if(GET_BIT(t->flags, ACTIVE_BIT) == 0) {
        FreeTweet(t);
        DMSG("Tweet já estava apagado!");
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
        DMSG("Primeira deleção! Atualizando o cabeçalho.");
        db->header.queueHead = offset;
    } else { // Se já exixte uma fila
        // Faz o atual fim da fila apontar pra esse registro
        DMSG("Adicionando registro no fim da fila de deleções.");
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

TweetSeq FindByUser(Database *db, const char *user)
{
    TweetSeq tws;
    tws.length = 0;
    tws.seq = NULL;

    Tweet tmp;
    DatabaseItr *i = GetIterator(db);
    while(GetNextTweet(i, &tmp) == 0) {     // Percorre todos os tweets ativos
        if(strcmp(user, tmp.user) == 0) {   // Filtra apenas os com user desejado
            tws.seq = realloc(tws.seq, (tws.length + 1) * sizeof(Tweet)); // Aumenta o vetor
            FATAL(tws.seq, 1);
            tws.seq[tws.length] = tmp;  // Copia para o vetor
            ++(tws.length);             // Ajusta o contador
        } else {
            FreeTweet(&tmp);
        }
    }
    FreeIterator(i);
    return tws;
}

TweetSeq FindByFav(Database* db, uint32_t favs)
{
    TweetSeq tws;
    tws.length = 0;
    tws.seq = NULL;

    Tweet tmp;
    DatabaseItr *i = GetIterator(db);
    while(GetNextTweet(i, &tmp) == 0) { // Percorre todos os tweets ativos
        if(tmp.favs == favs) {          // Filtra pela qtd. de FAVs
            tws.seq = realloc(tws.seq, (tws.length + 1) * sizeof(Tweet)); // Aumenta o vetor
            FATAL(tws.seq, 1);
            tws.seq[tws.length] = tmp;  // Copia para o vetor
            ++(tws.length);             // Ajusta o contador
        } else {
            FreeTweet(&tmp);
        }
    }
    FreeIterator(i);
    return tws;
}