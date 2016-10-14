/*
 ============================================================================
 Name        : VolatileBulletinBoard.c
 Author      : Pier Paolo Ciarravano
 Version     : 1.0 - 15/09/2008
 Copyright   : GPL - General Public License
 Description : Bacheca Elettronica Volatile in C, Ansi-style
               Corso di Sistemi Operativi I - Prof. A.Santoro 2007-2008
               Ingegneria Informatica - Univ. "La Sapienza" Roma
 ============================================================================
 */

#include "VolatileBulletinBoard.h"

void setDefaultValue() {

	//Valori di default 
	SEMAPHORE_KEY = 2730; //0AAA
	SMEMORY_KEY = 3003; //0BBB
	MAX_READERS = 100;
	MAX_MESSAGES = 1000;

}

void runSimpleBulettinboardManager() {

	printHelp();

	char inputCommand[1024];
	char command;

	do
	{
		//Leggo Comando
		printf("Comando (h per aiuto):");
		//gets(inputCommand);
		fgets(inputCommand, 1024, stdin); //Piu' sicuro di gets per buffer overflow
		command = inputCommand[0];
		//printf("char command input: %c\n" , command);

		switch (command) {

				case 'l':
				case 'L':
					//printf("Lista messaggi presenti in bacheca\n");
					listBulettinboardManager();
					break;

				case 'r':
				case 'R':
					//printf("Lettura messaggio in bacheca\n");
					readBulettinboardManager();
					break;

				case 'a':
				case 'A':
					//printf("Spedizione nuovo messaggio in bacheca\n");
					insertBulettinboardManager();
					break;

				case 'd':
				case 'D':
					//printf("Cancellazione messaggio in bacheca\n");
					removeBulettinboardManager();
					break;

				case 'h':
				case 'H':
					printHelp();
					break;

				case 'q':
				case 'Q':
					printf("Uscita dalla gestione bacheca... Bye!!\n\n");
					detachSharedMemory();
					break;

				default:
					printf("Comando non riconosciuto! Utilizzare h per lista comandi\n\n");
		}

	}
	while((command!='q')&&(command!='Q'));

}

void listBulettinboardManager() {

	bulettinboard *listaMessaggi = list_bb();

	if (((*listaMessaggi).max_messages)==0)
	{
		printf("Nessun messaggio presente in bacheca!\n\n");
		return;
	}

	printf("Lista messaggi presenti in bacheca:\n\n");
	char inputCommand[1024];
	int i;
	for(i=0; i<((*listaMessaggi).max_messages); i++)
	{
		//Stampa riga messaggio sintetica senza testo e email
		char dateFormat[20];
		formatTime(dateFormat, &((*listaMessaggi).messages[i].insertDate));
		printf("Messaggio ID: %5d - Oggetto: \"%-40.40s\" - Username: %-10.10s - Data/ora: %s\n", (*listaMessaggi).messages[i].id, (*listaMessaggi).messages[i].title, (*listaMessaggi).messages[i].userLogin, dateFormat);

		//Gestione paginazione di 10 messaggi
		if ((((i+1) % 10) == 0) && (i<(((*listaMessaggi).max_messages))-1) )
		{
			printf("\nPremere invio per prossima pagina, oppure Q e invio per terminare...");
			fgets(inputCommand, 1024, stdin);
			if ((inputCommand[0]=='q')||(inputCommand[0]=='Q'))
			{
				break;
			}
			printf("\n");
		}
	}
	printf("\n");
	free(listaMessaggi);

}

void readBulettinboardManager() {

	int idRequest = -1;
	int sscanfResult;
	char inputCommand[1024];

	printf("Inserisci ID identificativo messaggio da leggere:");
	fgets(inputCommand, 1024, stdin);
	//uso sscanf in modo da evitare la memorizzazione di altre stringhe nel buffer di lettura
	sscanfResult = sscanf(inputCommand, "%d", &idRequest);
	if ((sscanfResult == 0)||(idRequest==-1))
	{
		printf("Valore inserito non corretto!!\n\n");
	}
	else
	{
		//printf("Leggo messaggio: %d\n\n", idRequest);
		message_bb readedMessage = readMessage_bb(idRequest);

		if (readedMessage.id == -1)
		{
			printf("Il messaggio con ID: %d non e' presente in bacheca!!\n\n", idRequest);
		}
		else
		{
			printf("\n----------------------------------------------------\n");
			printf("MESSAGGIO ID: %d\n", readedMessage.id);
			char dateFormat[20];
			formatTime(dateFormat, &(readedMessage.insertDate));
			printf("Oggetto: \"%s\"\n", readedMessage.title);
			printf("Testo: \"%s\"\n", readedMessage.text);
			printf("Username: %s\n", readedMessage.userLogin);
			printf("Email: %s\n", readedMessage.email);
			printf("Data/Ora inserimento: %s\n", dateFormat);
			printf("----------------------------------------------------\n\n");
		}
	}

}

