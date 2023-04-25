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

#include "pimple.c"

#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

void client_appli (char *serveur, char *service);

int nb_essai = 1;

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
	adr_socket(service, serveur, SOCK_STREAM, &socket);

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
	char play_again;
	// On reste dans la boucle tant que le client redemande à jouer
	do {	
		char *bufferEmission = malloc(sizeof(char));
		// Gestion du choix de difficulté
		while ((bufferEmission[0]-48 != 0) && (bufferEmission[0]-48 != 1) && (bufferEmission[0]-48 != 2))
		{
			printf("Choisissez votre difficultée : Saisir :  0 'Facile' | 1 'Moyen' | 2 'Difficile' \n");
			scanf("%s", bufferEmission);
		}

		// Ecrit dans la socket le choix de la difficultée
		h_writes(id_socket, bufferEmission, strlen(bufferEmission));

		int size_mastermind = (bufferEmission[0]-48)+4; // -48 pour revenir à un int classique, +4 pour avoir les size du mastermind
		init_game_client(id_socket, size_mastermind);
		free(bufferEmission);
		// Gère la demande de nouvelle partie et transmet l'information au serveur
		do {
			printf("Voulez-vous rejouer ? o | n\n");
			scanf(" %c", &play_again);
		} while (play_again != 'o' && play_again != 'n');
		h_writes(id_socket, &play_again, 1);
	} while (play_again == 'o');

	/* Fermeture connexion */
	h_shutdown(id_socket, FIN_ECHANGES);
	h_close(id_socket);
 }

/*****************************************************************************/

/*
	Réagit en fonction du résultat du serveur par rapport à la combinaison du client
	@param char* bufferRes
	@param int socket
	@param int size: Nombre de pines à trouver
	@return 1 si partie terminée, 0 sinon
*/
int check_serv_res(char* bufferRes, int socket, int size) {
	h_reads(socket, bufferRes, size);
	printf("%s\n", bufferRes);	

	char* bufferIsFinished = malloc(1 * sizeof(char));
	// Récupère l'information si la partie est terminés ou non par le serveur
	h_reads(socket, bufferIsFinished, 1);
	
	// S'effectue si la partie est terminée
	if (bufferIsFinished[0] == 1) {
		printf("Vous avez gagné en %d essais !\n", nb_essai);
		free(bufferIsFinished);
		return 1;
	}

	free(bufferIsFinished);
	return 0;
}

/*
	Récupère la combinaison saisie par le client et la transmet au serveur
	@param char* bufferGame
	@param int socket
	@param int difficulty
*/
void play(char* bufferGame, int socket, int difficulty) {
	printf("Entrez une combinaison de %d couleurs (sans espace entre les chiffres): \n", difficulty);
	scanf("%s", bufferGame);

	print_colors(bufferGame);
	h_writes(socket, bufferGame, difficulty);
}

/*
	Affiche les couleurs choisient par le client dans le terminal
	@param char* buffer_colors
*/
void print_colors(char* buffer_colors) {
	int i = 0;
	while (buffer_colors[i]) {
		if (buffer_colors[i]-48 == Rouge) {
			printf("Rouge ");
		}
		else if (buffer_colors[i]-48 == Bleue) {
			printf("Bleue ");
		}
		else if (buffer_colors[i]-48 == Vert) {
			printf("Vert ");
		}
		else if (buffer_colors[i]-48 == Jaune) {
			printf("Jaune ");
		}
		else if (buffer_colors[i]-48 == Violet) {
			printf("Violet ");
		}
		else if (buffer_colors[i]-48 == Orange) {
			printf("Orange ");
		}
		else if (buffer_colors[i]-48 == Rose) {
			printf("Rose ");
		}
		else {
			printf("Flushia ");
		}
		i++;
	}
	printf("\n");
}

/*
	Controle la partie du client
	@param int socket
	@param int size
*/
void init_game_client(int socket,int size) {
	// Initialise le buffer de notre partie
	char* bufferJeu = malloc(size * sizeof(char));

	printf("Les couleurs disponibles sont 0->rouge, 1->bleue, 2->vert, 3->jaune, 4->violet, 5->orange, 6->marron, 7->rose, 8->flushia\n");

	play(bufferJeu, socket, size);
	// Initialise le buffer d'analyse du résultat fournit par le serveur
	char* bufferRes = malloc(5 * sizeof(char));
	// Continue la partie tant que la bonne combinaison n'a pas été trouvée
	while (!check_serv_res(bufferRes, socket, size)) {
		play(bufferJeu, socket, size);
		nb_essai += 1; // Variable global qui conserve l'information du nombre d'essais effectués
	}
	free(bufferJeu);
}