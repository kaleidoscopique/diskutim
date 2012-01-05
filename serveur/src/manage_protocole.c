#include "inc_define.h"
#include "inc_alllib.h"
#include "TAD_tabDyn.h"
#include "manage_sockets.h"
#include "manage_threads.h"
#include "manage_services.h"
#include "manage_protocole.h"
#include "manage_clients.h"
#include "manage_auth.h"
#include <math.h>

// Ces 3 fonctions doivent aller dans manage_services.h mais bug de compilation
// un peu incompréhensible sûrement du à un souci de dépendances croisées
TabServices* get_listof_services(Param_thread * param);
Service* get_the_nth_service(Param_thread * param, int n);
Tab_Dynamique* get_the_nth_service_tab(TabServices * listof, int n);

// Evaluation cmds
static int globaldata_evaluation(char*, Param_thread *);
static int cmd_evaluation(char*, Param_thread *);
static int msg_evaluation(char*, Param_thread *);
static int CMD_CONNECT_exec(char * data,info_client * sheet,Param_thread * param);
// Création des rawdata
static void create_rawdata_userslist(char* buffer, Service* client_service, info_client * listof_info_clients);
static void create_rawdata_serviceslist(char * buffer,TabServices * listof_services);
static void create_rawdata_msg(char* buffer,info_client * sheet,TabServices * listof_services);
static void create_rawdata_fileslist(char * buffer);
static void create_rawdata_filecontent(char * buffer, char* file);
static void create_doc(char *buffer);
// Broadcast
static void broadcast_rawdata_msg(char* buffer,int  my_sd,info_client *, TabServices * listof_services);
static void broadcast_rawdata_userslist(Param_thread * param);
// Sending
static void send_data_error(int sd, char* error_code,char* others);
// Purge
static void purge_all_connections(Param_thread * param);




/**
	Fonction de dialogue entre le serveur et le client
	@param arg Les paramètres passés au thread (cf plus bas)
*/
void* dialog (void* arg)
{

	// Conversion du PARAM THREAD qui contient respectivement :
		// -> Le SD du client
		// -> La liste des SD des autres clients connectés
		// -> La liste des toutes les fiches clients
		// -> La liste de tous les services disponibles (accompagnés de leurs utilisateurs)
	Param_thread * param = (Param_thread*)(arg);
	int sd = get_descriptor(param);

	int sock_err;
	char buffer[MAX_SIZE];
	int eval_return = 1;

	// Affiche des informations sur les entités qui se connectent
	// dans ce thread.
	who_connected(sd);

	// Si GLOBALDATA_EVALUATION renvoie 0, on déconnecte le client.
	while (eval_return == 1)
	{
		memset(buffer,0,MAX_SIZE);
		sock_err = recv(sd, buffer, MAX_SIZE, 0); // on reçoit les requêtes client

		if (sock_err == 0) break;
		else eval_return = globaldata_evaluation(buffer, param);
	}

	// Déconnecte les SD du client dans le tableau global & le tableau de son service
	// Purge ces tableaux si besoin
	purge_all_connections(param);

	// On va broadcaster la nouvelle userlist aux autres clients du service concerné
	broadcast_rawdata_userslist(param);

	// On libère l'espace du PARAM thread, on ferme la socket, on tue le thread
	close(sd);
	pthread_exit(NULL);
}

