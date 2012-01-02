#ifndef _MANAGE_SERVICES
#define _MANAGE_SERVICES
#include "../inc_define.h"

typedef struct
{
		int nb_services;
		char tab[ MAX_SERVICES][MAX_SIZE_SERVICE_NAME];
} Services;

void get_listof_services(char* data, Services* listof_services);
void show_services_list(Services* listof);

#endif
