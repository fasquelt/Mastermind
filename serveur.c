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
#include <curses.h>

#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

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

/* A completer ... */
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

/* Gestion des échanges à faire ici*/



/*----------------------------------------------------------------*/

h_close(socketConnexion); // Fermeture de la connexion avec le client.

}

/******************************************************************************/	

