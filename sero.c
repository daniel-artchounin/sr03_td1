#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
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

void handler(int the_signal);
void afficher_objet(const obj* objet_a_afficher);

int sd;

int main(int argc, char** argv){
	struct sigaction struct_sigaction; // La structure qui gère le signal de fin du serveur
	struct_sigaction.sa_handler = handler;
	sigemptyset(&struct_sigaction.sa_mask);
	struct_sigaction.sa_flags = 0;
	struct sockaddr_in saddr_ser, saddr_cli;
	socklen_t saddr_ser_l = sizeof(saddr_ser), saddr_cli_l = sizeof(saddr_cli);
	obj objet_recu;
	size_t obj_taille = sizeof(obj);

	sigaction(SIGINT, &struct_sigaction, 0); // Affectation de la structure gérant la 
	// fin du serveur

	// Vérification du bon nombre de paramètres
	if ((argc < 2) || (argc > 2)){
		fprintf(stderr, "Usage : %s <Numéro de " \
		"port du serveur>\n", argv[0]);
		exit(1);
	}

	// Création d'un socket
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sd == -1){ 
		perror("socket()");
		exit(-1);
	}

	// Fournir une addresse à une socket
	bzero(&saddr_ser, saddr_ser_l); // Initialisation à zero de la structure
	saddr_ser.sin_family = AF_INET; // IPV4
	saddr_ser.sin_addr.s_addr = htonl(INADDR_ANY); //fais des échanges d'octet si on est en big indian
	saddr_ser.sin_port = htons(atoi(argv[1]));

	int bd = bind(sd, (const struct sockaddr*)&saddr_ser, saddr_ser_l);
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

	// Le serveur fonctionne tant qu'il n'a pas reçu de signal de fin
	while(1){		
		printf("Ready\n");
		fflush(stdout);
		// Accepter une connexion sur une socket
		int accepted_socket = accept(sd, (struct sockaddr*)&saddr_cli, &saddr_cli_l);
		if(accepted_socket == -1){
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
			printf("Je commence à afficher les objets d'un client.\n");
			fflush(stdout);
			while(1){
				// Réception de l'objet envoyé par le client
				ssize_t recv_taille = recv(accepted_socket, &objet_recu, obj_taille, 0);
				if(recv_taille != obj_taille){
					perror("recv()");
					exit(-1);
				}
				else{
					if(objet_recu.ii != -1){
						// Absence du marqueur de fin donc affichage
						afficher_objet(&objet_recu);
					}
					else{
						// Présence du marqueur de fin
						printf("J'ai fini d'afficher les objets d'un client.\n");
						fflush(stdout);
						exit(0);
					}
				}
			}
		}
		else { // C'est le père
			waitpid(pid, &status, NULL); // Le père attend la fin du fils
			close(accepted_socket); // Fermeture de la socket de service
		}
	}
}

void afficher_objet(const obj* objet_a_afficher){
	// Affichage de l'objet transmis en paramètre
	printf("Id : %s\n\t- Description : %s\n\t- ii : %d\n\t- jj : %d\n\t- dd : %f\n", 
		objet_a_afficher->id, 
		objet_a_afficher->description, 
		objet_a_afficher->ii, 
		objet_a_afficher->jj,
		objet_a_afficher->dd);
	fflush(stdout);
}

void handler(int the_signal){
	// Gestion d'un signal
	close(sd); // Fermeture d'une socket
	printf("J'ai fini mon travail, bye bye !\n");
	fflush(stdout);
	exit(0); // Fin du serveur
}