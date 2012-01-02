#ifndef _THREAD_SOCKET
#define _THREAD_SOCKET
#include "../inc_define.h"
#include "../inc_alllib.h"
#include "manage_services.h"
#include "manage_clients.h"

typedef struct {
	int client_descriptor;
	void* treeview_services_list; // Le widget de la liste des services (connexion)
	void* treeview_users_list; // Le widget de la liste des utilisateurs (discussion)
	void* textview_discussion; // Le widget d'affichage des discussions (discussion)
	void* treeview_files_list; // Le widget d'affichage des fichiers shared (shared)
	void* textview_file_content; // Le widget d'affichage du contenu d'un fichier shared (shared)
	void* window_connection; // Le widget de la fenêtre de connexion
	void* window_discussion; // Le widget de la fenêtre de discussion

	// Les primitives adaptées à la librairie (void* = GTKWIDGET* déguisé pour la généricité de la compilation)
	void (*adapted_show_services_list) (Services*, void*);
	void (*adapted_show_users_list) (Clients*, void*);
	void (*adapted_show_error_and_msg) (void* ,void*, char*,char*, char*,int,char*);
	void (*adapted_show_files_list)(char[][MAX_SIZE_FILE_NAME],int, void*);
	void (*adapted_show_file_content)(char*,char*, void*);
} Param_thread;

pthread_t new_thread(void*(*)(void *), void *);
void init_param_thread(Param_thread** param);
void bind_param_thread(Param_thread** param,
											int client_descriptor,
											void* treeview_services_list,
											void* treeview_users_list,
											void* textview_discussion,
											void* treeview_files_list,
											void* textview_file_content,
											void* window_connection,
											void* window_discussion);

#endif