void insertBulettinboardManager() {

	char inputText[1024];
	char titleMessage[100];
	char textMessage[2048];
	char emailMessage[100];

	//strcpy (titleMessage,"");
	//strcpy (textMessage,"");
	//strcpy (emailMessage,"");

	printf("Spedizione nuovo messaggio in bacheca:\n");

	//Input titolo
	printf("Inserisci l'oggetto del messaggio (invio per terminare, max.100 carat.):");
	fgets(inputText, 1024, stdin); //leggo piu' di 100 caratteri per evitare che questi rimangano nel buffer di lettura
	strncpy(titleMessage, inputText, 100);
	titleMessage[99] = '\0';
	removeCR(titleMessage);

	//Input testo
	printf("Inserisci il testo del messaggio (per terminare \".\" da solo in una riga, max.2048 carat.):\n");
	inputTextData(textMessage, 2048);

	//Input email
	printf("Inserisci indirizzo email (invio per terminare, max.100 carat.):");
	fgets(inputText, 1024, stdin); //leggo piu' di 100 caratteri per evitare che questi rimangano nel buffer di lettura
	strncpy(emailMessage, inputText, 100);
	emailMessage[99] = '\0';
	removeCR(emailMessage);

	//Stampa campi inseriti
	printf("\n------------ DATI MESSSAGGIO DA SPEDIRE: ------------\n");
	printf("Oggetto: \"%s\"\n", titleMessage);
	printf("Testo: \"%s\"\n", textMessage);
	printf("Email: %s\n\n", emailMessage);

	char inputCommand[1024];
	printf("Sei sicuro di voler inviare questo messaggio in bacheca (s/N):");
	fgets(inputCommand, 1024, stdin);
	if ((inputCommand[0]=='s')||(inputCommand[0]=='S'))
	{
		//Spedisco il messaggio in bacheca
		message_bb message = insertNewMessage(titleMessage, textMessage, emailMessage);

		if (message.id == -1)
		{
			printf("Non e' stato possibile inserire in bacheca il messaggio perche' la bacheca e' piena!!\n\n");
		}
		else
		{
			printf("Messaggio spedito correttamente in bacheca:");
			printf("\n----------------------------------------------------\n");
			printf("MESSAGGIO ID: %d\n", message.id);
			char dateFormat[20];
			formatTime(dateFormat, &(message.insertDate));
			printf("Oggetto: \"%s\"\n", message.title);
			printf("Testo: \"%s\"\n", message.text);
			printf("Username: %s\n", message.userLogin);
			printf("Email: %s\n", message.email);
			printf("Data/Ora inserimento: %s\n", dateFormat);
			printf("----------------------------------------------------\n\n");
		}
	}
	else
	{
		printf("Spedizione messaggio annullata\n\n");
	}

}


void removeBulettinboardManager() {

	int idRequest = -1;
	int sscanfResult;
	char inputCommand[1024];

	printf("Inserisci ID identificativo messaggio da cancellare:");
	fgets(inputCommand, 1024, stdin);
	//uso sscanf in modo da evitare la memorizzazione di altre stringhe nel buffer di lettura
	sscanfResult = sscanf(inputCommand, "%d", &idRequest);
	if ((sscanfResult == 0)||(idRequest==-1))
	{
		printf("Valore inserito non corretto!!\n\n");
	}
	else
	{
		//printf("Cancello messaggio: %d\n\n", idRequest);
		int removeResult = removeMessage_bb(idRequest);

		if (removeResult == -1)
		{
			printf("Il messaggio con ID: %d non e' presente in bacheca, pertanto non e' possibile cancellarlo!!\n\n", idRequest);
		}
		else if (removeResult == 0)
		{
			printf("Il messaggio con ID: %d e' stato cancellato correttamente dalla bacheca!!\n\n", idRequest);
		}
	}

}


void inputTextData(char *textMessage, int maxLengthMessage) {

	int lastIndex = maxLengthMessage-1;
	maxLengthMessage -= 1; //conteggio il terminatore di stringa
	strcpy (textMessage, "");
	char inputLine[4096];
	char *substr;

	//Esce con (invio).(invio) (in stile DATA SMTP)
	//     Enter text, end with "." on a line by itself
	do
	{
		fgets(inputLine, 4000, stdin);
		strncat (textMessage, inputLine, maxLengthMessage);
		maxLengthMessage -= strlen(inputLine);
	}
	while ( (strcmp (inputLine , ".\n") != 0) && (maxLengthMessage>0) );
	//while ( ((substr = strstr(textMessage, "\n.\n"))==(char *)0) && (maxLengthMessage>0) );

	//if (substr!=(char *)0)
	if ((substr = strstr(textMessage, "\n.\n"))!=(char *)0)
	{
		//rimuove ultima sequenza riga \n.\n, spostando semplicemente il terminatore di stringa nel puntatore alla sottostringa
		//strcpy ((substr+1), ""); //Elimina solo .\n
		strcpy (substr, "");
	}
	//Caso stringa vuota con il solo ".\n", nel caso l'utente non abbia inserito nulla
	if((strcmp (textMessage , ".\n") == 0))
	{
		strcpy (textMessage, "");
	}

	//Aggiungo comunque un terminatore di stringa
	textMessage[lastIndex] = '\0';

}

void removeCR(char *text) {

	char *substr;
	if ((substr = strstr(text, "\n"))!=(char *)0)
	{
		strcpy (substr, "");
	}

}

