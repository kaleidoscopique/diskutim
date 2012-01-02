#ifndef __MANAGE_FILES
#define __MANAGE_FILES

int hash_index_users_bd(char* username);
void get_md5sum(const char* buffer,char* md5sum);
int add_user(char* buffer,int* index_db_users,char* password);

#endif
