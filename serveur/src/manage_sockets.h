#ifndef _MANAGE_SOCKETS
#define _MANAGE_SOCKETS
#include "TAD_tabDyn.h"

void who_connected(int);
int at_least_one_client (const Tab_Dynamique* liste_clients);
void create_socketS2C(int * sd_serv,struct sockaddr_in * addr);
int clear_old_connexions (Tab_Dynamique * clients_list, int nb_connexions);

#endif
