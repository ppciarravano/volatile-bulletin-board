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

int main(int argc, char *argv[]) {

	printHeader();
	setDefaultValue();
	int parseMainResult = parseMainOptions_bb(argc, argv);
	//printf("parseMainResult: %d\n", parseMainResult);

	//Ignora comportamento di default per i segnali di interruzione utente
	ignoreTerminationSignal();

	if ( (( ((parseMainResult & 2)==2) ? 1 : 0 ) +
			( ((parseMainResult & 4)==4) ? 1 : 0 ) +
			( ((parseMainResult & 8)==8) ? 1 : 0 ) +
			( ((parseMainResult & 16)==16) ? 1 : 0 )) >1 ) {
			//( ((parseMainResult & 32)==32) ? 1 : 0 )) >1 ) { //Per test (non implementato)
		//Specificato piu' di un paramentro tra -a, -d, -b, -l
		usage();
		printf("Errore: Specificare un solo parametro tra -a, -d, -b, -l oppure usare senza nessun parametro!\n");
		exit(-1);
	}
	else if ( ((parseMainResult & 2)==2) )	{
		//Creazione Bacheca
		printf("Creazione Bacheca: memoria condivisa e semafori...\n\n");
		dumpDefaultValues();
		int initbbResult = init_bb();
		//printf("initbbResult: %d\n", initbbResult);
		if (initbbResult==0)
		{
			printf("Memoria condivisa e samafori creati correttamente!\n");
		}
		else if (initbbResult==-1)
		{
			printf("ERRORE CREAZIONE MEMORIA CONDIVISA!!!\n");
		}
		else if (initbbResult==-2)
		{
			printf("ERRORE CREAZIONE SEMAFORI!!!\n");
		}

	}
	else if ( ((parseMainResult & 4)==4) )	{
		//Rimozione Bacheca
		printf("Rimozione Bacheca: memoria condivisa e semafori...\n\n");
		dumpDefaultKeysValues();
		int destroybbResult = destroy_bb();
		//printf("destroybbResult: %d\n", destroybbResult);
		if (destroybbResult==1)
		{
			printf("Memoria condivisa e samafori rimossi correttamente!\n");
		}
		if (destroybbResult & 2 )
		{
			printf("ERRORE RIMOZIONE MEMORIA CONDIVISA!!!\n");
		}
		else if (destroybbResult!=1)
		{
			printf("Memoria condivisa rimossa correttamente!\n");
		}
		if (destroybbResult & 4)
		{
			printf("ERRORE RIMOZIONE SEMAFORI!!!\n");
		}
		else if (destroybbResult!=1)
		{
			printf("Semafori rimossi correttamente!\n");
		}

	}
	//else if ( ((parseMainResult & 32)==32) )	{
	//	//Test
	//	printf("Test Multithread...NON IMPLEMENTATO IN QUESTA VERSIONE!!\n\n");
	//	usage();
	//	exit(-1);
	//}
	else if ( ((parseMainResult & 8)==8) )	{
		//Backup File
		printf("Backup su file Bacheca...\n\n");
		dumpDefaultKeysValues();
		int openbbResult = open_bb();
		if (openbbResult==0)
		{
			printf("Memoria condivisa e samafori aperti correttamente!\n\n");
		}
		else if (openbbResult==-1)
		{
			printf("ERRORE APERTURA MEMORIA CONDIVISA!!!\n");
			exit(-1);
		}
		else if (openbbResult==-2)
		{
			printf("ERRORE APERTURA SEMAFORI!!!\n");
			exit(-1);
		}
		else if (openbbResult==-3)
		{
			printf("ERRORE COLLEGAMENTO MEMORIA CONDIVISA!!!\n");
			exit(-1);
		}

		//Backup su file
		int resultBackup = fileBackupMessage_bb(FILE_MBULLETTINBOARD);
		if (resultBackup==-1)
		{
			printf("BACKUP SU FILE \"%s\" FALLITO!!\n\n", FILE_MBULLETTINBOARD);
		}
		else
		{
			printf("BACKUP SU FILE \"%s\" AVVENUTO CON SUCCESSO!!\n\n", FILE_MBULLETTINBOARD);
		}

		detachSharedMemory();

	}
	else if ( ((parseMainResult & 16)==16) )	{
		//Caricamento da File
		printf("Caricamento da file Bacheca...\n\n");
		dumpDefaultKeysValues();
		int openbbResult = open_bb();
		if (openbbResult==0)
		{
			printf("Memoria condivisa e samafori aperti correttamente!\n\n");
		}
		else if (openbbResult==-1)
		{
			printf("ERRORE APERTURA MEMORIA CONDIVISA!!!\n");
			exit(-1);
		}
		else if (openbbResult==-2)
		{
			printf("ERRORE APERTURA SEMAFORI!!!\n");
			exit(-1);
		}
		else if (openbbResult==-3)
		{
			printf("ERRORE COLLEGAMENTO MEMORIA CONDIVISA!!!\n");
			exit(-1);
		}

		//Backup su file
		int resultRestore = fileRestoreMessage_bb(FILE_MBULLETTINBOARD);
		if (resultRestore==-1)
		{
			printf("CARICAMENTO DA FILE \"%s\" FALLITO!!\n\n", FILE_MBULLETTINBOARD);
		}
		else
		{
			printf("CARICAMENTO DA \"%s\" AVVENUTO CON SUCCESSO!!\n\n", FILE_MBULLETTINBOARD);
		}

		detachSharedMemory();

	}
	else if ( ((parseMainResult & 1)==1) )	{
		//Gestione Bacheca
		printf("Gestione Bacheca...\n\n");
		dumpDefaultKeysValues();
		int openbbResult = open_bb();
		//printf("openbbResult: %d\n", openbbResult);
		if (openbbResult==0)
		{
			printf("Memoria condivisa e samafori aperti correttamente!\n\n");
		}
		else if (openbbResult==-1)
		{
			printf("ERRORE APERTURA MEMORIA CONDIVISA!!!\n");
			exit(-1);
		}
		else if (openbbResult==-2)
		{
			printf("ERRORE APERTURA SEMAFORI!!!\n");
			exit(-1);
		}
		else if (openbbResult==-3)
		{
			printf("ERRORE COLLEGAMENTO MEMORIA CONDIVISA!!!\n");
			exit(-1);
		}

		//Setta comportamento di default per i segnali di interruzione utente
		defaultTerminationSignal();

		//Lancio la gestione della bacheca
		runSimpleBulettinboardManager();

	}

	return EXIT_SUCCESS;
}

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

