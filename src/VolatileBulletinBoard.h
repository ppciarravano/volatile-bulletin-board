/*
 ============================================================================
 Name        : VolatileBulletinBoard.h
 Author      : Pier Paolo Ciarravano
 Version     : 1.0 - 15/09/2008
 Copyright   : GPL - General Public License
 Description : Bacheca Elettronica Volatile in C, Ansi-style
               Corso di Sistemi Operativi I - Prof. A.Santoro 2007-2008
               Ingegneria Informatica - Univ. "La Sapienza" Roma
 ============================================================================
 */

#ifndef VOLATILEBULLETINBOARD_H_
#define VOLATILEBULLETINBOARD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pwd.h>
#include <signal.h>
#include <fcntl.h>

//Struttura dei messaggi
typedef struct s_message_bb {
		int id;
		int isFree;
		char title[100];
		char text[2048];
		char userLogin[20];
		char email[100];
		struct tm insertDate; //time inserimento
	} message_bb;

//Struttura formata da l'array di strutture dei messaggi e numero di elementi dell'array
typedef struct s_bulettinboard {
			int max_messages;
			int idSequence;
			message_bb messages[];
		} bulettinboard;

//puntatore alla struttura bulettinboard, puntata dalla memoria condivisa
bulettinboard *MBULLETTINBOARD;

//variabili per valori di default
long int SEMAPHORE_KEY;
long int SMEMORY_KEY;
int MAX_READERS;
int MAX_MESSAGES;

//Setta valori di default
void setDefaultValue();

//Identificatore array di semafori: 0 semaforo READERS (init:MAX_READERS), 1 semaforo WRITERS (init:1 - mutex)
int ID_SEMAPHORE_RW;

//Identificatore memoria condivisa
int ID_SMEMORY;

//Parser degli eventuali parametri da linea di comando e inizializzazione delle variabili
int parseMainOptions_bb(int argc, char *argv[]);

//creazione e inizializzazione memoria condivisa e semafori, utilizzato dal programma di attivazione
//	ritorna:
//	0:ok
//	-1:errore creazione memoria condivisa
//	-2:errore creazione semafori
int init_bb_arg(long int sk, int long mk, int mr, int mm);
//Utilizza valori di default SEMAPHORE_KEY, SMEMORY_KEY, MAX_READERS, MAX_MESSAGES
int init_bb();

//rimuove memoria condivisa e semafori
int destroy_bb_arg(long int sk, int long mk);
//utilizza valori di default SEMAPHORE_KEY, SMEMORY_KEY
int destroy_bb();
//utilizza ID_SMEMORY, ID_SEMAPHORE_RW per rimuovere memoria condivisa e semafori
//FUNZIONE NON USATA MA IMPLEMENTATA COMUNQUE
int destroyUsingId_bb();

//apertura memoria condivisa e semafori, utilizzato dal programma operante sulla bacheca (lettore/scrittore),
// inizializza ID_SMEMORY, ID_SEMAPHORE_RW e MAX_MESSAGES leggendolo dalla memoria condivisa,
// e collega la memoria condivisa al puntatore della struttura di tipo bulettinboard: MBULLETTINBOARD
int open_bb_arg(long int sk, int long mk);
//utilizza i valori di default SEMAPHORE_KEY, SMEMORY_KEY
int open_bb();

//Scollega la memoria condivisa puntata da *MBULLETTINBOARD dallo spazio di indirizzamento del processo
int detachSharedMemory();

//Lista i messaggi nella bacheca ritornando un puntatore alla struttura di tipo bulettinboard,
// che contiene l'array messages clonato con i messaggi presenti nella memoria condivisa,
// per prevenire eventuali effetti collaterali sulla modifica dei messaggi, in una parte di codice non gestita dai semafori
bulettinboard *list_bb(); //OPERAZIONE DI LETTURA

//legge un messaggio prendendo in input l'id del messaggio
// ritorna una copia (clonata) della struttura sulla memoria condivisa,
// la struttura ha id = -1 se il messaggio non e' stato trovato
message_bb readMessage_bb(int idMessage); //OPERAZIONE DI LETTURA

//scrive nell'array dei messaggi della memoria condivisa il messaggio nella prima posizione disponibile
// ritorna l'id univoco assegnato al messaggio, oppure -1 se non ci sono posizioni disponibili
int writeMessage_bb(message_bb message); //OPERAZIONE DI SCRITTURA

