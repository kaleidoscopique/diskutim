#include "inc_define.h"
#include "inc_alllib.h"
#include "TAD_tabDyn.h"
#include "manage_sockets.h"
#include "manage_threads.h"
#include "manage_clients.h"
#include "manage_services.h"
#include "manage_protocole.h"

int main (int ac, char *av[])
{

	srand(time(NULL));

	// Socket serveur, socket client "temporaire" (tampon)
	int sd_serv, sd_comm,i;

	// Domaine d'adressage du serveur
	struct sockaddr_in addr;
	int size = sizeof(struct sockaddr);

	// Création d'un tableau dynamique pour stocker les clients connectés
	Tab_Dynamique listof_connected_clients;
	initialiser_tab(&listof_connected_clients);

	// Création du tableau de fiches client
	info_client listof_info_clients[MAX_CLIENTS];

	// Initialisation des services
	TabServices listof_services;
	complete_services_tab(&listof_services);

	/*printf("Clef : %d\n",hash_index_users_bd("RACEB"));
	printf("Clef : %d\n",hash_index_users_bd("RACEBzeff86"));*/

	int index_db_users[MAX_CLIENTS];
	int count_db_users = parse_users_db(index_db_users);
	printf("Chargement de la base de données ./data/users : %d entrées\n",count_db_users);

	/*  Cette structure est passée en paramètre de la fonction de dialogue pour permettre à chaque thread
		de disposer de ces informations à tout moment */
	Param_thread* param;

	// Création et parametrage de la socket serveur<->client
	create_socketS2C(&sd_serv,&addr);
	srand(time(NULL));
	/* Tant qu’on a de nouvelles connections, on les traite */
	do
	{
		if(rand()%30 == 0) printf("Wait...\n");

		sd_comm = accept(sd_serv, (struct sockaddr*)&addr, (socklen_t*)&size);
		if (sd_comm != -1)
		{
			// param est un pointeur vers une structure qui contient les paramètres à envoyer au thread
			init_param_thread(&param); // Malloc d'une struct param
			bind_param_thread(&param,&listof_connected_clients,sd_comm,listof_info_clients,&listof_services,index_db_users); // Remplissage de la structure avec ce qu'il faut

			// Détails sur les utilisateurs connectés
			printf("Ajout du client %d dans la liste...\n",sd_comm);
			add_new_descriptor(&listof_connected_clients,sd_comm);

			// Affichage de la liste
			printf("Liste des utilisateurs-descripteurs connectés : ");
			show_descriptors_list(&listof_connected_clients);

			// Création du thread pour le client
			new_thread(dialog, (void*)param);
		}

		usleep(SLEEP_NONBLOCK);
	} while (1/*at_least_one_client(&listof_connected_clients) == 1*/);

	printf("Le serveur s'arrête....\n");
	testament_tab(&listof_connected_clients);

	for(i = 0;i<listof_services.nb_services;i++)
	{
		testament_tab(listof_services.tab[i].listof_client_descriptors); // Libère le contenu du tableau dynamique
		free(listof_services.tab[i].listof_client_descriptors); // Libère le TAB lui-même (structure allouée avec un malloc !)
	}

	free(param);
	close(sd_serv);

	return 0;
}


