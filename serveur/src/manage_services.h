#ifndef _MANAGE_SERVICES
#define _MANAGE_SERVICES
#include "inc_define.h"
#include "TAD_tabDyn.h"
#include "manage_threads.h"

typedef struct
{
		char name[MAX_SIZE_SERVICE_NAME];
		Tab_Dynamique * listof_client_descriptors;
} Service;

typedef struct
{
	int nb_services;
	Service tab[MAX_SERVICES];
} TabServices;

void complete_services_tab(TabServices * listof);
const char* get_services_from_number(const TabServices * listof, int number);
int get_num_from_services(const TabServices * listof, char* services);
int get_numberof_user(const TabServices * listof, int num_of_service);
void add_SD_online_user(TabServices* listof, int num_of_service, int sd);
int get_ith_sd(const TabServices* listof, int num_of_service, int i);

#endif