void printHelp() {

	printf("   Comandi:\n");
	printf("            l: Lista messaggi presenti in bacheca\n");
	printf("            r: Lettura messaggio in bacheca\n");
	printf("            a: Spedizione nuovo messaggio in bacheca\n");
	printf("            d: Cancellazione messaggio in bacheca\n");
	printf("            h: Visualizza questo aiuto comandi\n");
	printf("            q: Uscita dalla gestione bacheca\n");
	printf("\n");

}

int init_bb() {
	return init_bb_arg(SEMAPHORE_KEY, SMEMORY_KEY, MAX_READERS, MAX_MESSAGES);
}

int init_bb_arg(long int sk, int long mk, int mr, int mm) {

	//Ritorna:	0:ok
	//			-1:errore creazione memoria condivisa
	//			-2:errore creazione semafori


	//AZIONI PER MEMORIA CONDIVISA

	//Creazione memoria condivisa
	ID_SMEMORY = shmget(mk, 2*sizeof(int) + mm*sizeof(message_bb), IPC_CREAT | IPC_EXCL | 0666);
	if (ID_SMEMORY == -1)
		return -1;

	//Collegamento memoria condivisa allo spazio di indirizzamento del processo in esecuzione
	MBULLETTINBOARD = shmat(ID_SMEMORY, NULL , SHM_R | SHM_W);
	if ( MBULLETTINBOARD == (bulettinboard*)-1 )
		return -1;

	//Inizializzazione struttura puntata dalla memoria condivisa
	(*MBULLETTINBOARD).max_messages = mm;
	(*MBULLETTINBOARD).idSequence = 0; //Variabile per assegnare un nuovo id al messaggio
	int i;
	for(i=0; i<mm; i++)
	{
		(*MBULLETTINBOARD).messages[i].id = 0;
		(*MBULLETTINBOARD).messages[i].isFree = 1;
		//strcpy((*MBULLETTINBOARD).messages[i].title, "");
	}


	//AZIONI PER SEMAFORI

	//Creazione array semafori: 0 semaforo READERS, 1 semaforo WRITERS
	ID_SEMAPHORE_RW = semget(sk, 2, IPC_CREAT | IPC_EXCL | 0666);
	if (ID_SEMAPHORE_RW == -1)
			return -2;

	//Inizializzazione semafori
	// Semaforo con indice 0 per READERS: inizializzato con MAX_READERS
	// Semaforo con indice 1 per WRITERS: semaforo di tipo mutex, inizializzato con 1
	ushort semaphoresInitValues[2] = {MAX_READERS, 1};
	int semctl_result = semctl(ID_SEMAPHORE_RW, 0, SETALL, semaphoresInitValues );
	if (semctl_result == -1)
		return -2;

	//Messaggi di output gestiti da chi ha chiamato questa funzione, tramite il parametro di ritorno
	return 0;
}


int destroy_bb() {
	return destroy_bb_arg(SEMAPHORE_KEY, SMEMORY_KEY);
}

int destroy_bb_arg(long int sk, int long mk) {

	//Ritorna maschera bit	1:ok
	//						2:errore rimozione memoria condivisa
	//						4:errore rimozione semafori

	int result = 1;

	//Rimozione memoria condivisa
	ID_SMEMORY = shmget(mk, 0, 0666); //il parametro 0666 viene ignorato
	if (ID_SMEMORY == -1) { //Specificato errore ENOENT in error.h
		//La memoria condivisa specificata dalla chiave non esiste
		//printf("La memoria condivisa specificata dalla chiave: %ld, non esiste!\n", mk);
		result += 2;
	}
	else
	{
		//Lancio comando rimozione memoria condivisa
		int shmctl_result = shmctl(ID_SMEMORY, IPC_RMID, NULL);
		if (shmctl_result == -1)
			result += 2;
		//else
		//	printf("La memoria condivisa specificata dalla chiave: %ld, e' stata rimossa correttamente!\n", mk);
	}

	//Rimozione array semafori
	ID_SEMAPHORE_RW = semget(sk, 0, 0666); //il parametro 0666 viene ignorato
	if (ID_SEMAPHORE_RW == -1) {
		result += 4;
	}
	else
	{
		//Lancio comando rimozione array semafori
		int semctl_result = semctl(ID_SEMAPHORE_RW, 0, IPC_RMID, NULL);
		if (semctl_result == -1)
			result += 4;
		//else
		//	printf("L'array di semafori specificato dalla chiave: %ld, e' stata rimosso correttamente!\n", sk);
	}

	//Messaggi di output gestiti da chi ha chiamato questa funzione, tramite il parametro di ritorno
	return result;
}

//utilizza ID_SMEMORY, ID_SEMAPHORE_RW per rimuovere memoria condivisa e semafori
//FUNZIONE NON USATA MA IMPLEMENTATA COMUNQUE
int destroyUsingId_bb() {

	//Ritorna maschera bit	1:ok
	//						2:errore rimozione memoria condivisa
	//						4:errore rimozione semafori

	int result = 1;

	//Rimozione memoria condivisa
	int shmctl_result = shmctl(ID_SMEMORY, IPC_RMID, NULL);
	if (shmctl_result == -1)
		result += 2;

	//Rimozione array semafori
	int semctl_result = semctl(ID_SEMAPHORE_RW, 0, IPC_RMID, NULL);
	if (semctl_result == -1)
		result += 4;

	//Messaggi di output gestiti da chi ha chiamato questa funzione, tramite il parametro di ritorno
	return result;
}


