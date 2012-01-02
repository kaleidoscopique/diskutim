#include "inc_define.h"
#include "inc_alllib.h"
#include "manage_threads.h"

/**
	Crée un nouveau thread lors d'une connexion client
	@param f La fonction de dialogue clietn<=>serveur
	@param arg Le descripteur socket client (type void* pour _create)
*/

void new_thread (void *(*f)(void*), void *arg)
{
	pthread_t pth;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	// On crée le thread avec la fonction de dialogue
	// f = fn de dialogue
	// arg = socket client descripteur (en void*)
	pthread_create(&pth, &attr, f, arg);
	pthread_attr_destroy(&attr);
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

void bind_param_thread(Param_thread** param, Tab_Dynamique* T, int client_descriptor, info_client * listof_info_clients, TabServices * services_list,int * index_db_users)
{
	(*param)->clients_list = T;
	(*param)->client_descriptor = client_descriptor;
	(*param)->info_clients_list = listof_info_clients;
	(*param)->services_list = services_list;
	(*param)->index_db_users = index_db_users;
}
