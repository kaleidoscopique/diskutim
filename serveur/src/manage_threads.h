#ifndef _MANAGE_THREADS
#define _MANAGE_THREADS
#include "TAD_tabDyn.h"
#include "manage_protocole.h"
#include "manage_services.h"
#include "inc_define.h"

/**
	Cette structure  est une FICHE de client (contenu ensuite dans un TABLEAU de fiche de clients
	Elle n'a rien à faire ici, elle devrait aller dans manage_clients.h MAIS erreur de compilation.....
*/

typedef struct {
	int connected;
	char firstname[SHEET_MAX_SIZE];
	char lastname[SHEET_MAX_SIZE];
	int service;
	char others[SHEET_MAX_SIZE];
} info_client;


/** Param_thread contient les variables globales utiles à tous les clients connectés
	* -> liste_client 				| Tableau de descripteur clients connectés
	* -> descripteur_client 	| Le SD du client associé au thread
	* -> info_clients_list 		| La liste des fiches de tous les clients
	* -> services_list		 		| La liste de tous les services dispo
	* -> index_db_users		| L'index "num de ligne => position octet" du fichier ./data/users
*/

typedef struct {
	Tab_Dynamique* clients_list;
	int client_descriptor;
	info_client * info_clients_list;
	TabServices* services_list;
	int * index_db_users;
} Param_thread;


void new_thread(void*(*)(void *), void *);

void init_param_thread(Param_thread** param);
void bind_param_thread(Param_thread* param, Tab_Dynamique* T, int client_descriptor, info_client * listof_info_clients, TabServices * services_list,int * index_db_users);

#endif
