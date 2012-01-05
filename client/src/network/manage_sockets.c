#include "../inc_define.h"
#include "../inc_alllib.h"
#include "manage_sockets.h"
#include "manage_protocole.h"
#include "manage_services.h"
#include "manage_threads.h"

static void parse_ServerConfigFile(char* filename,char* server_ip,int* server_port);
void complete_data_fileslist(char* data,char files_list[][MAX_SIZE_FILE_NAME],int * nb_files);
void complete_data_filecontent(char* buffer,char* file_title);

/**
	Crée un socket côté client pour se brancher sur un serveur donné
	@param sd Le descripteur de socket principal client
	@param addr Le domaine d'adressage de la socket
*/
void create_socketC2S(int * sd,struct sockaddr_in * addr)
{

	// Parse le fichier de configuration et récupère les données de connexion
	char server_ip[MAX_SIZE];
	int server_port;
	parse_ServerConfigFile("./data/config",server_ip,&server_port);

	// Adressage du domaine
	bzero(addr, sizeof(struct sockaddr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(server_port);
	addr->sin_addr.s_addr = inet_addr(server_ip);

	// Création du descripteur
	if ((*sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
	}

	// Fusion de l'adressage et de la socket
	if (connect(*sd, (struct sockaddr*)addr, sizeof(struct sockaddr)) < 0)
	{
		// Message d'erreur
		printf("Le serveur n'est pas connecté ! Veuillez contacter l'administrateur responsable.\n");
		exit(0);
	}
}


/**
	Fonction qui vérifie sans cesse si le serveur envoie un message au client
	@param arg Le socket serveur a vérifier */

void* check_server_reply (void* arg)
{
	// Conversion du paramètre d'entrée
	Param_thread * param = (Param_thread*)arg;
		int sd = param->client_descriptor; // on récupère le descriptor client
		void (*adapted_show_services_list)(Services*, void*) = param->adapted_show_services_list; // la fn adapté à l'affichage
		void (*adapted_show_users_list)(Clients*, void*) = param->adapted_show_users_list; // la fn adapté à l'affichage
		void (*adapted_show_error_and_msg)(void*,void * ,char*,char*, char*,int,char*) = param->adapted_show_error_and_msg; // la fn adapté à l'affichage
		void (*adapted_show_files_list)(char[][MAX_SIZE_FILE_NAME],int, void*) = param->adapted_show_files_list; // Adapté à l'affichage de la liste des fichiers
		void (*adapted_show_file_content)(char*,char*, void*) = param->adapted_show_file_content; // Adapté à l'affichage de la liste des fichiers


	char buffer[MAX_SIZE]; // Le buffer principal, celui qui reçoit les rawdata du serveur
	char file_title[MAX_SIZE_FILE_NAME];
	Services listof_services; 	// stock la liste des services dispo sur le serveur
	Clients listof_users; // stock la liste des autres clients connectés au même salon que l'utilisateur
	int sock_err, type_buffer;

	// Des string qui serviront à recevoir les données d'un message
	char author[MAX_SIZE]; memset(author,0,MAX_SIZE);
	char msg[MAX_SIZE]; memset(msg,0,MAX_SIZE);
	char service[MAX_SIZE]; memset(service,0,MAX_SIZE);
	char others[MAX_SIZE]; memset(others,0,MAX_SIZE);
	char files_list[MAX_NB_FILES][MAX_SIZE_FILE_NAME];
	int nb_files=0;

	while(1)
	{
		memset(buffer,0,MAX_SIZE);

		/* Stock le message serveur au format RAWDATA :
		*			/SYS+CODE (ex : /SYS+402 pour transmettre un code 402)
		*			/MSG+TOKENmsgTOKENauteurTOKENservices (pour transmettre un message)
		*			/USERS+user1+user2... (pour transmettre une liste d'utilisateurs)
		*			/SERVICES+service1+service2... (pour transmettre la liste des services) */
		sock_err = recv(sd, buffer, MAX_SIZE, 0);

		// Retourne le type de contenu reçu (SYS ou MSG ou USERS ou SERVICES ou FILES ou GET sous forme d'INT)
		type_buffer = get_type_buffer(buffer);

		if (sock_err > 0)
		{
			switch(type_buffer)
			{
				case 1: // Liste de services
				// On récupère la réponse serveur, on la traite et l'affiche en fonction de la librairie
				get_listof_services(buffer, &listof_services);
				(*adapted_show_services_list)(&listof_services,param->treeview_services_list);
				break;

				case 2:
				// On utilise une fonction adaptée pour afficher la liste des services à l'utilisateur
					// |-> Avec printf pour le NOGUI
					// |-> En mettant à jour le widget treeview avec GTK
				get_listof_online_clients(buffer,&listof_users); // Reçoit la liste et la stock dans la structure adéquate
				(*adapted_show_users_list)(&listof_users,param->treeview_users_list); // Affichage
				break;

				case 3:
				complete_data_msg(buffer,author,msg,service,others); // On extrait les données
				// Utilise la bonne fonction pour afficher le message utilisateur broadcasté
				(*adapted_show_error_and_msg)(param->textview_discussion,param->window_connection,author,msg,service,0,buffer);
				break;

				case 4:
				// Utilise la bonne fonction pour afficher le message d'erreur
				(*adapted_show_error_and_msg)(param->textview_discussion,param->window_connection,"","","",sysmsg2int(buffer),buffer);
				break;

				case 5:
				complete_data_fileslist(buffer,files_list,&nb_files); // On extrait les données
				(*adapted_show_files_list)(files_list,nb_files,param->treeview_files_list);
				break;

				case 6:
				complete_data_filecontent(buffer,file_title); // On extrait les données
				(*adapted_show_file_content)(buffer,file_title,param->textview_file_content);
				break;
			}
		}
	}

	close(sd);
	pthread_exit(NULL);
}

/**
	Parse le fichier de configuration de Diskutim
	@param filename Le nom du fichier
	@param server_ip L'IP du serveur récupérée
	@param server_port Le port du servuer récupérée
*/
static void parse_ServerConfigFile(char* filename,char* server_ip,int* server_port)
{
	FILE* file = fopen(filename,"r");
	if(file != NULL)
	{
		char line[MAX_SIZE];
		while(fgets(line,MAX_SIZE,file) != NULL)
		{
			if(line[0] == '#') { /* On ne fait rien, il s'agit d'un commentaire */ }
			else
			{
				if(!strncmp(line,"SERVER_IP",strlen("SERVER_IP"))) 							sscanf(line,"%*s %s",server_ip);
				else if(!strncmp(line,"SERVER_PORT",strlen("SERVER_PORT"))) 	sscanf(line,"%*s %d",server_port);
				else 																												{/* Si l'attribut est fictif, on ne fait rien */};
			}
		}
	}
	else
	{
		printf("erreur d'ouverture de config.ini\n");
		exit(0);
	}
}

/**
	Transforme la liste rawdata (/FILES+file1+file2...) du serveur en un tableau à 2 dimensions exploitables client-side
	@param data Les data brutes
	@param files_list La liste à remplir
	@param nb_files Le nombre de fichiers partagés
*/
void complete_data_fileslist(char* data,char files_list[][MAX_SIZE_FILE_NAME],int * nb_files)
{
	char* pch;
	*nb_files = 0;

	int i;
	// RaZ du tableau de SERVICES
	for (i=0;i<MAX_NB_FILES;i++)
		memset(files_list[i],0,MAX_SIZE_FILE_NAME);

	data[strlen(data)-1] = '\0'; // on enlève le + final dans la chaine de caractères qui contient les services
	pch = strtok(data,TRAME_SEPARATOR);
	while (pch != NULL)
	{
		strcpy(files_list[*nb_files],pch); // pour chaque fichier, on enregistre son nom dans le tableau 2D
		pch = strtok(NULL, TRAME_SEPARATOR); // puis on strock
		(*nb_files)++; // on incrémente le nb de fichiers
	}
}

/**
	Récupère le contenu d'un fichier via un buffer envoyé par le serveur
	@param buffer Reçoit un buffer rawdata avec le contenu d'un fichier
	@param title [out] Récupère le nom du fichier récupéré
*/
void complete_data_filecontent(char* buffer,char* title)
{
	char temp[MAX_SIZE];
	strcpy(temp,buffer);

	char * pch;
	pch = strtok (temp,"+");
	strcpy(title,pch);
	pch = strtok (NULL, "+");
	strcpy(buffer,pch);

}