/**
	Première interface client<->serveur : décide si des données sont MSG ou CMD
	@param buffer Les données à traiter
	@param param Les données utiles au traitement (liste des clients, descripteurs du client...)
	@return 1 pour conserver la connexion et 0 pour la fermer
*/
static int globaldata_evaluation(char* buffer, Param_thread * param)
{
	info_client * listof_info_clients = get_info_clients_list(param); // Liste des fiches de tous les clients
	int sd = get_descriptor(param); // SD du client actuel
	info_client * sheet = get_sheet(listof_info_clients,sd);

	// On s'occupe des commandes particulières qui ne nécessitent pas de connexion
	// AKA : /connect & /services
	if (!strncasecmp(buffer, "/services", strlen("/services"))
		|| !strncasecmp(buffer, "/connect", strlen("/connect"))
		|| !strncasecmp(buffer, "/help", strlen("/help"))
		|| !strncasecmp(buffer, "/quit", strlen("/quit")))
	{
		return cmd_evaluation(buffer,param);
	}

	// Si l'utilisateur ne fait pas de demande de connexion (ci-dessus),
	// il doit être connecté pour envoyer une CMD ou un MSG
	if(get_connected(sheet) == 1)
	{
		if(buffer[0] == '/') return cmd_evaluation(buffer,param); // S'il envoie une commande (commence par '/')
		else return msg_evaluation(buffer,param); // Sinon, c'est simplement un message...
	}
	else
	{
		// On envoie un message d'erreur 501 au client pour lui signaler l'erreur
		printf("/!\\ Un client inconnu tente d'utiliser une commande protégée (%d - \"%s\").\n",sd,buffer);
		strcpy(buffer,"/SYS");
		strcat(buffer,TRAME_SEPARATOR);
		strcat(buffer,"501");
		send(sd, buffer, strlen(buffer), 0);
		memset(buffer,0,MAX_SIZE);
		return 1; // On ne le déconnecte pas, on lui laisse une chance
	}
}