int open_bb() {
	return open_bb_arg(SEMAPHORE_KEY, SMEMORY_KEY);
}

int open_bb_arg(long int sk, int long mk) {

	//Ritorna:	0:ok
	//			-1:errore apertura identificatore memoria condivisa
	//			-2:errore apertura identificatore semaforo
	//			-3:errore collegamento memoria condivisa

	//apertura identificatore semaforo
	ID_SEMAPHORE_RW = semget(sk, 0, 0666); //il parametro 0666 viene ignorato
	if (ID_SEMAPHORE_RW == -1) {
		return -2;
	}

	//apertura identificatore memoria condivisa
	ID_SMEMORY = shmget(mk, 0, 0666); //il parametro 0666 viene ignorato
	if (ID_SMEMORY == -1) {
		return -1;
	}

	//Colegamento memoria condivisa
	MBULLETTINBOARD = shmat(ID_SMEMORY, NULL , SHM_R | SHM_W);
	if ( MBULLETTINBOARD == (bulettinboard*)-1 )
		return -3;

	//Inizializzazione MAX_MESSAGES
	MAX_MESSAGES = (*MBULLETTINBOARD).max_messages;

	return 0;
}


int detachSharedMemory() {
	return shmdt(MBULLETTINBOARD);
}


//utilizzare free() dopo aver usato la struttura ritornata
bulettinboard *list_bb() {

	ignoreTerminationSignal();

	//Decremento (-1) semaforo 0 READERS
	struct sembuf waitReader[1] = { {0, -1, 0} };
	int retSemop = semop(ID_SEMAPHORE_RW, waitReader, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 0: -1\n");
		//exit(-1);
	}

	//Conto numero di messaggi validi presenti nell'array (*MBULLETTINBOARD).messages[]
	int num_valid_messages = 0;
	int i;
	for(i=0; i<MAX_MESSAGES; i++)
	{
		if ((*MBULLETTINBOARD).messages[i].isFree == 0)
		{
			num_valid_messages++;
		}
	}
	//printf("num_valid_messages: %d\n", num_valid_messages);

	//Alloco struttura result da ritornare
	bulettinboard *result;
	result = malloc(2*sizeof(int) + num_valid_messages*sizeof(message_bb));
	if (result == NULL) {
		printf("Errore malloc struttura result per funzione list_bb\n");
		//exit(-1);
	}

	//Popolo struttura result da ritornare copiando i campi dei messaggi validi
	(*result).max_messages = num_valid_messages;
	(*result).idSequence = -1; //Variabile non utilizzata nella struttura ritornata
	int j = 0;
	for(i=0; i<MAX_MESSAGES; i++)
	{
		if ((*MBULLETTINBOARD).messages[i].isFree == 0)
		{
			(*result).messages[j].id = (*MBULLETTINBOARD).messages[i].id;
			(*result).messages[j].isFree = 0;
			strcpy((*result).messages[j].title, (*MBULLETTINBOARD).messages[i].title);
			strcpy((*result).messages[j].text, (*MBULLETTINBOARD).messages[i].text);
			strcpy((*result).messages[j].userLogin, (*MBULLETTINBOARD).messages[i].userLogin);
			strcpy((*result).messages[j].email, (*MBULLETTINBOARD).messages[i].email);
			(*result).messages[j].insertDate = (*MBULLETTINBOARD).messages[i].insertDate;
			j++;
		}
	}

	//Incremento (+1) semaforo 0 READERS
	struct sembuf signalReader[1] = { {0, +1, 0} };
	retSemop = semop(ID_SEMAPHORE_RW, signalReader, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 0: +1\n");
		//exit(-1);
	}

	defaultTerminationSignal();

	return result;
}


message_bb readMessage_bb(int idMessage) {

	ignoreTerminationSignal();

	//Decremento (-1) semaforo 0 READERS
	struct sembuf waitReader[1] = { {0, -1, 0} };
	int retSemop = semop(ID_SEMAPHORE_RW, waitReader, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 0: -1\n");
		//exit(-1);
	}

	message_bb result;
	result.id = -1;

	int i;
	for(i=0; i<MAX_MESSAGES; i++)
	{
		//Cerco il messaggio con id==idMessage
		if ( ( (*MBULLETTINBOARD).messages[i].isFree == 0 ) && ((*MBULLETTINBOARD).messages[i].id == idMessage) )
		{
			result.id = (*MBULLETTINBOARD).messages[i].id;
			result.isFree = 0;
			strcpy(result.title, (*MBULLETTINBOARD).messages[i].title);
			strcpy(result.text, (*MBULLETTINBOARD).messages[i].text);
			strcpy(result.userLogin, (*MBULLETTINBOARD).messages[i].userLogin);
			strcpy(result.email, (*MBULLETTINBOARD).messages[i].email);
			result.insertDate = (*MBULLETTINBOARD).messages[i].insertDate;
			break; //esco dal ciclo
		}
	}

	//Incremento (+1) semaforo 0 READERS
	struct sembuf signalReader[1] = { {0, +1, 0} };
	retSemop = semop(ID_SEMAPHORE_RW, signalReader, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 0: +1\n");
		//exit(-1);
	}

	defaultTerminationSignal();

	return result;
}


