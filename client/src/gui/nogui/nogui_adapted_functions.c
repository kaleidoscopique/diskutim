#include "../../inc_alllib.h"
#include "../../inc_define.h"
#include "../../network/manage_services.h"
#include "../../network/manage_clients.h"
#include "../../network/manage_threads.h"
#include "../../network/manage_protocole.h"

/**
	Affiche la liste des fichiers shared dispo sur le serveur
	@param listof Le tableau avec le nom des fichiers
	@param useless le GTKwidget osef
*/
void nogui_show_files_list(char listof[][MAX_SIZE_FILE_NAME],int nb_files, void* useless)
{
	int i;
	printf("Liste des fichiers partagés sur le serveur : \n");
	for(i=0;i<nb_files;i++)
	{
		printf("- %s\n",listof[i]);
	}

}

void nogui_show_file_content(char* content, char* file_title, void* useless)
{
	printf("\n======== %s ========\n",file_title);
	printf("%s",content);
	printf("\n =====================\n");
}

/**
	Affiche la liste des services disponibles sur le serveur
	@param listof La liste des services dispo
	@param useless Le gtkwidget osef
*/
void nogui_show_services_list(Services* listof,void* useless)
{
	printf("Liste des services disponibles : \n");
	int nb_services = listof->nb_services,i;
	for (i = 0; i<nb_services;i++)
	{
		printf("- %s\n",listof->tab[i]);
	}
}

/**
	Affiche la liste des utilisateurs suite à /users côté NOGUI
	@param listof La liste des clients connectés
	@param useless le Gtkwidget osef
*/
void nogui_show_users_list(Clients* listof,void* useless)
{
	printf("Liste des utilisateurs connectés à votre service : \n");
	int nb_users = listof->nb_online_clients,i;
	for (i = 0; i<nb_users;i++)
	{
		printf("- %s\n",listof->tab[i]);
	}
}

/**
	Gère l'affichage des messages systèmes ET des messages utilisateurs serveur côté NOGUI
	@param useless Le gtkwidget dont on se fout ici
	@param author l'auteur du message
	@param msg Le message
	@param service Le service de l'utilisateur
	@param code Le code du message SYS (si code != "", alors author msg et service == "", et vis et versa)
*/
void nogui_show_error_and_msg(void* useless,void* useless_,char* author,char* msg, char* service,int code,char* buffer)
{
	char codeErr[MAX_SIZE]; // Reçoit (au besoin) un texte correspondant au code erreur "code"

	if(code == 0)
	{
		printf("(%s) %s dit : %s\n",service,author,msg); // On affiche le message à l'écran
	}
	else
	{
		codeErr2char(codeErr,code,buffer);
		printf("%s",codeErr);
	}
}


/**
	Ajuste les param du thread pour utiliser les bonnes primitives en fonction de la librairie utilisée
	@param param L'adresse du pointeur sur la structure
	@note Ici on s'occupe du NOGUI
*/
void init_functions_thread_for_nogui(Param_thread** param)
{
	(*param)->adapted_show_services_list = nogui_show_services_list;
	(*param)->adapted_show_users_list = nogui_show_users_list;
	(*param)->adapted_show_error_and_msg = nogui_show_error_and_msg;
	(*param)->adapted_show_files_list = nogui_show_files_list;
	(*param)->adapted_show_file_content = nogui_show_file_content;
}