/**
	Fonction de traitement d'une commande client
	@param buffer Le contenu la commande
	@param param Les données utiles au traitement (liste des clients, descripteurs du client...)
	@return 1 si OK, 0 sinon
*/
static int cmd_evaluation(char* buffer, Param_thread * param)
{
		// -- Récupération de toutes les données utiles au traitement des commandes
		int sd = get_descriptor(param); // SD du client courant
		info_client * listof_info_clients = get_info_clients_list(param); // Tableau de toutes les fiches clients
		info_client * sheet = get_sheet(listof_info_clients,sd); // Fiche du client courant
		TabServices * listof_services = get_listof_services(param); // Tableau de tous les services dispo (+ leurs users)
		Service * client_service = get_the_nth_service(param,sheet->service); // Service de l'utilisateur courant
		int* index_db_users = get_index_db_users(param); // L'index des lignes du fichier pour la hashtable

		// -- Variables globales utilisées pour la gestion des erreurs
		char data_error[MAX_SIZE]; // Tampon qui recevra des info à envoyer dans un message d'erreur /SYS+.....
		int connection_error; // Recevra l'état de la tentative de connexion (1 = ok // 0 = échec de connexion)

		// -- Variables pour /get et /files
		char file_content[MAX_SIZE_FILE_CONTENT]; // un buffer gros spécial fichier à transferer
		char parameter[MAX_SIZE]; // reçoit <file> du fichier de la commande /get <file>

		// -- Variables pour /adduser
		int usermanage_error; // Stock le retour erreur de /adduser
		char password[SHEET_MAX_SIZE] = {0}; // Stock le password généré lors de la commande /adduser

		/* On vérifie si la commande qui commence par / est bien une commande reconnue */
		printf("Un client utilise une commande (%d - \"%s\").\n",sd,buffer);

		// ########## GESTION DE LA COMMANDE /CONNECT ############ //
		if(!strncasecmp(buffer,"/connect",strlen("/connect")))
		{
			// Si la connexion se passe bien
			if ((connection_error = CMD_CONNECT_exec(buffer,listof_info_clients+sd,param)) == 1)
			{
				printf("Un client inconnu (%d) vient de s'authentifier...\n",sd);
				printf("---------- %s %s\n",get_firstname(sheet),get_lastname(sheet));
				printf("---------- Connecté à %s\n",get_service_char(listof_services,sheet));
				printf("---------- Statut : %s\n",get_others(sheet));

				// On envoie un message code 400 au client pour lui signaler que tout est OK
				// @400 Prénom Nom Services
				sprintf(data_error,"%s %s %s",get_firstname(sheet),
																				get_lastname(sheet),
																				get_service_char(listof_services,sheet)); // Crée une chaine pr les info facultatives
				send_data_error(sd,"400",data_error); // Envoie le code erreur + les info
				memset(buffer,0,MAX_SIZE);
				memset(data_error,0,MAX_SIZE);
				return 1;
			}
			// S'il y a une erreur de connexion
			else
			{
				printf("/!\\ ERREUR - Le client %d tente de se connecter mais sans succès (err %d - \"%s\").\n",connection_error,sd,buffer);

				// On envoie un message d'erreur 30X au client pour lui signaler l'erreur de connexion
				// @30X (ou X est un sous-code décrivant précisement le pb)
				sprintf(data_error,"%d",connection_error+300); // Convertit le sous-code erreur en char (301,302...)
				send_data_error(sd,data_error,""); // Envoie le message d'erreur
				memset(buffer,0,MAX_SIZE);
				memset(data_error,0,MAX_SIZE);
				return 1; // On ne le déconnecte pas, on lui laisse une chance
			}
		}

		// ########## GESTION DE LA COMMANDE /FILES ############ //
		else if (!strcasecmp(buffer, "/files"))
		{
			create_rawdata_fileslist(buffer);
			send(sd,buffer,strlen(buffer),0);
			return 1;
		}

		// ########## GESTION DE LA COMMANDE /GET ############ //
		else if (!strncasecmp(buffer, "/get",strlen("/get")))
		{
			// Récupère le nom du fichier à ouvrir dans parameter
			memset(parameter,MAX_SIZE_FILE_NAME,0);
			sscanf(buffer,"/get %s",parameter);

			// TODO : tester si le format du fichier est OK
			// ~~~~~~~

			printf("Un client demande un fichier partagé (%d - \"%s\").\n",sd,parameter);
			create_rawdata_filecontent(file_content,parameter);
			send(sd,file_content,strlen(file_content),0);
			return 1;
		}

		// ########## GESTION DE LA COMMANDE /SERVICES ############ //
		else if (!strcasecmp(buffer, "/services"))
		{
			// Génère une RAWLIST des services (~Service1~Service2....)
			// ... et l'envoie au client courant
			create_rawdata_serviceslist(buffer,listof_services);
			send(sd, buffer, strlen(buffer), 0);
			return 1;
		}

		else if (!strncasecmp(buffer, "/adduser",strlen("/adduser")))
		{
			// Cette commande nécessitte d'être SOP
			if(!strncmp(sheet->others,"sop",3))
			{
				// Ajoute un utilisateur et renvoie 1 si OK, 0 sinon
				usermanage_error = add_user(buffer,index_db_users,password);
				usermanage_error+=800; // pour adapter à la plage 800+ des erreurs liées à la gestion des users
				sprintf(data_error,"%d",usermanage_error);
			}
			else
			{
				strcpy(data_error,"200");
			}

			send_data_error(sd,data_error,password); // Envoie le code erreur de l'ajout
			memset(buffer,0,MAX_SIZE);
			memset(data_error,0,MAX_SIZE);
			return 1;
		}

			else if (!strcasecmp(buffer, "/help"))
		{
			// Génère une RAWLIST des services (~Service1~Service2....)
			// ... et l'envoie au client courant
			memset(buffer,MAX_SIZE,0);
			create_doc(buffer);
			send(sd, buffer, strlen(buffer), 0);
			return 1;
		}

		// ########## GESTION DE LA COMMANDE /USERS ############ //
		else if (!strcasecmp(buffer, "/users"))
		{
			// Génère une RAWLIST des users connectés au service (+User1+User2...)
			// ... et l'envoie au client courant
			create_rawdata_userslist(buffer,client_service,listof_info_clients);
			send(sd, buffer, strlen(buffer), 0);
			return 1;
		}
		// ########## GESTION DE LA COMMANDE /QUIT ############ //
		else if (!strcasecmp(buffer, "/quit"))
		{
			// On déconnecte le client (il sort de la boucle géante et libère tout)
			return 0;
		}

		// ##########  GESTION DES COMMANDES INCONNUES ####### //
		else
		{
			send_data_error(sd,"502","");
			return 1; // On ne déconnecte pas le client pour autant
		}
}

