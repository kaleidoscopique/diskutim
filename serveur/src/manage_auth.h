#ifndef __MANAGE_FILES
#define __MANAGE_FILES

int hash_index_users_bd(char* username);
void get_md5sum(const char* buffer,char* md5sum);
int add_user(char* buffer,int* index_db_users,char* password);
int parse_users_db(int* index);
int get_entry_db(int num_index,const int* index_bd_users,const char* surname,const char* name,const char* password,const char* info);

#endif
