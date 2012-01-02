#ifndef __MANAGE_CLIENTS
#define __MANAGE_CLIENTS
#include "../inc_define.h"

typedef struct
{
		int nb_online_clients;
		char tab[MAX_CLIENTS][SHEET_MAX_SIZE*2];
} Clients;

void get_listof_online_clients(char* data, Clients* listof_clients);

#endif
