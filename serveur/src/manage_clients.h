#ifndef _MANAGE_CLIENTS
#define _MANAGE_CLIENTS
#include "manage_threads.h"
#include "TAD_tabDyn.h"



/* ACCESSEURS */

char* get_firstname(info_client * sheet);
char* get_lastname(info_client * sheet);
int get_service_int(info_client * sheet);
char* get_service_char(TabServices * listof, info_client * sheet);
char* get_others(info_client * sheet);
int get_descriptor(Param_thread * param);
int get_connected(info_client * sheet);
info_client* get_info_clients_list(Param_thread * param);
info_client* get_sheet(info_client* listof, int sd);
int getn_client_descriptor_value(Tab_Dynamique* listof, int n);
Tab_Dynamique* get_descriptors_clients_list(Param_thread * param);

/* MUTATEURS */

void set_firstname(info_client * sheet,char *firstname);
void set_lastname(info_client * sheet,char *lastname);
void set_service(info_client * sheet,int service);
void set_others(info_client * sheet,char *others);
void remove_descriptor_form_list(Tab_Dynamique* listof, int sd);
void add_new_descriptor(Tab_Dynamique* listof, int sd);
void show_descriptors_list(Tab_Dynamique* listof);

/* Accès à la BDD */

int parse_users_db(int* index);
int get_entry_db(int num_index,const int* index_bd_users,char* surname,char* name,char* password,char* info);

#endif
