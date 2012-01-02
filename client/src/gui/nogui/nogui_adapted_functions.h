#ifndef __NOGUI_ADAPTED_FUNCTIONS
#define __NOGUI_ADAPTED_FUNCTIONS
#include "../../network/manage_services.h"
#include "../../network/manage_threads.h"

void nogui_show_services_list(Services* listof,void* useless);
void nogui_show_users_list(Clients* listof,void* useless);
void nogui_show_error_and_msg(void* ,void* ,char* author,char* msg, char* services,int code);
void init_functions_thread_for_nogui(Param_thread** param);

#endif
