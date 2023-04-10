/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/	


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include<stdlib.h>
#include <string.h>

#include "couleurs.c"

#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	client_appli(serveur,service);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{
	// PHASE DE CONNEXION //

	// Création et initialisation de la structure "socket"
	struct sockaddr_in *socket = malloc(sizeof(struct sockaddr_in));
	adr_socket(service, NULL, SOCK_STREAM, &socket);

	// Création de la socket
	int id_socket = h_socket(AF_INET, SOCK_STREAM); // AF_INET pour IPv4 et SOCK_STREAM pour TCP

	// Message d'erreur de création de la socket
	if (id_socket == -1){ // Si le test est vrai c'est que la création de la socket à échouée
		printf("Impossible de créer la socket\n");
		return;
	}

	// on se connecte au serveur
	h_connect(id_socket, socket);

	// PHASE DE COMMUNICATION (LE JEU) //

	char *bufferEmission = malloc(1000 * sizeof(char));

	printf("Choisissez votre difficultée : Saisir :  0 'Facile' | 1 'Moyen' | 2 'Difficile' \n");
	scanf("%s", bufferEmission);
	int length = (int)bufferEmission;

	// Ecrit dans la socket le choix de la difficultée
	h_writes(id_socket, bufferEmission, strlen(bufferEmission));

	//Test d'affichage du jeu Facile
	char* bufferJeuFacile = malloc(12 * sizeof(char));
	int readed = 0;
	while (readed == 0) {
		readed = h_reads(id_socket, bufferJeuFacile, 12);
		printf("Affichage du jeu: %s\n", bufferJeuFacile);
	}	

	// Connexion toujours active ?
	h_reads(id_socket, bufferEmission, 9);

	// Affichage du nombre d'essais au départ
	int triesleft = length+2;
	h_reads(id_socket, triesleft, 2);
	printf("Vous avez %d essais.\n", triesleft);

	// Lecture du nombre de pions
	h_reads(id_socket, length, 2);

	// Affichage du nombre de lettres
	printf("Il y a %d pions dans le mot. \n", length);

	// Proposition couleur et position
	char win[2];
	char* ligne;
	char* found;

	// Tant que la connexion n'est pas en "FERMETURE"
	while (strcmp(bufferEmission, "FERMETURE"))
	{
		// Choix de la nouvelle position
		printf("Entrez une position : \n");
		scanf("%d",bufferEmission);
		//printf("tests tfvgyhujik %d\n",(int) bufferEmission[0]);

		h_writes(id_socket, bufferEmission, 1);
		h_reads(id_socket, bufferEmission, 4);

		// Choix de la nouvelle couleur
		printf("Entrez une couleur : \n");
		scanf("%s", bufferEmission);

		// Envoi de la lettre au serveur
		h_writes(id_socket, bufferEmission, 1);
		h_reads(id_socket, bufferEmission, 4);

		if (bufferEmission[0] == 'J')
		{
			printf("La lettre demandée a déjà été jouée, veuillez choisir une autre lettre.\n");
		}

		// Connexion toujours active ?
		h_reads(id_socket, bufferEmission, 9);

		// On lit le mot à trouver
		h_reads(id_socket, ligne, 2);

		// On lit le nombre d'essais
		h_reads(id_socket, triesleft, 2);

		// Affichage du mot
		printf("\n%s\n", found);

		// Affichage du nb d'essais restants
		printf("Nombre d'essais restants : %s\n", triesleft);

		// On récupère le win côté serveur
		h_reads(id_socket, win, 1);

		// Si le jeu est terminé (nb_essais=0 ou que win=1)
		if (triesleft == 0 || atoi(win) == 1)
		{
			h_reads(id_socket, bufferEmission, 9);
		}
	}


 }

/*****************************************************************************/

