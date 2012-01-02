#include "../inc_define.h"
#include "../inc_alllib.h"
#include "manage_threads.h"
#include "../gui/nogui/nogui_adapted_functions.h"


/**
	Crée un nouveau thread lors d'une connexion client
	@param f La fonction de dialogue clietn<=>serveur
	@param arg Le descripteur socket client (type void* pour _create)
*/

pthread_t new_thread (void *(*f)(void*), void *arg)
{
	pthread_t pth;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	// Lancement du thread
	pthread_create(&pth, &attr, f, arg);
	pthread_attr_destroy(&attr);
	return pth;
}

/**
	Crée une variable de type PARAM_THREAD relative à un UNIQUE client
	Permet de passer facilement des paramètres au thread client concerné
	@param param L'adresse du pointeur sur la structure
*/

void init_param_thread(Param_thread** param)
{
	(*param) = (Param_thread*) malloc(sizeof(Param_thread));
}

/**
	Remplit une variable de type PARAM_THREAD avant de créer un thread
	@param param L'adresse du pointeur sur la structure
	@param T La liste globale des utilisateurs connectés au serveur
	@param client_descriptor Le descripteur de socket relatif au client concerné par le thread
*/

void bind_param_thread(Param_thread** param,
											int client_descriptor,
											void* treeview_services_list,
											void* treeview_users_list,
											void* textview_discussion,
											void* treeview_files_list,
											void* textview_file_content,
											void* window_connection,
											void* window_discussion)
{
	(*param)->client_descriptor = client_descriptor;
	// Quand on lance un thread pour l'application GTK, on se met de côté les pointeurs ...
	// ... vers les widgets à MAJ pendant le fonctionnement
	(*param)->treeview_services_list = (void*) treeview_services_list;
	(*param)->treeview_users_list = (void*) treeview_users_list;
	(*param)->textview_discussion = (void*) textview_discussion;
	(*param)->treeview_files_list = (void*) treeview_files_list;
	(*param)->textview_file_content = (void*) textview_file_content;
	(*param)->window_connection = (void*) window_connection;
	(*param)->window_discussion = (void*) window_discussion;
}
