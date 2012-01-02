#ifndef _MANAGE_SOCKETS
#define _MANAGE_SOCKETS
#include "../inc_define.h"

void create_socketC2S(int * sd,struct sockaddr_in * addr);
void *check_server_reply(void *);

#endif
