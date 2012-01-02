#ifndef __GTKGUI_MAIN
#define __GTKGUI_MAIN
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "../../network/manage_threads.h"
#include "gtkgui_create_widgets.h"

// Structure utilisée pour passer des paramètres de callback lors de l'ouverture de la fenêtre de shared files
typedef struct {
	GtkWidget* window;
	GtkWidget* treeview_files;
	GtkWidget* textview_files;
	int sd; // Le SD : un peu "bricolage" pour simplifier le code, pas forcément très cohérent cependant...
} Param_callback_sharedfiles;

// Structure utilisée pour passer des paramètres de callback lors de l'envoi de message
// Widget important qui seront mis à jour après
typedef struct {
	GtkWidget* discussion_window; // La fenêtre elle même
	GtkWidget* textview_discussion; // Le texteview qui affiche les discussions
	GtkWidget* treeview_users_list; // La userlist
	GtkWidget* description_label; // Le petit label "vous êtes connecté entant que..."
	GtkWidget* entry_msg; // Le petit label "vous êtes connecté entant que..."
	Param_callback_sharedfiles * GTKW_SharedfW;
	Param_thread * param_thread;
} Param_callback_discussion;

// Structure utilisée pour passer des paramètres de callback lors de la connexion (transition connexion->discussion)
typedef struct {
	Param_thread * param_thread; // les paramètres du thread
	GtkWidget* entry_username; // Récupère le widget identity_name
	GtkWidget* entry_password; // Récupère le widget identity_surname
	GtkWidget* treeview_services; // Récupère le widget treeview pour chopper le service
	GtkWidget* main_window; // Le widget de la fenètre principale
	GtkWidget* discussion_window; // le widget de la fenètre de discussion principale
	GtkWidget * description_label; // Le label de la discussion_window qui affiche qq info sur l'utilisateur
	int client_descriptor; // le SD du client
} Param_callback_connection;

void callback_cliked_button_connection(GtkWidget* widget, gpointer data);

void callback_clicked_button_sharedfiles (GtkWidget* widget, gpointer widget_param);
void callback_clicked_button_readfile(GtkWidget* widget, gpointer gparam);
void gtkgui_main (Param_thread * param,int sd);
void gtkgui_discussion (GtkWidget* window,GtkWidget* description_label, int sd,const char* client_username,char* client_services);

#endif