int writeMessage_bb(message_bb message) {

	ignoreTerminationSignal();

	//Decremento (-1) semaforo 1 WRITERS (mutex)
	struct sembuf waitWriter[1] = { {1, -1, 0} };
	int retSemop = semop(ID_SEMAPHORE_RW, waitWriter, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 1: -1\n");
		//exit(-1);
	}

	//Blocco tutti i lettori
	int r;
	for (r = 0; r < MAX_READERS; ++r) {
		//Decremento (-1) semaforo 0 READERS
		struct sembuf waitReader[1] = { {0, -1, 0} };
		int retSemop = semop(ID_SEMAPHORE_RW, waitReader, 1);
		if (retSemop == -1) {
			printf("Errore semop semaforo 0: -1\n");
			//exit(-1);
		}
	}

	//Usata per test segnali di interruzione
	//waitSec(10);

	//Opero l'inserimento del messaggio nella memoria condivisa
	int idResult = -1;
	//Cerco la prima posizione libera (free==1) nell'array messages della struttura puntata da *MBULLETTINBOARD
	int i;
	for(i=0; i<MAX_MESSAGES; i++)
	{
		if (  (*MBULLETTINBOARD).messages[i].isFree == 1 )
		{
			//Posizione libera con indice i trovata nell'array: (*MBULLETTINBOARD).messages[i]
			//Copio message in (*MBULLETTINBOARD).messages[i]
			(*MBULLETTINBOARD).messages[i].id = ++((*MBULLETTINBOARD).idSequence); //Assegno un id univoco al messaggio
			idResult = (*MBULLETTINBOARD).messages[i].id; //popolo il valore da ritornare
			(*MBULLETTINBOARD).messages[i].isFree = 0;
			strcpy((*MBULLETTINBOARD).messages[i].title, message.title);
			strcpy((*MBULLETTINBOARD).messages[i].text, message.text);
			strcpy((*MBULLETTINBOARD).messages[i].userLogin, message.userLogin);
			strcpy((*MBULLETTINBOARD).messages[i].email, message.email);
			(*MBULLETTINBOARD).messages[i].insertDate = message.insertDate;
			break; //esco dal ciclo
		}
	}


	//Sblocco tutti i lettori
	for (r = 0; r < MAX_READERS; ++r) {
		//Incremento (+1) semaforo 0 READERS
		struct sembuf signalReader[1] = { {0, +1, 0} };
		retSemop = semop(ID_SEMAPHORE_RW, signalReader, 1);
		if (retSemop == -1) {
			printf("Errore semop semaforo 0: +1\n");
			//exit(-1);
		}
	}

	//Incremento (+1) semaforo 1 WRITERS (mutex)
	struct sembuf signalWriter[1] = { {1, +1, 0} };
	retSemop = semop(ID_SEMAPHORE_RW, signalWriter, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 1: +1\n");
		//exit(-1);
	}

	defaultTerminationSignal();

	return idResult;
}


int removeMessage_bb(int idMessage) {

	ignoreTerminationSignal();

	//Decremento (-1) semaforo 1 WRITERS (mutex)
	struct sembuf waitWriter[1] = { {1, -1, 0} };
	int retSemop = semop(ID_SEMAPHORE_RW, waitWriter, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 1: -1\n");
		//exit(-1);
	}

	//Blocco tutti i lettori
	int r;
	for (r = 0; r < MAX_READERS; ++r) {
		//Decremento (-1) semaforo 0 READERS
		struct sembuf waitReader[1] = { {0, -1, 0} };
		int retSemop = semop(ID_SEMAPHORE_RW, waitReader, 1);
		if (retSemop == -1) {
			printf("Errore semop semaforo 0: -1\n");
			//exit(-1);
		}
	}


	//Opero la rimozione del messaggio nella memoria condivisa
	int idResult = -1;
	int i;
	for(i=0; i<MAX_MESSAGES; i++)
	{
		//Cerco il messaggio con id==idMessage
		if ( ( (*MBULLETTINBOARD).messages[i].isFree == 0 ) && ((*MBULLETTINBOARD).messages[i].id == idMessage) )
		{
			(*MBULLETTINBOARD).messages[i].isFree = 1; //Operazione necessaria solo se il messaggio e' l'ultimo dell'array o se non si vuole effettuare lo spostamento dei messaggi seguenti nell'array
			//Scalo di una posizione tutti gli elementi dell'array, se i non e' l'ultimo elemento
			if (i<(MAX_MESSAGES-1))
			{
				int j;
				for(j=(i+1); j<MAX_MESSAGES; j++)
				{
					//Copia elemento (*MBULLETTINBOARD).messages[j] in elemento (*MBULLETTINBOARD).messages[j-1]
					(*MBULLETTINBOARD).messages[j-1].id = (*MBULLETTINBOARD).messages[j].id;
					(*MBULLETTINBOARD).messages[j-1].isFree = (*MBULLETTINBOARD).messages[j].isFree;
					strcpy((*MBULLETTINBOARD).messages[j-1].title, (*MBULLETTINBOARD).messages[j].title);
					strcpy((*MBULLETTINBOARD).messages[j-1].text, (*MBULLETTINBOARD).messages[j].text);
					strcpy((*MBULLETTINBOARD).messages[j-1].userLogin, (*MBULLETTINBOARD).messages[j].userLogin);
					strcpy((*MBULLETTINBOARD).messages[j-1].email, (*MBULLETTINBOARD).messages[j].email);
					(*MBULLETTINBOARD).messages[j-1].insertDate = (*MBULLETTINBOARD).messages[j].insertDate;
					//printf("copiato elemento: %d-->%d\n", j, j-1 );
				}
			}
			idResult = 0;
			break; //esco dal ciclo
		}
	}


	//Sblocco tutti i lettori
	for (r = 0; r < MAX_READERS; ++r) {
		//Incremento (+1) semaforo 0 READERS
		struct sembuf signalReader[1] = { {0, +1, 0} };
		retSemop = semop(ID_SEMAPHORE_RW, signalReader, 1);
		if (retSemop == -1) {
			printf("Errore semop semaforo 0: +1\n");
			//exit(-1);
		}
	}

	//Incremento (+1) semaforo 1 WRITERS (mutex)
	struct sembuf signalWriter[1] = { {1, +1, 0} };
	retSemop = semop(ID_SEMAPHORE_RW, signalWriter, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 1: +1\n");
		//exit(-1);
	}

	defaultTerminationSignal();

	return idResult;
}