/**
	Fonction de broadcast pour diffuser le message à tout les utilisateurs d'un service
	@param buffer Le contenu du message
	@param param Les données utiles au traitement (liste des clients, descripteurs du client...)
	@return 1 si bien envoyé, 0 si erreur
*/
static int msg_evaluation(char* buffer,Param_thread * param)
{

	/* Extractions des informations vitales au traitement du message */
	int my_sd = get_descriptor(param); // SD du client courant
	info_client * sheet = get_sheet(param->info_clients_list,param->client_descriptor); // Fiche du client courant
	TabServices * listof_services = get_listof_services(param); // Tableau de tous les services disponibles

	// Génère la trame à envoyer à tous les clients au format RAWDATA
	// ([TOKEN]Message[TOKEN]Auteur[TOKEN]Service)
	create_rawdata_msg(buffer,sheet,listof_services);

	// Broadcast le buffer (rawdata_msg) à tous les utilisateurs
	// connectés au salon du client courant
	broadcast_rawdata_msg(buffer,my_sd,sheet,listof_services);

	return 1;
}

/**
	Tente de connecter un utilisateur à son service en se basant sur la DB ./data/users du serveur
	Si tout se passe bien renvoie 1, sinon renvoie un code entre 0 et -5.
	@param data Le contenu de "/connect <username> <password> <service>"
	@param sheet La fiche client à remplir (contenu dans UNE case du tableau global)
	@return 0 si aucun paramètre n'est spécifié
					1 si tout s'est bien passé
					2 s'il manque le password
					3 s'il manque le service
					4 si le service indiqué n'existe pas
					5 si le password ne coïncide pas avec la BDD
					6 l'utilisateur n'existe pas dans la BDD
*/
static int CMD_CONNECT_exec(char * data,info_client * sheet,Param_thread * param)
{
	char *pch; // Sera utilisé pour stroké DATA (/connect First Nast Services Info)
	TabServices * listof_services = get_listof_services(param); // Tableau de tous les services dispo (+ leurs users)
	int client_descriptor = get_descriptor(param); // Le SD du client courant
	int num_service_of_active_user; // Recevra le numéro du service de l'utilisateur courant
	int* index_db_users = get_index_db_users(param); // L'index des lignes du fichier pour la hashtable


	// Variables utilisées pour extraire les info de la BDD
	int index_hashtable,index_start;
	char username[SHEET_MAX_SIZE+1]; // Le login fournit dans /connect
	//UNUSED >> char password[SHEET_MAX_SIZE]; // le password fournit dans /connect
	char hash_password[SHEET_MAX_SIZE]; // le password fournit dans /connect hashé en md5
	char username_temp[SHEET_MAX_SIZE+1]; // le username visité dans le fichier
		char db_surname[SHEET_MAX_SIZE]; // Recevra l'username récupéré dans la BDD
		char db_name[SHEET_MAX_SIZE]; // .... le name
		char db_password[SHEET_MAX_SIZE]; // ... le hash md5
		char db_info[SHEET_MAX_SIZE]; // ... les info

	// Initialise les champs de la fiche
	strcpy(sheet->firstname,""); strcpy(sheet->lastname,""); sheet->service = -1; strcpy(sheet->others,""); sheet->connected=0;

	pch = strtok (data," "); // Strtok le /connect (sans le sauver nulle part bien sûr)
	if((pch = strtok (NULL, " ")) != NULL)  strcpy(username,pch); // Strtok & save l'username
	else  return 0;
	if((pch = strtok (NULL, " ")) != NULL) get_md5sum(pch,hash_password); // strock le password & récupère son hash md5
	else return 2;

	if((pch = strtok (NULL, " ")) != NULL) // Strtok & save le service
	{
		if(get_num_from_services(listof_services,pch) == -1) return 4; // Si le service tapé dans la cmd /connect n'existe pas, renvoit le code -3
		else set_service(sheet,get_num_from_services(listof_services,pch)); // Sinon, met à jour le champs "service" de la fiche "sheet"
	}
	else return 3;

	index_hashtable = hash_index_users_bd(username); // on calcule la clef du username
	index_start = index_hashtable; // L'indice pour arrêter la recherche de collision linéaires au bout d'un moment

	// Récupération du tuple à la position index_hashtable potentielle
	get_entry_db(index_hashtable-1,index_db_users,db_surname,db_name,db_password,db_info);

	while(1)
	{
		// Construit l'username du tuple en question (Nom + première lettre du prénom)
		strcpy(username_temp,db_name);
		sprintf(username_temp, "%s%c",username_temp, db_surname[0]);

		if(!strcasecmp(username_temp,username)) // si l'username a été trouvé dans la BDD à l'indice potentiel...
		{

			if(strncasecmp(hash_password,db_password,32)) // on compare les hash des password, s'ils ne coïncident pas, on dégage
				return 5; // code "mot de passe incorrect"

			else // Si password + login OK
			{
				sheet->connected = 1; // On met le statut "connected" à 1

				// Ajoute le SD de l'utilisateur dans le tableau de son service
				num_service_of_active_user = sheet->service;
				add_SD_online_user(listof_services,num_service_of_active_user,client_descriptor);

				// On met à jour la structure SHEET du client connecté
				set_firstname(sheet,db_surname);
				set_lastname(sheet,db_name);
				set_others(sheet,db_info);

				// On va broadcaster la nouvelle userlist aux autres clients du service concerné
				broadcast_rawdata_userslist(param);

				return 1;
			}
		}
		else
		{
				index_hashtable++; // Si collision, on passe à la ligne suivante (résolution linéaire bien adaptée à cette application)
				if(index_hashtable >= MAX_CLIENTS) index_hashtable = 0;

				if(index_hashtable == index_start || // Si on a tourné dans tout le fichier sans rien trouver...
					get_entry_db(index_hashtable-1,index_db_users,db_surname,db_name,db_password,db_info) == 0) // Ou si la ligne suivant la collision est vide...
				{
					return 6; //.... c'est que l'username n'existe carrément pas ! On envoie le code -5.
				}
		}
	}
}

