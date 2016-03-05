#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "iniobj.h"


/*
  clio.c
      initialise le socket TCP
      fait une demande de connexion au serveur
  	  boucle :
  	       envoi de "n" objets "obj" (obtenus de iniobj.h)
  	       le dernier objet envoyé contient un marqueur de fin
  	       obj.iqt = -1
      fermer la connexion
      fin
*/

int main(int argc, char **argv){
	int sd;
	struct sockaddr_in saddrcli, saddrser;
	struct hostent *hid;

	// Initialisation du tableau d'objets
	strcpy(tableau[0].id,"ident_o1");
	strcpy(tableau[0].descrition,"description_o1");
	tableau[0].ii = 11;
	tableau[0].jj = 12;
	tableau[0].dd = 10.2345;

	strcpy(tableau[1].id,"ident_o2");
	strcpy(tableau[1].descrition,"description_o2");
	tableau[1].ii = 21;
	tableau[1].jj = 22;
	tableau[1].dd = 20.2345;

	strcpy(tableau[2].id,"ident_o3");
	strcpy(tableau[2].descrition,"description_o3");
	tableau[2].ii = 31;
	tableau[2].jj = 32;
	tableau[2].dd = 30.2345;

	tableau[3].ii = -1;

	// Création d'un socket
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sd == -1){ 
		perror("socket()");
		exit(-1);
	}

	// Construction de l'addresse du serveur
	bzero(&saddrser, sizeof(saddrser)); // Initialisation à zero de la structure
	saddrser.sin_family = AF_INET;
	saddrser.sin_addr.s_addr = htonl(INADDR_ANY);
	saddrser.sin_port = htons(atoi(argv[2]));
	hid = gethostbyname(argv[1]); //Demande au DNS l'@IP du serveur d'après son nom
	if(hid == NULL){
		perror("gethostbyname()");
		exit(-1);
	}

	// Copier une séquence d'octets	
	bcopy(hid->h_addr, &(saddrser.sin_addr.s_addr), hid->h_length);

	// Débuter une connexion sur une socket
	int c = connect(sd, (struct sockaddr*)&saddrser, sizeof(saddrser));
	if(c == -1){
		perror("bind()");
		exit(-1);
	}
	printf("coucou\n");
	int e = send(sd, &tableau[0], sizeof(obj), 0);
	if (e == -1){
		perror("send()");
		exit(-1);	
	}
}


