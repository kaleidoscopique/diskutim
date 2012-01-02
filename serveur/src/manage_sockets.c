#include "inc_define.h"
#include "inc_alllib.h"
#include "manage_sockets.h"
#include "manage_threads.h"


/**
	Affiche des informations sur l'état d'une connexion entre UN client et le serveur
	@param [in] sd Le descripteur de la socket client
*/
int who_connected (int sd)
{
	int size = sizeof(struct sockaddr);
	struct sockaddr_in me, you;
	struct hostent *h_you, *h_me;
	unsigned long addr;

	bzero(&me, sizeof(struct sockaddr));
	bzero(&you, sizeof(struct sockaddr));

	/* pour savoir de qui a qui est connecte notre socket */
	getsockname(sd, (struct sockaddr*)&me, (socklen_t*)&size);
	getpeername(sd, (struct sockaddr*)&you, (socklen_t*)&size);
	addr = inet_addr(inet_ntoa(you.sin_addr));
	h_you = gethostbyaddr((char*)&addr, sizeof(struct in_addr), AF_INET);
	addr = inet_addr(inet_ntoa(me.sin_addr));
	h_me = gethostbyaddr((char*)&addr, sizeof(struct in_addr), AF_INET);
	printf("[%s:%i] connected at [%s:%i]\n", h_you->h_name, ntohs(you.sin_port),
	h_me->h_name, ntohs(me.sin_port));

	return 0;
}

/**
	Retourne 1 si au moins un client est en ligne (aka si le serveur a une bonne raison de prospérer). Sinon 0.
	Permet de fermer le serveur proprement dans la boucle MAIN principale.
	@param clients_list La liste des clients connectés
*/

int at_least_one_client (const Tab_Dynamique* clients_list)
{
	int i=0;

	/* Cas particulier : si le tableau dynamique est vide, c'est que le serveur vient d'être lancé
	*   et qu'aucun client ne s'est connecté jusqu'à lors.
	*   Il ne faut donc pas FERMER le serveur ! On fait comme s'il existait un client, on continue à écouter. */
	if (clients_list->derniere_position == 0) return 1;

	// Sinon, on navigue dans la liste des clients voir si des descripteurs sont positifs
	// Si c'est le cas pour au moins un, on retourne 1. Sinon 0.
	for (i=0;i<clients_list->derniere_position;i++)
	{
		if (clients_list->ad[i] > 0) return 1;
	}
	printf("Aucun client connecté !\n");
	return 0;
}


/**
	Crée une socket côté serveur qui attent des connexion clients sur PORT
	@param sd_serv Le descripteur de socket serveur
	@param addr Le domaine d'adressage de la socket

*/
void create_socketS2C(int * sd_serv,struct sockaddr_in * addr)
{

	int on = 1;

	/* Création du socket SERVEUR en TCP/IP */
	if ((*sd_serv = socket(PF_INET, SOCK_STREAM,0)) < 0)
	{
		perror("socket");
	}

	/* Définition du domaine d'adressage du socket serveur */
	addr->sin_family = AF_INET; // Configure en TCP/IP
	addr->sin_port = htons(PORT); // Défini le port (voir DEFINE plus haut)
	addr->sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = toutes les IP peuvent se connecter sur ce socket

	/* cette commande permet de creer le socket serveur meme si le port est
	   deja utilise (utile si le serveur vient de planter et qu’on veut le relancer) */
	setsockopt(*sd_serv, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(on));


	/* On rattache le socket à un port et une adresse */
	if (bind(*sd_serv, (struct sockaddr*)addr, sizeof(struct sockaddr)) < 0)
	{
		perror("bind");
	}

	// On configure la socket comme étant non-bloquante
	/*
	Pourquoi une socket bloquante ?
	Sans ça, la boucle principale du programme ne tourne pas et se bloque sur 'accept'.
	Il n'y aurait aucun moyen de vérifier si des clients sont toujours connectés !
	Donc aucun moyen de fermer le serveur proprement
	*/
	fcntl(*sd_serv,F_SETFL,O_NONBLOCK);

	/* On définit la file d'attente à 5 connexions en queue max */
	if (listen(*sd_serv, 5) < 0)
	{
		perror("listen");
	}

	printf("En attente de connexion sur [localhost:%i] ...\n", PORT);

}
/**
	Nettoie le tableau contenant les descripteurs des sockets connectés et déconnectés (supprime les négatifs)
	(tableau de la forme {4,5,-6,7,-7,8} (< 0 => socket déconnecté précédemment)
	@param clients_list La structure contenant les descripteurs de sockets
	@param nb_connexion Le nombre de descripteurs à atteindre avant de nettoyer le tableau
*/
int clear_old_connexions(Tab_Dynamique * clients_list, int nb_connexions)
{

	 // S'il est temps de nettoyer le tableau de descripteurs clients (s'il y a plus de nb_connexions dedans)
	if (clients_list->derniere_position >= nb_connexions)
	{
		// On crée un nouveau tableau pour contenir la nouvelle liste
		Tab_Dynamique new_list;
		initialiser_tab(&new_list);
		int i;

		// On ajoute tous les clients connectés (SD > 0) dans un nouveau tableau dynamique
		for(i=0;i<clients_list->derniere_position;i++)
		{
			if (clients_list->ad[i] > 0)
			{
				// Recopie les descripteurs positifs dans le nouveau tableau
				ajoutElement_tab(&new_list,clients_list->ad[i]);
			}
		}

		// On vide l'ancien tableau de descripteurs
		clients_list->derniere_position = 0;
		// Puis on recopie toutes les valeurs >0 gardées plus haut dans le tableau d'origine
		for(i=0;i<new_list.derniere_position;i++)
		{
				ajoutElement_tab(clients_list,new_list.ad[i]);
		}

		// On libère le tableau temporaire
		testament_tab(&new_list);
		return 1;
	}
	else
	{
		// Si on ne nettoie pas le tableau (car pas assez de clients déconnectés (@param "nb_connections")
		return 0;
	}
}