void setUserLogin(char *userLogin, int size) {

	//non ritorna correttamente l'utente che ha lanciato il processo, ma ritorna i nome utente che aperto la finestra della shell
	//getlogin_r(userLogin, size);

	//prelevo correttamente il nome utente che ha lanciato il processo
	uid_t uid = geteuid ();
	struct passwd *pw = getpwuid (uid);
	if (pw)
	{
		char *user = pw->pw_name;
		strncpy(userLogin, user, size);
	}
	else
	{
		strcpy(userLogin, ".");
	}

	//printf("Username utente che ha lanciato il processo: %s\n", userLogin);
}


void formatTime(char *formatDateTime, struct tm *dateTime) {

	strftime (formatDateTime,20,"%d/%m/%Y %H:%M:%S", dateTime);

}


void setDateTime(struct tm *dateTime) {

	time_t rawtime;
	time ( &rawtime );
	*dateTime = *(localtime ( &rawtime ));

}


message_bb insertNewMessage(char *title, char *text, char *email) {

	//Creo il messaggio e lo popolo
	message_bb result;
	result.id = -1;
	result.isFree = 0;
	strncpy(result.title, title, 100);
	strncpy(result.text, text, 2048);
	setUserLogin(result.userLogin, 20);
	strncpy(result.email, email, 100);
	setDateTime(&(result.insertDate));

	//Inserisco il messaggio nella memoria condivisa
	int idMessage = writeMessage_bb(result);
	result.id = idMessage;

	return result;
}


int updateMessage_bb(message_bb message) {

	ignoreTerminationSignal();

	//Decremento (-1) semaforo 1 WRITERS (mutex)
	struct sembuf waitWriter[1] = { {1, -1, 0} };
	int retSemop = semop(ID_SEMAPHORE_RW, waitWriter, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 1: -1\n");
		//exit(-1);
	}

	//Blocco tutti i lettori
	int r;
	for (r = 0; r < MAX_READERS; ++r) {
		//Decremento (-1) semaforo 0 READERS
		struct sembuf waitReader[1] = { {0, -1, 0} };
		int retSemop = semop(ID_SEMAPHORE_RW, waitReader, 1);
		if (retSemop == -1) {
			printf("Errore semop semaforo 0: -1\n");
			//exit(-1);
		}
	}


	//Opero la rimozione del messaggio nella memoria condivisa
	int idResult = -1;
	int i;
	for(i=0; i<MAX_MESSAGES; i++)
	{
		//Cerco il messaggio con id==message.id
		if ( ( (*MBULLETTINBOARD).messages[i].isFree == 0 ) && ((*MBULLETTINBOARD).messages[i].id == message.id) )
		{
			//modifico il messaggio
			strcpy((*MBULLETTINBOARD).messages[i].title, message.title);
			strcpy((*MBULLETTINBOARD).messages[i].text, message.text);
			strcpy((*MBULLETTINBOARD).messages[i].email, message.email);
			(*MBULLETTINBOARD).messages[i].insertDate = message.insertDate;

			idResult = message.id;
			break; //esco dal ciclo
		}
	}


	//Sblocco tutti i lettori
	for (r = 0; r < MAX_READERS; ++r) {
		//Incremento (+1) semaforo 0 READERS
		struct sembuf signalReader[1] = { {0, +1, 0} };
		retSemop = semop(ID_SEMAPHORE_RW, signalReader, 1);
		if (retSemop == -1) {
			printf("Errore semop semaforo 0: +1\n");
			//exit(-1);
		}
	}

	//Incremento (+1) semaforo 1 WRITERS (mutex)
	struct sembuf signalWriter[1] = { {1, +1, 0} };
	retSemop = semop(ID_SEMAPHORE_RW, signalWriter, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 1: +1\n");
		//exit(-1);
	}

	defaultTerminationSignal();

	return idResult;
}

