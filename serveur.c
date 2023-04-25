/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/	

#include<stdio.h>
#include <stdlib.h>
#include <curses.h>
#include<time.h>
#include <string.h>

#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>
#include "pimple.c"

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

#define SIZE_FACILE  4
#define SIZE_MOYEN  5
#define SIZE_DIFFICILE  6

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/
	
	serveur_appli(service);
}


/******************************************************************************/	
void serveur_appli(char *service)

/* Procedure correspondant au traitemnt du serveur de votre application */

{

	/* Initialisation de la connexion */
	int serveurSocket;
	int socketConnexion;
	int nbReq = 1; // Nombre de requètes en file d'attente, 1 pour le moment
	// Il faudrait créer des tableaux de socketConnexion et des tableau de p_adr_client pour gérer plusieurs clients.

	struct sockaddr_in *p_adr_serveur = NULL;
	struct sockaddr_in *p_adr_client = NULL;

	serveurSocket = h_socket(AF_INET,SOCK_STREAM);
	adr_socket(service, NULL, SOCK_STREAM, &p_adr_serveur);
	h_bind(serveurSocket, p_adr_serveur);
	h_listen(serveurSocket, nbReq);
	socketConnexion = h_accept(serveurSocket, p_adr_client);

	char play_again;
	do {
	/* Initialisation de la partie */
		char *bufferEmission = malloc(sizeof(char));	
		h_reads(socketConnexion, bufferEmission, 1);
	
	/* 
		Partie de difficulté simple (4 couleurs)
	*/
	
		if (bufferEmission[0]-48 == 0) // 48 le code ASCII de 0
		{
			play_game(socketConnexion, SIZE_FACILE);
		}

	// -------------------------------------------------------------------------------
		
		/* 
			Partie de difficulté moyenne (5 couleurs) 
		*/
		else if (bufferEmission[0]-48 == 1)
		{
			play_game(socketConnexion, SIZE_MOYEN);
		}

	// -------------------------------------------------------------------------------

		/* 
			Partie de difficulté difficile (6 couleurs)
		*/
		else // C'est du côté Client que l'on fait la vérification de la valeur entrée.
		{
			play_game(socketConnexion, SIZE_DIFFICILE);
		}

		free(bufferEmission);
		h_reads(socketConnexion, &play_again, 1);
	} while (play_again == 'o');

	/*----------------------------------------------------------------*/
	// Fermeture de la connexion avec le client.
	h_shutdown(socketConnexion, FIN_ECHANGES);
	h_close(socketConnexion);

}

/******************************************************************************/	


/*
	Fonction de vérification du résultat
*/
int check_result(int res[], char user_value[], char bufferRes[]) {
	int index_bufferRes = 0;
	int correct = 0;

	for (int index = 0; index < strlen(user_value); index++) {
		int isIn = 0;
		for (int j = 0; j < strlen(user_value); j++) {
			if (((int)user_value[index])-48 == res[j]) {
				
				if (index == j) {
					bufferRes[index_bufferRes] = 'c';
					correct += 1;
				}
				else {
					bufferRes[index_bufferRes] = 'p';
				}
				isIn = 1;
			}
		}
		if (!isIn)
			bufferRes[index_bufferRes] = 'n';
		index_bufferRes += 1;
	}
	if (correct == strlen(user_value))
		return 1;
	return 0;
}

/*
	Init Game by initialising the color's array to find
*/
void init_game(int *res, int size) {
	int alreadyUsed[size];
	u_int8_t nb;
	bool used;
	srand(time(NULL)); // Initialisation de l'aléatoire.
	// Création de ma liste de couleurs de manière aléatoire sans doublon.
	int index = 0;
	while (index < size) {
		used = false;
		nb = rand() % 9;
		for (int j = 0; j < 4; j++) {
			if (nb == alreadyUsed[j]) {
				used = true;
			}
		}
		if (!used) {
			res[index] = (char)nb;
			alreadyUsed[index] = (char)nb;
			index ++;
		}
	}
}

/*
	Control the game
*/
void play_game(int socket, int difficulty) {
	// Variable contenant la partie en cours.
		char bufferJeuMoyen[difficulty];
		// Variable contenant le résultat à trouver
		int res[difficulty];
		
		init_game(res, difficulty);

		h_reads(socket, bufferJeuMoyen, difficulty);
		
		char* bufferToReturn = malloc(difficulty * sizeof(char));
		char* bufferFinish = malloc(1 * sizeof(char));

		while (!check_result(res, bufferJeuMoyen, bufferToReturn)) {
			h_writes(socket, bufferToReturn, difficulty);
			bufferFinish[0] = 0;
			h_writes(socket, bufferFinish, 1);

			h_reads(socket, bufferJeuMoyen, difficulty);
		}

		h_writes(socket, bufferToReturn, difficulty);
		bufferFinish[0] = 1;
		h_writes(socket, bufferFinish, 1);
		free(bufferToReturn);
		free(bufferFinish);
}