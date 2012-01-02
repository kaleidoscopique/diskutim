#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "../../inc_define.h"
#include "../../inc_alllib.h"
#include "../../network/manage_threads.h"
#include "../../network/manage_sockets.h"
#include "../../network/manage_protocole.h"
#include "../../network/manage_services.h"
#include "../../network/manage_clients.h"
#include "gtkgui_main.h"
#include "gtkgui_adapted_functions.h"

/**
	Boucle principale pour gérer l'affichage du client /w GTK GUI
	@param param Les paramètres à passer au thread
	@param sd Le descripteur client
	@note Le thread sera lancé après GTK pour récupérer des pointeurs sur certains widgets à MAJ
*/
void gtkgui_main(Param_thread * param,int sd)
{
	/* Initialisation de GTK+ */
    gtk_init(0,NULL);

	// Les différents widget de l'interface qui sont réutilisés un peu partout par la suite
	Param_callback_discussion GTKW_DiscW; // on utilise cette structure pour récupérer les widgets discussion après la connexion
	Param_callback_sharedfiles GTKW_SharedfW; // on utilise cette structure lorsqu'on ouvre la fenêtre des shared files
		GTKW_SharedfW.sd = sd; // bricole : on stock le SD dans cette structure que l'on passe dans un callback (pratique mais pas bien propre)

/* ============================================
 CREATION DE LA FENETRE DE RECEPTION DE FICHIERS A PRIORI
 ============================================= */

	// Création de la fenêtre et de la table
	GTKW_SharedfW.window = mygtk_create_widget_window(500,300,GTK_WINDOW_TOPLEVEL);
	gtk_window_set_deletable(GTK_WINDOW(GTKW_SharedfW.window),FALSE);
	GtkWidget * SharedfW_table = gtk_table_new(3,3,FALSE);
	gtk_container_add (GTK_CONTAINER (GTKW_SharedfW.window), SharedfW_table); // bind la table

	// Créer le widget treeview de la liste des fichiers
	GtkWidget* SharedfW_scroll_window;
	mygtk_create_treeview_1column("Fichiers partagés",&GTKW_SharedfW.treeview_files,&SharedfW_scroll_window);
		gtk_table_attach(GTK_TABLE(SharedfW_table),SharedfW_scroll_window,0,1,1,2,GTK_EXPAND | GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL | GTK_EXPAND,0,0);

	// Création du text buffer scrollable
	GtkWidget *SharedfW_scroll_window_buffer;
	mygtk_create_textview_discussion(&SharedfW_scroll_window_buffer,&GTKW_SharedfW.textview_files);
		gtk_widget_set_can_focus(GTKW_SharedfW.textview_files,FALSE);
		gtk_table_attach(GTK_TABLE(SharedfW_table),SharedfW_scroll_window_buffer,2,3,1,2,GTK_EXPAND | GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,0,0);

	// Créer le bouton de téléchargement d'un fichier
	GtkWidget* read_button = gtk_button_new_with_label("Lire le fichier");
		gtk_table_attach(GTK_TABLE(SharedfW_table),read_button,0,1,2,3,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);
		g_signal_connect(G_OBJECT(read_button), "clicked", G_CALLBACK(callback_clicked_button_readfile), (gpointer)&GTKW_SharedfW);

	// Close button
	GtkWidget* close_button = gtk_button_new_with_label("Fermer la fenêtre");
		gtk_table_attach(GTK_TABLE(SharedfW_table),close_button,2,3,2,3,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);
		g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(mygtk_close_window), (gpointer)GTKW_SharedfW.window);