int updateMessage(int idMessage, char *title, char *text, char *email) {

	//Leggo il messaggio
	message_bb updateMessage = readMessage_bb(idMessage);
	if (updateMessage.id == -1)
	{
		return -1;
	}

	//Controllo non richiesto:
	// Possibile modifica solo se l'utente che modifica il messaggio e' lo stesso di quello che lo ha creato
	char actualUserLogin[20];
	setUserLogin(actualUserLogin, 20);
	if (strcmp (updateMessage.userLogin , actualUserLogin) != 0) {
		printf("Errore modifica: owners messaggio differenti: %s != %s\n", updateMessage.userLogin , actualUserLogin);
		return -2;
	}

	//setta la data di inserimento differente
	setDateTime(&(updateMessage.insertDate));

	//Setto i campi
	strcpy(updateMessage.title, title);
	strcpy(updateMessage.text, text);
	strcpy(updateMessage.email, email);

	//Salvo il messaggio
	int idResultUpdate = updateMessage_bb(updateMessage);

	return idResultUpdate;
}


int fileBackupMessage_bb(char *filename) {

	ignoreTerminationSignal();

	//Decremento (-1) semaforo 0 READERS
	struct sembuf waitReader[1] = { {0, -1, 0} };
	int retSemop = semop(ID_SEMAPHORE_RW, waitReader, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 0: -1\n");
		//exit(-1);
	}


	int result;
	result = -1;
	//Salvataggio su file leggendo la memoria condivisa
	int dsFileBackup;
	dsFileBackup = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (dsFileBackup == -1) {
		printf("ERRORE APERTURA FILE IN SCRITTURA: %s!!\n", filename);
	}
	else
	{
		int nread = write(dsFileBackup, MBULLETTINBOARD, (2*sizeof(int) + MAX_MESSAGES*sizeof(message_bb)) );
		if (nread==(2*sizeof(int) + MAX_MESSAGES*sizeof(message_bb))) {
			result = 0;
		}
	}
	close(dsFileBackup);


	//Incremento (+1) semaforo 0 READERS
	struct sembuf signalReader[1] = { {0, +1, 0} };
	retSemop = semop(ID_SEMAPHORE_RW, signalReader, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 0: +1\n");
		//exit(-1);
	}

	defaultTerminationSignal();

	return result;
}

int fileRestoreMessage_bb(char *filename) {

	ignoreTerminationSignal();

	//Decremento (-1) semaforo 1 WRITERS (mutex)
	struct sembuf waitWriter[1] = { {1, -1, 0} };
	int retSemop = semop(ID_SEMAPHORE_RW, waitWriter, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 1: -1\n");
		//exit(-1);
	}

	//Blocco tutti i lettori
	int r;
	for (r = 0; r < MAX_READERS; ++r) {
		//Decremento (-1) semaforo 0 READERS
		struct sembuf waitReader[1] = { {0, -1, 0} };
		int retSemop = semop(ID_SEMAPHORE_RW, waitReader, 1);
		if (retSemop == -1) {
			printf("Errore semop semaforo 0: -1\n");
			//exit(-1);
		}
	}


	int result;
	result = -1;
	//Restore del file scrivendo sulla memoria condivisa
	int dsFileRestore = open(filename, O_RDONLY);
	if (dsFileRestore == -1) {
		printf("ERRORE APERTURA FILE IN LETTURA: %s!!\n", filename);
	}
	else
	{
		int nwrite = read(dsFileRestore, MBULLETTINBOARD, (2*sizeof(int) + MAX_MESSAGES*sizeof(message_bb)) );
		if (nwrite==(2*sizeof(int) + MAX_MESSAGES*sizeof(message_bb))) {
			result = 0;
		}
	}
	close(dsFileRestore);


	//Sblocco tutti i lettori
	for (r = 0; r < MAX_READERS; ++r) {
		//Incremento (+1) semaforo 0 READERS
		struct sembuf signalReader[1] = { {0, +1, 0} };
		retSemop = semop(ID_SEMAPHORE_RW, signalReader, 1);
		if (retSemop == -1) {
			printf("Errore semop semaforo 0: +1\n");
			//exit(-1);
		}
	}

	//Incremento (+1) semaforo 1 WRITERS (mutex)
	struct sembuf signalWriter[1] = { {1, +1, 0} };
	retSemop = semop(ID_SEMAPHORE_RW, signalWriter, 1);
	if (retSemop == -1) {
		printf("Errore semop semaforo 1: +1\n");
		//exit(-1);
	}

	defaultTerminationSignal();

	return result;
}

void printMessage(message_bb message) {

	//Formato la insertDate
	char dateFormat[20];
	formatTime(dateFormat, &(message.insertDate));

	printf("Message id: %d --> %d, \"%s\", \"%s\", %s, %s, %s\n", message.id, message.isFree, message.title, message.text, message.userLogin, message.email, dateFormat);

}

