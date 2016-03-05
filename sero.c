#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <errno.h>
#include "defobj.h"

/* 
	sero.c
      initialise le socket TCP
      boucle :
  	  attente des connexions clients
  	  quand connexion client: fork()
  	      fils : traiterclient()
  		      boucle : lire data client sur socket
  		    	       jusqu'à objet contenant "fin"
  	      père : waitpid() attends fin du fils
  		      si statut fin fils et retour sur attente des connexions clients 
*/

int main(int argc, char** argv){
	int sd;
	socklen_t saddrl;
	struct sockaddr_in saddr;
	obj* objet_recu;
	// Création d'un socket
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sd == -1){ 
		perror("socket()");
		exit(-1);
	}

	// Fournir une addresse à une socket
	saddrl = sizeof(saddr);
	bzero(&saddr, saddrl); // Initialisation à zero de la structure
	saddr.sin_family = AF_INET; // IPV4
	saddr.sin_addr.s_addr = htonl(INADDR_ANY); //fais des échanges d'octet si on est en big indian
	saddr.sin_port = htons(atoi(argv[1]));

	int bd = bind(sd, (const struct sockaddr*)&saddr, saddrl);
	if(bd == -1){
		perror("bind()");
		exit(-1);
	}

	// Attendre des connexions sur une socket
	int l = listen(sd, SOMAXCONN); 
	if(l == -1){
		perror("listen()");
		exit(-1);
	}

	while(1){
		// Accepter une connexion sur une socket
		printf("ok\n"); // Test
		fflush(stdout); // Test
		int a = accept(sd, (struct sockaddr*)0, 0);
		if(a == -1){
			perror("accept()");
			exit(-1);
		}

		// Création d'un processus fils
		int pid = fork(), status;
		if(pid == -1){
			perror("fork()");
			exit(-1);
		}
		else if (pid == 0){ // C'est le fils
			printf("cool\n"); // Test
			fflush(stdout); // Test
			recv(sd, objet_recu, sizeof(obj), 0);
		}
		else { // C'est le père
			waitpid(pid, &status, NULL); // Le père attend la fin du fils
		}
	}
}
