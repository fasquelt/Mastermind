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
	// Crée et initialise la socket passive
	serveurSocket = h_socket(AF_INET,SOCK_STREAM);
	adr_socket(service, NULL, SOCK_STREAM, &p_adr_serveur);
	h_bind(serveurSocket, p_adr_serveur);

	while (1) {	// Revient en écoute lorsque le client actuel a terminé la communication.
		h_listen(serveurSocket, nbReq); // Ecoute le réseau et reste en attente d'une connexion
		socketConnexion = h_accept(serveurSocket, p_adr_client); // Accept la connexion du client et construit la nouvelle socket.
	
		char play_again; // Variable contenant le choix du joueur de rejouer ou non.
		// On reste dans le do, while tant que le joueur veut rejouer
		do {
		/* Retour du choix de la difficulté de la partie par le client */
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

			free(bufferEmission); // Libère le buffer pour éviter de surcharger la mémoire
			h_reads(socketConnexion, &play_again, 1); // Lecture de la réponse si rejouer ou non
		} while (play_again == 'o');

		/*----------------------------------------------------------------*/
		// Fermeture de la connexion avec le client.
		h_shutdown(socketConnexion, FIN_ECHANGES);
		h_close(socketConnexion);
	}
	// Ctrl+C pour fermer le serveur
}

/******************************************************************************/	


/*
	Fonction de vérification du résultat.
	@param int* res
	@param char* user_value
	@param char* bufferRes

	@return 1 pour jeu terminé ou 0 pour continuer
	@attention bufferRes contient le résultat de la comparaison 'n' pour pas dans la combinaison,
	'c' pour bien placé et 'p' pour pas à la bonne place
*/
int check_result(int res[], char user_value[], char bufferRes[]) {
	int index_bufferRes = 0;
	int correct = 0; // Variable pour savoir 

	// Les deux boucles for servent à comparer les valeurs fournit par le client (char* user_value) et
	// les valeurs de la combinaison générer au départ (contenu dans le int* res).
	for (int index = 0; index < strlen(user_value); index++) {
		int isIn = 0;
		for (int j = 0; j < strlen(user_value); j++) {
			// Vérification si la couleur est dans la combinaison initiale
			if (((int)user_value[index])-48 == res[j]) {
				// Regarde si elle est également à la bonne place.
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
	@param int* res: Tableau contenant la combinaison
	@param int size: taille des combinaisons

*/
void init_game(int *res, int size) {
	int alreadyUsed[size]; // Tableau pour stocker les couleurs déjà dans le jeu. Sert à éviter les doublons.
	u_int8_t nb; // Variable contanant la couleur choisit
	bool used; // Décrit si la couleur choisi aléatoirement a déjà été utilisée
	srand(time(NULL)); // Initialisation de l'aléatoire.
	// Création de ma liste de couleurs de manière aléatoire sans doublon.
	int index = 0;
	while (index < size) {
		used = false;
		nb = rand() % 8;
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
	@param int socket: contient la valeur de la socket de connexion avec le client
	@param int difficulty: contient la difficulté choisie par le client
*/
void play_game(int socket, int difficulty) {
	// Variable contenant la partie en cours.
		char bufferJeu[difficulty];
		// Variable contenant le résultat à trouver
		int res[difficulty];
		
		// Lance la création du tableau de combinaison à trouver.
		init_game(res, difficulty);

		// Récupère du bufferJeuMoyen la combinaison du client.
		h_reads(socket, bufferJeu, difficulty);
		
		// Buffer contenant le résultat de la comparaison entre la combinaison serveur et du client
		char* bufferToReturn = malloc(difficulty * sizeof(char));
		// Buffer contenant l'information si le joueur a gagné ou non.
		char* bufferFinish = malloc(1 * sizeof(char));

		// Boucle de vérification de la combinaison du joueur avec celle du serveur
		// Communique les réponse au client et attend une nouvelle combinaison
		// On reste dedans tant que la combinaison n'est pas bonne
		while (!check_result(res, bufferJeu, bufferToReturn)) {
			h_writes(socket, bufferToReturn, difficulty);
			bufferFinish[0] = 0;
			h_writes(socket, bufferFinish, 1);

			h_reads(socket, bufferJeu, difficulty);
		}
		// Communications finales pour dire au joueur qu'il a gagné.
		h_writes(socket, bufferToReturn, difficulty);
		bufferFinish[0] = 1;
		h_writes(socket, bufferFinish, 1);
		// Libération des buffers de jeu
		free(bufferToReturn);
		free(bufferFinish);
}