int parseMainOptions_bb(int argc, char *argv[]) {

	//Ritorna maschera bit 	1: lancia programma di gestione della bacheca
	//						2: Creazione Bacheca
	//						4: Rimozione Bacheca
	//						8: Backup su file Bacheca
	//						16: Restore da file Bacheca
	//						32: Test (NON IMPLEMENTATO)

	int result = 1;
	int optionsResult = 0;
	while ((optionsResult = getopt(argc, argv, "hads:m:r:n:b:l:")) != -1) {
		switch (optionsResult) {

		case 'h':
			usage();
			exit(-1);
			break;
		case 'a':
			//printf("Creazione Bacheca: memoria condivisa e semafori...\n");
			//Setto azione: creazione
			result += 2;
			break;
		case 'd':
			//printf("Rimozione Bacheca: memoria condivisa e semafori...\n");
			//Setto azione: rimozione
			result += 4;
			break;
		case 's':
			SEMAPHORE_KEY = strtol(optarg, NULL, 10);
			if (SEMAPHORE_KEY == 0) {
				printf("Errore: Specificare per -s un valore numerico o diverso da 0!\n");
				exit(-1);
			}
			//printf("s: %ld\n", SEMAPHORE_KEY);
			break;
		case 'm':
			SMEMORY_KEY = strtol(optarg, NULL, 10);
			if (SMEMORY_KEY == 0) {
				printf("Errore: Specificare per -m un valore numerico o diverso da 0!\n");
				exit(-1);
			}
			//printf("m: %ld\n", SMEMORY_KEY);
			break;
		case 'r':
			MAX_READERS = atoi(optarg);
			if (MAX_READERS == 0) {
				printf("Errore: Specificare per -r un valore numerico o diverso da 0!\n");
				exit(-1);
			}
			//printf("r: %d\n", MAX_READERS);
			break;
		case 'n':
			MAX_MESSAGES = atoi(optarg);
			if (MAX_MESSAGES == 0) {
				printf("Errore: Specificare per -n un valore numerico o diverso da 0!\n");
				exit(-1);
			}
			//printf("n: %d\n", MAX_MESSAGES);
			break;
		case 'b':
			FILE_MBULLETTINBOARD = optarg;
			//printf("b: %s\n", FILE_MBULLETTINBOARD);
			result += 8;
			break;
		case 'l':
			FILE_MBULLETTINBOARD = optarg;
			//printf("l: %s\n", FILE_MBULLETTINBOARD);
			result += 16;
			break;
		//case 't':
		//	//Setto azione: TEST
		//	result += 32;
		//	break;
		case '?':
			printf("Digitare `VolatileBulletinBoard -h' per maggiori informazioni\n");
			exit(-1);
			break;
		//default:
			//printf("DEFAULT!!!\n");
		}

	}
	return result;
}

/*
//Implementato in maniera differente nelle versioni VolatileBulletinBoard.c e VolatileBulletinBoardUsers.c
void usage() {

	printf("Usare: VolatileBulletinBoard [-a|-d|-b|-l|-t] [OPZIONI]\nOPZIONI:\n");
	printf("	-a       Creazione Bacheca: memoria condivisa e semafori\n");
	printf("	-d       Rimozione Bacheca: memoria condivisa e semafori\n");
	printf("	-s N     setta chiave identificazione semafori\n");
	printf("	-m N     setta chiave identificazione memoria condivisa\n");
	printf("	-r N     massimo numero di utenti readers\n");
	printf("	-n N     massimo numero di messaggi della bacheca\n");
	printf("	-b file  backup memoria condivisa su file\n");
	printf("	-l file  carica memoria condivisa da file backup\n");
	//printf("	-t       test (NON IMPLEMENTATO IN QUESTA VERSIONE)\n");
	printf("	-h       Visualizza questo help e termina il programma\n");
	printf("\n");
	printf("Non usando i parametri -a, -d, -b, -l viene eseguito il programma di gestione della bacheca.\n");
	printf("Non usando i parametri -s, -m, -r, -n vengono usati i valori di default.\n");
	printf("I parametri -r e -n vengono usati solamente insieme al parametro -a nella creazione bacheca, altrimenti vengono ignorati.\n");
	printf("\n");

}
*/

void dumpDefaultValues() {

	printf("Chiave identificazione semafori: %ld (0X%08X)\n", SEMAPHORE_KEY, (unsigned int)SEMAPHORE_KEY);
	printf("Chiave identificazione memoria condivisa: %ld (0X%08X)\n", SMEMORY_KEY, (unsigned int)SMEMORY_KEY);
	printf("Massimo numero di utenti readers: %d\n", MAX_READERS);
	printf("Massimo numero di messaggi della bacheca: %d\n", MAX_MESSAGES);
}

void dumpDefaultKeysValues() {

	printf("Chiave identificazione semafori: %ld (0X%08X)\n", SEMAPHORE_KEY, (unsigned int)SEMAPHORE_KEY);
	printf("Chiave identificazione memoria condivisa: %ld (0X%08X)\n", SMEMORY_KEY, (unsigned int)SMEMORY_KEY);
}

void printHeader() {

	printf("Bacheca Elettronica Volatile di Pier Paolo Ciarravano\n");
	printf("Corso di Sistemi Operativi I - Prof. A.Santoro 2007-2008\nIngegneria Informatica - Univ. \"La Sapienza\" Roma\n");
	printf("Versione: 1.0 - 15/09/2008 \n\n");

}


void defaultTerminationSignal() {
	//Setta comportamento di default dei segnali di interruzione causati dall'utente
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGHUP, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void ignoreTerminationSignal() {
	//Ignora comportamento di default dei segnali di interruzione causati dall'utente
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}


//Usata per test segnali di interruzione
//void waitSec(int seconds){
//	clock_t endwait;
//	endwait=clock()+seconds*CLOCKS_PER_SEC;
//	while (clock()<endwait);
//}

