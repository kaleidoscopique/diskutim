#ifndef _MANAGE_PROTOCOLE
#define _MANAGE_PROTOCOLE
#include "../inc_define.h"

int lecture_stdin(char * chaine, int size_of);
void complete_data_msg(char*data, char* author,char* msg,char* service, char* others);

int get_type_buffer(char* buffer);
int is_a_usrmsg(int);
int is_listof_users(int);
int is_a_sysmsg(int);
int is_listof_services(int);

int sysmsg2int(char* data);
void codeErr2char(char *, int,char *);

#endif