/**
	Stock la liste des utilisateurs connecté _à un service_ au format RAW dans buffer
	USERS RAWDATA = +User1+User2+....
	@param buffer [OUT] Reçoit les rawdata
	@param client_service Les informations sur le service du client
	@param listof_info_clients Le tableau de toutes les fiches clients
*/
static void create_rawdata_userslist(char* buffer,
														Service* client_service,
														info_client * listof_info_clients)
{
	info_client * sheet_online_client; // Tampon permettant de stocker des info sur chaque client connectés
	int i, nb_users = get_nb_elements(client_service->listof_client_descriptors); // Nb de clients connectés
	memset(buffer,0,MAX_SIZE);

	strcpy(buffer,"/USERS"); // Format des rawdata (/CMD.TRAME_SEPARATOR)
	strcat(buffer,TRAME_SEPARATOR);

	for(i=0;i<nb_users;i++)
	{
		// On récupère la fiche de chaque client connecté au service via son SD
		sheet_online_client = get_sheet(listof_info_clients,valeurIemeElement_tab(client_service->listof_client_descriptors,i));
		// On concatene PRENOM NOM +....
		strcat(buffer,get_firstname(sheet_online_client));
		strcat(buffer," ");
		strcat(buffer,get_lastname(sheet_online_client));
		strcat(buffer,TRAME_SEPARATOR);
	}
}