/* ====================================
 CREATION DE LA FENETRE DE DISCUSSION A PRIORI
 ===================================== */

 	// On incorpore les widget de la window SharedFiles dans ceux de la Window Discussion
	GTKW_DiscW.GTKW_SharedfW = &GTKW_SharedfW;
	GTKW_DiscW.param_thread = param;

	// On crée entièrement la fenètre de discussion sans l'affiche pour le moment
	// ... on l'affichera par la suite, après la connexion.
	// ... les widgets de cette fenètre qui nous intéressent sont stockés dans GTKW_DiscW;
	GTKW_DiscW.discussion_window = mygtk_create_widget_window(850,500,GTK_WINDOW_TOPLEVEL);

	// Création d'une table pour contenir les éléments de taille 10*10
	GtkWidget * table = gtk_table_new(8,10,FALSE);
	gtk_container_add (GTK_CONTAINER (GTKW_DiscW.discussion_window), table); // bind la masterbox à la fenètre

	// Création du text buffer scrollable
	GtkWidget *swindow;
	mygtk_create_textview_discussion(&swindow,&GTKW_DiscW.textview_discussion);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow),
																GTK_POLICY_NEVER,GTK_POLICY_NEVER);
		gtk_widget_set_can_focus(GTKW_DiscW.textview_discussion ,FALSE);
		gtk_table_attach(GTK_TABLE(table),swindow,0,7,0,7,GTK_EXPAND | GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);

	// Créer le widget treeview de la liste des connectés
	GtkWidget* scroll_window;
	mygtk_create_treeview_1column("Utilisateurs connectés",&GTKW_DiscW.treeview_users_list,&scroll_window);
		gtk_table_attach(GTK_TABLE(table),scroll_window,7,9,0,5,GTK_EXPAND | GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL | GTK_EXPAND,10,10);

	// Créer le widget ENTRY pour discuter
	GTKW_DiscW.entry_msg = mygtk_new_entrytext(MAX_SIZE);
		gtk_widget_set_can_focus(GTKW_DiscW.entry_msg ,TRUE);
		gtk_widget_grab_focus(GTKW_DiscW.entry_msg);
		gtk_table_attach(GTK_TABLE(table),GTKW_DiscW.entry_msg,0,6,7,8,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);

	// Créer le bouton d'envoi d'un message (en mode DEFAULT pour réagir à la touche entrée)
	GtkWidget* sendmsg_button = gtk_button_new_with_label("Envoyer");
		gtk_widget_set_can_default(sendmsg_button,TRUE); // Met le flag CAN_DEFAULT sur le bouton
		gtk_table_attach(GTK_TABLE(table),sendmsg_button,6,7,7,8,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10); // important à faire avant grab_defaut (need intégration à window)
		gtk_widget_grab_default(sendmsg_button); // Active le mode DEFAULT (possible car flag OK)
		gtk_window_set_default(GTK_WINDOW(GTKW_DiscW.discussion_window),sendmsg_button); // Associe la fenêtre au bouton default

	// Créer le bouton d'envoi d'un message (en mode DEFAULT pour réagir à la touche entrée)
	GtkWidget* sharedfiles_button = gtk_button_new_with_label("Fichiers partagés");
		gtk_table_attach(GTK_TABLE(table),sharedfiles_button,7,8,7,8,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);
		g_signal_connect(G_OBJECT(sharedfiles_button), "clicked", G_CALLBACK(callback_clicked_button_sharedfiles), (gpointer)&GTKW_DiscW);

	// Bind les param du callback send_msg
		g_signal_connect(G_OBJECT(sendmsg_button), "clicked", G_CALLBACK(callback_clicked_button_sendmsg), (gpointer)&GTKW_DiscW);

	// On crée un label qui affiche un petit texte d'information sur la connexion
	// "Vous êtes connecté entant que <Prénom> <Nom> (<Nom du service>)"
	GTKW_DiscW.description_label = gtk_label_new(NULL);
	// Le texte du label sera mis à jour lors de la connexion (pour afficher prénom+nom+service)
		gtk_table_attach(GTK_TABLE(table),GTKW_DiscW.description_label,0,6,8,9,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,5);

