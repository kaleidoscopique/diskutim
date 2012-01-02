#ifndef __GTKGUI_ADAPTED_FUNCTIONS
#define __GTKGUI_ADAPTED_FUNCTIONS
#include "../../network/manage_services.h"
#include "../../network/manage_clients.h"

void gtkgui_show_services_list(Services* listof,void* useless);
void gtkgui_show_users_list(Clients* listof,void* treeview);
void gtkgui_show_error_and_msg(void* textview,void* window_connection,char* author,char* msg, char* services,int code,char* buffer);
void init_functions_thread_for_gtk(Param_thread** param);

#endif