/**
	Stock la liste des services disponibles au format RAW dans buffer
	SERVICES RAWDATA = ~Service1~Service2...
	@param buffer [OUT] Reçoit les rawdata
	@param listof_services Le tableau contenant tous les services et leurs informations
*/
static void create_rawdata_serviceslist(char * buffer,
																		TabServices * listof_services)
{
		int i;
		memset(buffer,0,MAX_SIZE);
		strcpy(buffer,"/SERVICES"); // Format des rawdata (/CMD.TRAME_SEPARATOR)
		strcat(buffer,TRAME_SEPARATOR);

		// Pour chaque service du tableau....
		for (i=0;i<listof_services->nb_services;i++)
		{
			// On concatène son nom avec ~...
			strcat(buffer,listof_services->tab[i].name);
			strcat(buffer,TRAME_SEPARATOR);
		}
}

/**
	Stock le message d'un client au format RAW dans buffer
	MSG RAWDATA = ([TOKEN]Message[TOKEN]Auteur[TOKEN]Service)
	@param buffer [OUT] Reçoit les rawdata
	@param sheet La fiche du client qui envoie le message
	@param listof_services Des info sur les services (càd la liste des utilisateurs connectés aux services)
*/
static void create_rawdata_msg(char* buffer,
															info_client * sheet,
															TabServices * listof_services)
{
	char buffer_temp[MAX_SIZE]; // tampon du message, utile à l'algo
	char token[TOKEN_SIZE]; // recevra le séparateur dans la trame broadcasté aux clients (123456message12346auteur.....)
	sprintf(token,"%d",100000+rand()%99999); // On génère ce token au format char (nombre de 100000 à 199999)
	//printf("TOKEN GÉNÉRÉ : %s\n",token);

	// on garde le message à envoyer au chaud dans buffer_temp
	memset(buffer_temp, 0,MAX_SIZE); strcpy(buffer_temp,buffer);

	/* On construit les data à envoyer au client
		|-> Format : [#TOKENprenom nomTOKENmsgTOKENserviceTOKENothers */
		memset(buffer, 0,MAX_SIZE);
		strcpy(buffer,"/MSG");
		strcat(buffer,token);
		strcat(buffer,get_firstname(sheet));
		strcat(buffer," ");
		strcat(buffer,get_lastname(sheet));
		strcat(buffer,token);
		strcat(buffer,buffer_temp);
		strcat(buffer,token);
		strcat(buffer,get_service_char(listof_services,sheet));
		strcat(buffer,token);
		strcat(buffer,get_others(sheet));
}

/**
	Broadcast le message d'un client (format rawdata) à tous les
	utilisateurs de son service.
	@param buffer Le message rawdata à broadcaster
	@param my_sd Le SD du client qui envoie le message
	@param sheet La fiche du client qui envoie le message
	@param listof_services Des info sur les services (càd la liste des utilisateurs connectés aux services)
*/
static void broadcast_rawdata_msg(char* buffer,
													int  my_sd,
													info_client * sheet,
													TabServices * listof_services)
{
	int i,a_descriptor;
	int num_of_service = get_service_int(sheet); // Le numéro du service à broadcaster
	int nb_of_clients_on_service =	get_nb_elements(listof_services->tab[num_of_service].listof_client_descriptors); // le nombre de client sur ce service

	// Pour chaque client connecté...
	for(i=0;i<nb_of_clients_on_service;i++)
	{
		// On récupère son SD
		a_descriptor = getn_client_descriptor_value(listof_services->tab[num_of_service].listof_client_descriptors,i) ;

		// Si le SD récupéré n'est pas celui du client qui broadcast...
		if(a_descriptor != my_sd)
		{
			// On envoie le message rawdata !
			printf("	* Broadcast %d to %d\n",my_sd,a_descriptor);
			send(a_descriptor, buffer, strlen(buffer), 0);
		}
	}
}