/* ====================================
 CREATION DE LA FENETRE DE CONNECTION
 ===================================== */

	// Création de la liste des services (widget MAJ par le thread lors d'une réponse serveur)
	// Il faut laisser ce widget ici (en haut) car on en a besoin dès le lancement du thread
	GtkWidget* treeview_services;
	GtkWidget* scroll_window_services;
	mygtk_create_treeview_1column("Quel est votre service ?",&treeview_services,&scroll_window_services);

	// Création de la fenêtre avant toute chose pour la même raison
    GtkWidget* main_window = mygtk_create_widget_window(300,450,GTK_WINDOW_TOPLEVEL);
    gtk_window_set_keep_above(GTK_WINDOW(main_window),TRUE);

	// Création du thread avec récupération des widgets GTK utiles
	bind_param_thread(&param,sd,treeview_services,
										GTKW_DiscW.treeview_users_list,
										GTKW_DiscW.textview_discussion,
										GTKW_SharedfW.treeview_files,
										GTKW_SharedfW.textview_files,
										main_window,
										GTKW_DiscW.discussion_window);
	init_functions_thread_for_gtk(&param);
	new_thread(check_server_reply,(void*)param);

	// Le client demande la liste des services pour remplir le widget
	send(sd, "/services", strlen("/services"), 0);

	// Création de la table qui fait tout tenir
	GtkWidget * table_disc = gtk_table_new(4,14,FALSE);
		gtk_container_add (GTK_CONTAINER (main_window), table_disc); // bind de la table à la fenètre
		gtk_table_attach(GTK_TABLE(table_disc),scroll_window_services,0,4,7,12,GTK_EXPAND | GTK_SHRINK | GTK_FILL,GTK_EXPAND | GTK_SHRINK | GTK_FILL,10,10);

	// Ajout du logo de Diskutim
	GtkWidget* logo_image = gtk_image_new_from_file("./data/img/logo.jpg");
		gtk_table_attach(GTK_TABLE(table_disc),logo_image,0,4,0,3,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);

	// Création du label de connexion
	GtkWidget* label_form[2] ;
	label_form[0] =gtk_label_new("Votre login :");
	label_form[1] =gtk_label_new("Votre password :");
		gtk_table_attach(GTK_TABLE(table_disc),label_form[0],2,3,5,6,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);
		gtk_table_attach(GTK_TABLE(table_disc),label_form[1],2,3,6,7,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);

	// Création du champs de formulaire (identité)
	GtkWidget* entry_identity[2];
	entry_identity[0] = mygtk_new_entrytext(SHEET_MAX_SIZE);
		gtk_widget_set_can_focus(entry_identity[0],TRUE);
		gtk_widget_grab_focus(entry_identity[0]);
		gtk_table_attach(GTK_TABLE(table_disc),entry_identity[0],3,4,5,6,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);
	entry_identity[1] = mygtk_new_entrytext(SHEET_MAX_SIZE);
	gtk_entry_set_visibility(GTK_ENTRY(entry_identity[1]),FALSE);
		gtk_table_attach(GTK_TABLE(table_disc),entry_identity[1],3,4,6,7,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);

	// Création du bouton de connexion en mode default
	GtkWidget* connection_button;
	connection_button = gtk_button_new_with_label("Se connecter");
		gtk_widget_set_can_default(connection_button,TRUE);
		gtk_table_attach(GTK_TABLE(table_disc),connection_button,0,4,12,13,GTK_SHRINK | GTK_FILL,GTK_SHRINK | GTK_FILL,10,10);
		gtk_widget_grab_default(connection_button);
		gtk_window_set_default(GTK_WINDOW(main_window),connection_button);
	// Mise en place des paramètres du callback dans PARAM_CALLBACK_CONNECTION
		Param_callback_connection param_connection_gtk;
		param_connection_gtk.param_thread = param;
		param_connection_gtk.entry_username = entry_identity[0];
		param_connection_gtk.entry_password = entry_identity[1];
		param_connection_gtk.treeview_services = treeview_services;
		param_connection_gtk.main_window = main_window;
		param_connection_gtk.client_descriptor = sd;
		param_connection_gtk.discussion_window = GTKW_DiscW.discussion_window;
		param_connection_gtk.description_label = GTKW_DiscW.description_label;
		g_signal_connect(G_OBJECT(connection_button), "clicked", G_CALLBACK(callback_cliked_button_connection), (gpointer)&param_connection_gtk);

	// Affichage des widgets et boucle principale
	gtk_widget_show_all(main_window);
    gtk_main();
}

