/*
 ============================================================================
 Name        : VolatileBulletinBoardUsers.c
 Author      : Pier Paolo Ciarravano
 Version     : 1.0 - 15/09/2008
 Copyright   : GPL - General Public License
 Description : Bacheca Elettronica Volatile in C, Ansi-style
               Corso di Sistemi Operativi I - Prof. A.Santoro 2007-2008
               Ingegneria Informatica - Univ. "La Sapienza" Roma

               VERSIONE PER LA SOLA LETTURA/SCRITTURA IN BACHECA
               SENZA POSSIBILITA' DI CREARE E RIMUOVERE MEMORIA CONDIVISA
               E SEMAFORI.
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

	if (  ((parseMainResult & 2)==2) ||
			((parseMainResult & 4)==4) ||
			((parseMainResult & 8)==8) ||
			((parseMainResult & 16)==16) ) {
		//Specificati paramentri -a, -d, -b, -l
		usage();
		printf("Errore: Questa versione non puo' utilizzare i parametri: -a, -d, -b, -l!\n");
		exit(-1);
	}
	else  if ( ((parseMainResult & 1)==1) )	{
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

	printf("Usare: VolatileBulletinBoard [OPZIONI]\nOPZIONI:\n");
	printf("	-s N     setta chiave identificazione semafori\n");
	printf("	-m N     setta chiave identificazione memoria condivisa\n");
	printf("	-h       Visualizza questo help e termina il programma\n");
	printf("\n");
	printf("Non usando i parametri -s, -m vengono usati i valori di default.\n");
	printf("\n");

}