/**
	Vide les tableaux de SD (le tableau global & le tableau du salon concerné par une deconnexion)
	@param param Les param du thread qui contiennent tout ce qu'il faut
*/
static void purge_all_connections(Param_thread * param)
{
	// A la déconnexion du client, on récupère des info pour le déconnecter proprement
	int sd = get_descriptor(param);
	info_client * listof_clients = get_info_clients_list(param); // Récupère les fiches de tous les clients
	info_client * sheet = get_sheet(listof_clients,sd); // Récupère en particulier la fiche du client qui se déconnecte
	int num_service = get_service_int(sheet); // Récupère le service qu'il quitte
	TabServices * listof_services = get_listof_services(param);
	Tab_Dynamique* client_service = get_the_nth_service_tab(listof_services,num_service); // Accède aux informations du service exact
	Tab_Dynamique* descriptors_clients_list = get_descriptors_clients_list(param);

		// Déconnexion du salon
	printf("\nUn client se déconnecte du serveur (%s (%d) - %s).\n",get_lastname(sheet),sd,get_services_from_number(listof_services,num_service));

	// Si l'utilisateur ne s'est jamais connecté à un salon, on ne cherche pas à le déconnecter d'un salon
	if(sheet->connected == 1)
		remove_descriptor_form_list(client_service,sd); // Dans la liste des connectés à feu son salon

	remove_descriptor_form_list(descriptors_clients_list,sd); // Dans la liste des connectés globaux

	// Purge de la liste des clients connectés/déconnectés du-dit salon
	if(clear_old_connexions(client_service,1))
	{
		printf("Nettoyage des listes du service %d : ",num_service);
		show_descriptors_list(client_service);
	}
	// Purge de la liste des clients connectés/déconnectés
	if(clear_old_connexions(descriptors_clients_list,3))
	{
		printf("Nettoyage des listes du serveur : ");
		show_descriptors_list(descriptors_clients_list);
	}
}

/**
	Envoie un message d'information (erreur ou non) à un client SD
	sous la forme définit par le protocole du logiciel (/SYS+[CODE ] [INFO FACULTATIVE])
	@param sd Le SD du client à qui envoyer le message
	@param error_code Le numéro du code erreur à envoyer (ex : 500,400...)
	@param others Une chaine de caractères qui contient des info supplémentaires à envoyer (ou "")
*/
static void send_data_error(int sd, char* error_code,char* others)
{
	char buffer[MAX_SIZE];
	memset(buffer,0,MAX_SIZE);
	// Construit le message d'erreur
	strcpy(buffer,"/SYS");
	strcat(buffer,TRAME_SEPARATOR);
	strcat(buffer,error_code);
	strcat(buffer," ");
	strcat(buffer,others);
	// Envoie le message au client
	send(sd, buffer, strlen(buffer), 0);
}

/**
	Broadcast la liste des utilisateurs connectés à un service aux autres utilisateurs du serveur
	@param param Contient la liste de tous les utilisateurs à contacter
*/
static void broadcast_rawdata_userslist(Param_thread * param)
{

	int client_descriptor = get_descriptor(param);
	info_client * listof_info_clients = get_info_clients_list(param);
	info_client * sheet = get_sheet(listof_info_clients,client_descriptor);
	TabServices* listof_services = get_listof_services(param);
	int num_service_of_active_user = get_service_int(sheet);

	int sd_to_use,i;
	int number_of_clients = get_numberof_user(listof_services,num_service_of_active_user);
	char buffer[MAX_SIZE];

	// Pour chaque client connecté...
	for(i=0;i<number_of_clients;i++)
	{
		// On récupère le SD du i-ème client connecté au service
		sd_to_use = get_ith_sd(listof_services,num_service_of_active_user,i);
		// On envoie la nouvelle qu'aux autres clients, pas à celui qui se connecte et qui déclenche la MAJ
		if (sd_to_use != client_descriptor)
		{
			printf("Un client demande la userlist de son service (%d).\n",sd_to_use);

			// On génère la rawdata_userslist à lui envoyer, et on lui envoie !
				// 2nd param : la liste des SD des clients connectés au service concerné
				// 3nd param : les info sur eux (pour avoir leurs noms & les envoyer aux autres)
			create_rawdata_userslist(buffer,
														get_the_nth_service(param,num_service_of_active_user),
														listof_info_clients);

			send(sd_to_use,buffer,strlen(buffer),0);// Envoie de la rawdata list
		}
	}

}