/**
	La fonction appellée lorsque l'utilisateur clique sur le bouton SE CONNECTER
	après avoir tapé son nom et choisit son service (vérification à effectuer dans la fonction)
	@param widget Le pointeur sur le widget crée
	@param data Un void* sur une structure de paramètres (PARAM_CALLBACK_CONNECTION (cf gtkgui_main.h)
*/
void callback_cliked_button_connection(GtkWidget* widget, gpointer data)
{
	Param_callback_connection * param = (Param_callback_connection*) data;
	GtkWidget* entry_username = param->entry_username;
	GtkWidget* entry_password = param->entry_password;
	GtkWidget* treeview_services = param->treeview_services;
	GtkWidget* main_window = param->main_window;
	GtkWidget* discussion_window = param->discussion_window;
	GtkWidget* description_label = param->description_label;
	int sd = param->client_descriptor;
	char connection_cmd[MAX_SIZE];

	/*
	##################################
	RECUPERE LES INFORMATIONS DE CONNEXION
	##################################
	*/

	// Get entry text
	const char* client_username = gtk_entry_get_text(GTK_ENTRY(entry_username));
	const char* client_password = gtk_entry_get_text(GTK_ENTRY(entry_password));
	int nb_char_username = gtk_entry_get_text_length(GTK_ENTRY(entry_username));
	int nb_char_password= gtk_entry_get_text_length(GTK_ENTRY(entry_password));

	// Get treeview selection
	GtkTreeSelection *selection;
	GtkTreeModel     *model;
	GtkTreeIter       iter;
	gchar *client_services;
	int is_there_selected_service = 0;

	// Récupère la séléction du treeview
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_services));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get(model, &iter, 0, &client_services, -1);
		is_there_selected_service = 1;
	}
	else
	{
		is_there_selected_service = 0;
	}

	/*
	##################################
	GERE LES ERREURS ET LA SUITE DE L'EXECUTION
	##################################
	*/

	if(nb_char_username== 0 || nb_char_password == 0 || is_there_selected_service == 0)
		{
			// Gestion d'un message d'erreur sophistiqué
			char error[200];
			if(nb_char_username+nb_char_password== 0) strcpy(error,"Merci de renseigner un nom et un prénom d'identification.");
			else if (nb_char_username == 0) strcpy(error,"Merci de renseigner votre prénom.");
			else if (nb_char_password == 0) strcpy(error,"Merci de renseigner votre nom.");
			else strcpy(error,"Merci de séléctionner votre service.");

			// Création du dialog d'erreur (en deux parties 1- TITRE 2- TEXTE)
			GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW (main_window),
												GTK_DIALOG_MODAL,
												GTK_MESSAGE_ERROR,
												GTK_BUTTONS_OK | GTK_BUTTONS_CLOSE, "Erreur 500");
			gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),error);
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
	else
	{
		// On envoie la commande de connexion, on enlève la fenètre de connexion et on affiche celle de discussion
		sprintf(connection_cmd,"/connect %s %s %s",client_username,client_password,client_services);
		send(sd, connection_cmd,strlen(connection_cmd), 0);
		gtk_widget_hide(main_window);
		gtkgui_discussion(discussion_window,description_label,sd,client_username,client_services);
	}
}

/**
	Active la fenêtre de discussion (quand connexion OK)
	@param window Le Widget de la fenêtre de discussion crée au préalable qu'il faut afficher
	@param description_label Le petit label à mettre à jour qui donne des info à l'utilisateur
	@param param Les param du thread
	@param sd Le client descriptor
	@param client_firstname, client_lastname, client_service Des informations sur le client à afficher dans le label
*/
void gtkgui_discussion (GtkWidget* window,GtkWidget* description_label, int sd,const char* client_username,char* client_services)
{
	// Demande la liste des utilisateurs connectés au serveur
	mygtk_set_label_information_connection(GTK_LABEL(description_label),client_username,client_services);
	usleep(10000); // Permet de laisser un temps de repos entre "/connect" et "/users" (sinon, mélange et bug) (invisible pour l'utilisateur)
	send(sd, "/users", strlen("/users"), 0);
	gtk_widget_show_all (window);
}

/**
	Fonction de callback lorsque l'utilisateur ouvre la fenêtre de shared files
	@param gparam La structure callback_discussion avec tous les widgets importants
*/
void callback_clicked_button_sharedfiles (GtkWidget* widget, gpointer gparam)
{
	Param_callback_discussion * param = (Param_callback_discussion*) gparam;
	gtk_widget_show_all(param->GTKW_SharedfW->window);
	send(param->param_thread->client_descriptor, "/files", strlen("/files"), 0);
}

/**
	Fonction de callback lorsque l'utilisateur clique sur le bouton "lire le fichier" après l'avoir séléctionné dans la liste
	@param gparam La structure callback_sharedfiles (armé du descripteur socket)
*/
void callback_clicked_button_readfile(GtkWidget* widget, gpointer gparam)
{
	Param_callback_sharedfiles * param = (Param_callback_sharedfiles*) gparam;
	char tosend[MAX_SIZE];

	// Get treeview selection
	GtkTreeSelection *selection;
	GtkTreeModel  *model;
	GtkTreeIter   iter;
	gchar *shared_file;

	// Récupère la séléction du treeview
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(param->treeview_files));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		// récupère le fichier séléctionné dans le treeview
		gtk_tree_model_get(model, &iter, 0, &shared_file, -1);
		sprintf(tosend,"/get %s",shared_file); // écrit la requête /get <file.ext>
		send(param->sd,tosend,strlen(tosend),0); // envoie sur la socket
	}
}




