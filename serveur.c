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
	// Il faudrait créer des tableaux de socketConnexion et des tableau de p_adr_client pour gérer plusieurs client.

	struct sockaddr_in *p_adr_serveur;
	struct sockaddr_in *p_adr_client;

	serveurSocket = h_socket(AF_INET,SOCK_STREAM);
	adr_socket(service, NULL,SOCK_STREAM, &p_adr_serveur);
	h_bind(serveurSocket, p_adr_serveur);
	h_listen(serveurSocket, nbReq);
	socketConnexion = h_accept(serveurSocket, p_adr_client);

	/* Initialisation de la partie */
	char *bufferEmission = malloc(1000 * sizeof(char));
	int readed = 0;
	while (readed == 0) {
		readed = h_reads(socketConnexion, bufferEmission, 1);
		printf("%d\n", readed);
	}
	/* 
		Partie de difficulté simple (4 couleurs)
	*/
	if ((int)bufferEmission[0] == 48) // 48 le code ASCII de 0
	{
	// Rouge: ro, Bleu: bl, Jaune, ja, Vert, ve
     	char *bufferJeuFacile = malloc(12 * sizeof(char));
		bufferJeuFacile = "ro bl ja ve";
		h_writes(socketConnexion, bufferJeuFacile, 12);
	}
	/* 
		Partie de difficulté moyenne (5 couleurs) 
	*/
	else if ((int)bufferEmission[0] == 49)
	{
		// Jeu à 5 couleurs
		int alreadyUsed[5];
		// Variable contenant la partie en cours.
		char *bufferJeuMoyen = malloc(SIZE_MOYEN * sizeof(char));
		u_int8_t nb;
		bool used;
		srand(time(NULL)); // Initialisation de l'aléatoire.
		// Création de ma liste de couleurs de manière aléatoire sans doublon.
		for (int i = 0; i < SIZE_MOYEN; i++) {
			used = false;
			nb = rand() % 9;
			for (int j = 0; j < 4; j++) {
				if (nb == alreadyUsed[j]) {
					used = true;
				}
			}
			if (!used) {
				bufferJeuMoyen[i] = nb;
				alreadyUsed[i] = nb;
			}
		}     	
		h_writes(socketConnexion, bufferJeuMoyen, 5);
	}
	/* 
		Partie de difficulté difficile (6 couleurs)
	*/
	else // C'est du côté Client que l'on fait la vérification de la valeur entrée.
	{
     	char *bufferJeuDifficile = malloc(18 * sizeof(char));

		h_writes(socketConnexion, bufferJeuDifficile, 18);
	}


	/*
		Gestion de la partie
	*/


	/*----------------------------------------------------------------*/
	// Fermeture de la connexion avec le client.
	h_close(socketConnexion); 

}

/******************************************************************************/	