/**
	Crée la liste de fichier disponible sur le serveur dans le dossier /shared/
	@param buffer La liste, retour de la fonction
*/
static void create_rawdata_fileslist(char * buffer)
{
			DIR *dirp = opendir("./shared"); // on ouvre le dossier /shared
			struct dirent *dp;
			memset(buffer,0,MAX_SIZE);

			strcpy(buffer,"/FILES"); // on commence à écrire les rawdata
			strcat(buffer,TRAME_SEPARATOR);
			while ((dp = readdir(dirp)) != NULL) // tant qu'on lit un fichier du repertoire...
			{
				// Si le fichier lu n'est pas (ou ne commence pas) par '.', et si ce n'est pas un fichier temporaire (avec ~)...
				if(dp->d_name[0] != '.' && strchr(dp->d_name,'~') == NULL)
				{
					strcat(buffer,dp->d_name); // on ajoute son nom+ dans les rawdata
					strcat(buffer,TRAME_SEPARATOR);
				}
			}
			closedir(dirp);
}

/**
	Lit un fichier partagé demandé par un utilisateur
	@param buffer Le buffer qui reçoit le contenu du fichier
	@param file Le nom du fichier à récupérer
*/
static void create_rawdata_filecontent(char * buffer, char* file)
{
	char path[MAX_SIZE_FILE_NAME+strlen("./shared")];
	char line_buffer[MAX_SIZE_FILE_CONTENT];

	strcpy(path,"./shared/");
	strcat(path,file);
	FILE * filestream = fopen(path,"r");
	if(filestream == NULL)
	{
		printf("Un client demande le fichier \"%s\" mais celui-ci n'existe pas.\n",path);
		strcpy(buffer,"/SYS");
		strcat(buffer,TRAME_SEPARATOR);
		strcat(buffer,"600");
	}
	else
	{
		memset(buffer,MAX_SIZE_FILE_CONTENT,0);
		strcpy(buffer,"/GET");
		strcat(buffer,TRAME_SEPARATOR);
		strcat(buffer,file);
		strcat(buffer,TRAME_SEPARATOR);
		while(fgets(line_buffer,MAX_SIZE_FILE_CONTENT,filestream) != NULL)
		{
			strcat(buffer,line_buffer);
		}
	}
}

/**
	Génère la documentation server-side et la met dans buffer
	@param buffer Retour de fonction, la documentation complète (commande /help)
*/
static void create_doc(char *buffer)
{
	strcpy(buffer,"/SYS+700+========================\n");
	strcat(buffer,"Bienvenue dans Diskutim !\n");
	strcat(buffer,"Voici les commandes principales du logiciel :\n");
	strcat(buffer,"	/connect <username> <password> <service> : connexion au serveur\n");
	strcat(buffer,"	/services : affiche la liste des services disponibles\n");
	strcat(buffer,"	/users : affiche la liste des utilisateurs connectés à votre services\n");
	strcat(buffer,"	/files : affiche la liste des fichiers partagés\n");
	strcat(buffer,"	/get <fichier> : affiche le contenu de <fichier>\n");
	strcat(buffer,"	/adduser <prenom> <nom> <statut> : ajoute un utilisateur (none,hop,aop,sop)\n");
	strcat(buffer,"	/quit : vous deconnecte du serveur\n");
	strcat(buffer,"Enjoy ! :-)\n");
	strcat(buffer,"========================\n");
}

