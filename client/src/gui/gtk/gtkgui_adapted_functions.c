#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "../../inc_define.h"
#include "../../inc_alllib.h"
#include "../../network/manage_services.h"
#include "../../network/manage_clients.h"
#include "../../network/manage_threads.h"
#include "../../network/manage_protocole.h"
#include "gtkgui_adapted_functions.h"
#include "gtkgui_create_widgets.h"

void gtkgui_show_file_content(char* content, char* file_title, void* textview_file)
{
	mygtk_clear_textview (GTK_TEXT_VIEW(textview_file));
	mygtk_append_text_to_textview(GTK_TEXT_VIEW(textview_file),content);
}

void gtkgui_show_files_list(char listof[][MAX_SIZE_FILE_NAME],int nb_files, void* treeview_sharedfiles)
{
	GtkListStore  *store;
	GtkTreeIter iter;
	int i;

	// Création d'un magasin de données pour stocker le contenu de mon treeview
	// Le 1 correspond au nombre de colonnes dans la treeview
	store = gtk_list_store_new (1, G_TYPE_STRING, G_TYPE_UINT);

	// Ajout d'un element dans le magasin et définition de l'élément
	// le 0 est l'ID de la colonne du treeview. Cet ID est défini dans [mygtk_create_treeview_1column]
	for (i = 0; i<nb_files;i++)
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,0, listof[i],-1);
	}

	// On bind le magasin avec la treeview pour afficher les données
	gtk_tree_view_set_model (GTK_TREE_VIEW(treeview_sharedfiles), GTK_TREE_MODEL(store));
	// On demande au magasin de se détruire en même temps que le treeview (nettoyeur)
	g_object_unref(GTK_TREE_MODEL(store));
}

void gtkgui_show_services_list(Services* listof,void* treeview)
{
	GtkListStore  *store;
	GtkTreeIter iter;
	int nb_services = listof->nb_services,i;

	// Création d'un magasin de données pour stocker le contenu de mon treeview
	// Le 1 correspond au nombre de colonnes dans la treeview
	store = gtk_list_store_new (1, G_TYPE_STRING, G_TYPE_UINT);

	// Ajout d'un element dans le magasin et définition de l'élément
	// le 0 est l'ID de la colonne du treeview. Cet ID est défini dans [mygtk_create_treeview_1column]
	for (i = 0; i<nb_services;i++)
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,0, listof->tab[i],-1);
	}

	// On bind le magasin avec la treeview pour afficher les données
	gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
	// On demande au magasin de se détruire en même temps que le treeview (nettoyeur)
	g_object_unref(GTK_TREE_MODEL(store));
}


/**
	MAJ la userslist côté client GTK
	@param listof La structure qui list les clients
	@param treeview Le widget à mettre à jour
	@note Si listof = NULL, on vide la liste (utile pour l'effet visuel "déconnexion")
*/
void gtkgui_show_users_list(Clients* listof,void* treeview)
{
	GtkListStore  *store;
	GtkTreeIter iter;
	int nb_online_clients,i;

	// Création d'un magasin de données pour stocker le contenu de mon treeview
	// Le 1 correspond au nombre de colonnes dans la treeview
	store = gtk_list_store_new (1, G_TYPE_STRING, G_TYPE_UINT);

	// Si le paramètre n'est pas NULL, on met bien à jour la liste...
	if(listof != NULL)
	{
		// Ajout d'un element dans le magasin et définition de l'élément
		// le 0 est l'ID de la colonne du treeview. Cet ID est défini dans [mygtk_create_treeview_1column]
		nb_online_clients = listof->nb_online_clients;
		for (i = 0; i<nb_online_clients;i++)
		{
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter,0, listof->tab[i],-1);
		}
	}
	else
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,0,"-",-1);
	}

	// On bind le magasin avec la treeview pour afficher les données
	gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
	// On demande au magasin de se détruire en même temps que le treeview (nettoyeur)
	g_object_unref(GTK_TREE_MODEL(store));
}

/**
	Fonction d'affichage des erreurs serveurs ET des messages broadcasts pour GTK
	@param textview Le widget textview à MAJ
	@param window_connection Le widget de la fenêtre de connexion
	@param author L'auteur du message
	@param msg Le message à afficher ou...
	@param services Le service concerné par le message
	@param code le code erreur à afficher
	@param content_msg Le contenu brute
	@note si code = 0, il s'agit d'un msg (cette fonction fait tout à la fois)
*/
void gtkgui_show_error_and_msg(void* textview,void* window_connection,char* author,
															char* msg, char* services,int code,char* content_msg)
{
	char codeErr[MAX_SIZE]; // Reçoit (au besoin) un texte correspondant au code erreur "code"
	GtkWidget* view;
	char buffer[MAX_SIZE];

	// Si la réponse serveur n'est pas un code
	if(code == 0)
	{
		view = (GtkWidget*) textview;
		// On crée la chaine de caractère à afficher sur le textview
		sprintf(buffer,"<%s> %s\n",author,msg);
		mygtk_append_text_to_textview(GTK_TEXT_VIEW(view),buffer);
	}
	// Si c'est un code
	else
	{
		// Si c'est un code d'erreur de connexion, ...
		if(code >= 300 && code <= 306)
		{
			// ... alors on affiche la fenêtre de connexion pour recommencer
			gtk_widget_show(window_connection);
		}
		codeErr2char(codeErr,code,content_msg);
		mygtk_append_text_to_textview(textview,codeErr);
	}
}

/**
	Ajuste les param du thread pour utiliser les bonnes primitives en fonction de la librairie utilisée
	@param param L'adresse du pointeur sur la structure
	@note Ici on s'occupe de GTK
*/
void init_functions_thread_for_gtk(Param_thread** param)
{
	(*param)->adapted_show_services_list = gtkgui_show_services_list;
	(*param)->adapted_show_users_list = gtkgui_show_users_list;
	(*param)->adapted_show_error_and_msg = gtkgui_show_error_and_msg;
	(*param)->adapted_show_files_list = gtkgui_show_files_list;
	(*param)->adapted_show_file_content = gtkgui_show_file_content;
}