//rimuove dall'array dei messaggi della memoria condivisa il messaggio specificando l'id del messaggio,
// inoltre scala tutti i messaggi seguenti in modo da non lasciare buchi nell'array stesso,
// ritorna 0 se buon esito, -1 in caso di errore o se non e' stato trovato il messaggio con id richiesto
int removeMessage_bb(int idMessage); //OPERAZIONE DI SCRITTURA

//Popola la variabile passata per riferimento con la stringa contenente la user name dell'utente che ha lanciato il processo, riempiendo non piu' di size caratteri
void setUserLogin(char *userLogin, int size);

//Popola la variabile passata per riferimento di size 20 con la stringa contenente la stringa formattata della struttura tm passata come parametro, formattata con "GG/MM/AAAA hh:mm:ss"
void formatTime(char *formatDateTime, struct tm *dateTime);

//Popola la variabile tempo passata per riferimento, con l'orario attuale di sistema
void setDateTime(struct tm *dateTime);

//Crea un nuovo messaggio message_bb, popolando i suoi campi e inserendolo nella memoria condivisa,
// utilizzando la funzione writeMessage_bb: ritorna il messaggio stesso creato,
// con id popolato con -1 se non e' stato possibile inserire il messaggio, oppure con l'id univoco assegnato al messaggio
message_bb insertNewMessage(char *title, char *text, char *email);

//printf di tutti i campi del messaggio passato come parametro in un'unica riga
void printMessage(message_bb message);

//modifica nell'array dei messaggi della memoria condivisa, il messaggio passato come paramentro con id popolato,
// ritorna -1 se non ha trovato il messaggio nella memoria condivisa, oppure l'id del messaggio da modificare
//NON RICHISTO DA SPECIFICHE MA IMPLEMENTATO COMUNQUE
//FUNZIONE NON USATA MA IMPLEMENTATA COMUNQUE
int updateMessage_bb(message_bb message); //OPERAZIONE DI SCRITTURA

//Modifica il messaggio con id passato come parametro, controllando se l'utente e' proprietario del messaggio stesso,
// aggiorna anche insertDate,
// ritorna -1 se non ha trovato il messaggio nella memoria condivisa, -2 se l'utente non e' owner del messaggio, oppure l'id del messaggio da modificare
//NON RICHISTO DA SPECIFICHE MA IMPLEMENTATO COMUNQUE
//FUNZIONE NON USATA MA IMPLEMENTATA COMUNQUE
int updateMessage(int idMessage, char *title, char *text, char *email);

//Funzioni per Backup/Restore memoria condivisa su file
//NON RICHISTO DA SPECIFICHE MA IMPLEMENTATO COMUNQUE
//Ritornano -1 in caso di errore o 0 in caso di successo
int fileBackupMessage_bb(char *filename); //OPERAZIONE DI LETTURA
int fileRestoreMessage_bb(char *filename); //OPERAZIONE DI SCRITTURA
char *FILE_MBULLETTINBOARD;

//Funzioni gestione bacheca con menu testuali e funzioni standard per input-output
void runSimpleBulettinboardManager();
void printHelp();
void listBulettinboardManager();
void readBulettinboardManager();
void insertBulettinboardManager();
void removeBulettinboardManager();

//Legge in input da stdin, maxLengthMessage caratteri, fino ad incontrare (invio).(invio) (in stile DATA SMTP)
void inputTextData(char *textMessage, int maxLengthMessage);
//Rimuove da \n in poi, in una stringa
void removeCR(char *text);

//Funzioni di utilita'
//Stampa l'help di utilizzo
void usage();
//Stampa i valori di default utilizzati
void dumpDefaultValues();
void dumpDefaultKeysValues();
//Stampa intestazione iniziale
void printHeader();

//Setta la gestione di default dei segnali SIGINT, SIGTERM, SIGHUP, SIGQUIT
void defaultTerminationSignal();
//Ignora la gestione di default dei segnali SIGINT, SIGTERM, SIGHUP, SIGQUIT
void ignoreTerminationSignal();

//Funzione di test usata per segnali di interruzione
//void waitSec(int seconds);


#endif /* VOLATILEBULLETINBOARD_H_ */

