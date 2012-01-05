#ifndef _MANAGE_CLIENTS
#define _MANAGE_CLIENTS
#include "manage_threads.h"
#include "TAD_tabDyn.h"



/* ACCESSEURS */

int* get_index_db_users(const Param_thread* param);
const char* get_firstname(const info_client * sheet);
const char* get_lastname(const info_client * sheet);
int get_service_int(const info_client * sheet);
const char* get_service_char(const TabServices * listof, const info_client * sheet);
const char* get_others(const info_client * sheet);
int get_descriptor(const Param_thread * param);
int get_connected(const info_client * sheet);
info_client* get_info_clients_list(const Param_thread * param);
info_client* get_sheet(const info_client* listof, int sd);
int getn_client_descriptor_value(const Tab_Dynamique* listof, int n);
Tab_Dynamique* get_descriptors_clients_list(const Param_thread * param);
void show_descriptors_list(const Tab_Dynamique* listof);

/* MUTATEURS */

void set_firstname(info_client * sheet,char *firstname);
void set_lastname(info_client * sheet,char *lastname);
void set_service(info_client * sheet,int service);
void set_others(info_client * sheet,char *others);
void remove_descriptor_form_list(Tab_Dynamique* listof, int sd);
void add_new_descriptor(Tab_Dynamique* listof, int sd);

#endif
