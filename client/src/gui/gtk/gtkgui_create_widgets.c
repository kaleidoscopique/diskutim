#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "../../inc_alllib.h"
#include "gtkgui_create_widgets.h"
#include "gtkgui_adapted_functions.h"
#include "gtkgui_main.h"

/**
	Crée un widget treeview à une colone (une liste) scrollable dans une swindow
	@param label Le texte à afficher au dessus de la liste
	@param view Le treeview à créer
	@param swindow Le conteneur (fenêtre scrollable) du treeview (c'est ça qu'on manipule sur la main_window par la suite)
*/
void mygtk_create_treeview_1column(gchar* label,GtkWidget ** view,GtkWidget ** swindow)
{
	GtkCellRenderer* renderer;
	*view = gtk_tree_view_new (); // création du treeview

	renderer = gtk_cell_renderer_text_new (); // création d'une colonne...
	gtk_tree_view_insert_column_with_attributes ( //... & configuration de la colonne
												GTK_TREE_VIEW (*view),
												-1,label,renderer,
												"text",
												0, // ID de la colone rajouté, important pour insérer des données
												NULL);

	// Pour configurer l'objet séléction du treeview (facultatif pour SELECTION SINGLE) : //
	GtkTreeSelection *  treeview_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(*view));
	gtk_tree_selection_set_mode (treeview_selection,GTK_SELECTION_SINGLE);

	*swindow = gtk_scrolled_window_new (NULL, NULL); //init de la scrollbar du treeview
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(*swindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER(*swindow), *view); // attache le treeview a la scrollbar
	//!\ Note : ce sera swindow qu'il faudra attacher la box plus tard, et non pas view
}

/**
	Crée un textview scrollable (un textview basique DANS une swindow scrollable)
	@param swindow Pointeur sur la fenêtre scrollable à créer (une "boite" qui contient le textview)
	@param textview Pointeur sur le textview (sur lequel on agit directement par la suite pour append du text etc)
*/
void mygtk_create_textview_discussion(GtkWidget **swindow,GtkWidget ** textview)
{
	GtkTextBuffer *buffer;
	GtkTextIter iter;

	// Création d'un window scrollable & d'un textview
	*swindow = gtk_scrolled_window_new (NULL, NULL);
	*textview = gtk_text_view_new ();
		gtk_text_view_set_left_margin(GTK_TEXT_VIEW (*textview),5);
		gtk_text_view_set_editable(GTK_TEXT_VIEW (*textview),FALSE); // Textview non editable
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (*textview), GTK_WRAP_WORD); // Retour à la ligne auto si texte trop long
		gtk_container_add (GTK_CONTAINER (*swindow), *textview); // Ajout du textview dans la scrollable window

	// Création d'un mark pour la gravité du scrollage
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (*textview));
	gtk_text_buffer_get_end_iter (buffer, &iter);
	gtk_text_buffer_create_mark (buffer, "scroll", &iter, TRUE);
}

/**
	Ajoute un texte content_to_insert à un textview scrollable
	@param textview Le textview considéré auquel on ajoute du texte
	@param content_to_insert Le contenu à insérer
*/
void mygtk_append_text_to_textview (GtkTextView *textview, gchar * content_to_insert)
{
	GtkTextBuffer *buffer; // Recevra le buffer du textview
	GtkTextIter iter; // Iterateur de déplacement (curseur)
	GtkTextMark *mark; // Gestion de la gravité (scroll)

	// Récupère le buffer
	buffer = gtk_text_view_get_buffer (textview);
	gtk_text_buffer_get_end_iter (buffer, &iter);

	// Insère content_to_insert
	gtk_text_buffer_insert (buffer, &iter, content_to_insert, -1);

	// Gère le scroll et la gravité /w mark
	gtk_text_iter_set_line_offset (&iter, 0);
	mark = gtk_text_buffer_get_mark (buffer, "scroll");
	gtk_text_buffer_move_mark (buffer, mark, &iter);

	// Scroll vers la marks
	gtk_text_view_scroll_mark_onscreen (textview, mark);
}

void mygtk_clear_textview (GtkTextView *textview)
{
	GtkTextBuffer *buffer; // Recevra le buffer du textview
	GtkTextIter iterstart; // Iterateur de déplacement (curseur)
	GtkTextIter iterend; // Iterateur de déplacement (curseur)

	// Récupère le buffer
	buffer = gtk_text_view_get_buffer (textview);
	gtk_text_buffer_get_start_iter (buffer, &iterstart); // récupère la position du début
	gtk_text_buffer_get_end_iter (buffer, &iterend); // et celle de fin
	gtk_text_buffer_delete(buffer,&iterend,&iterstart); // supprime ce qu'il y a entre les deux positions
}

/**
	Modifie le label \label et lui ajouter un texte type "Vous êtes connecté entant que .... (<service>)
	@param Label [in,out] Le label à modifier
	@param client_firstsname Le prénom du client à afficher dans le label
	@param client_lastname Le nom du client ---
	@param client_service Le service du client ---
	@note Si les 3 const char sont NULL, alors on affiche une DECONNEXION ! ("Vous êtes déconnecté")
*/
void mygtk_set_label_information_connection(GtkLabel * label,
																					const char* client_username,
																					const char* client_service)
{
	char char_description_label[SHEET_MAX_SIZE*2+20];
	if(client_username != NULL && client_service != NULL)
	{
		strcpy(char_description_label,"Vous êtes connecté entant que ");
		strcat(char_description_label,client_username);
		strcat(char_description_label," ");
		strcat(char_description_label," (");
		strcat(char_description_label,client_service);
		strcat(char_description_label,")");
	}
	else
	{
		strcpy(char_description_label,"Vous êtes déconnecté du serveur.");
	}

	gtk_label_set_text(label,char_description_label);
}


/**
	Créer une widget window préconfiguré pour être centrée
	@param sizex La largeur de la fenêtre
	@param sizey La hauteur de la fenêtre
	@return Le widget de la fenètre
*/
GtkWidget* mygtk_create_widget_window(int sizex,int sizey,int FLAG)
{
	GtkWidget * main_window = gtk_window_new(FLAG);
		gtk_window_set_title(GTK_WINDOW(main_window),TITLE_MAINWINDOW);
		gtk_window_set_default_size(GTK_WINDOW(main_window),sizex,sizey);
		gtk_window_set_position(GTK_WINDOW(main_window),GTK_WIN_POS_CENTER);
		g_signal_connect(G_OBJECT(main_window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
		return main_window;
}


/**
	Crée un entrytext de taille \maxsize
	@param maxsize La taille maximale du texte que l'on peut entrer
	@return Le widget de l'entrytext
*/
GtkWidget* mygtk_new_entrytext(int maxsize)
{
		GtkWidget * entrytext = gtk_entry_new();
		gtk_entry_set_max_length (GTK_ENTRY(entrytext),maxsize);
		gtk_entry_set_activates_default(GTK_ENTRY(entrytext),TRUE); 	// Active le widget DEFAULT en appuyant sur entrée
																															// (ce widget est généralement le button "valider")
		return entrytext;
}


// ==============================================================
// ==============================================================
//													CREATION DE FENETRE ENTIERE
// ==============================================================
// ==============================================================

/**
	Fonction appellé lors d'un click sur le bouton "envoyer message" dans la fenêtre de discussion
	@param widget Le pointeur du bouton clické
	@param data Un void* des param envoyés (structure Param_callback_discussion*)
*/
void callback_clicked_button_sendmsg(GtkWidget* widget, gpointer data)
{
	char buffer[MAX_SIZE]; // Recevra la chaine à afficher dans le textview en local (son propre message)

	// On récupère les paramètres du callback dans la structure adéquate
	Param_callback_discussion* param = (Param_callback_discussion*) data;
		int sd = param->param_thread->client_descriptor;
		GtkWidget* entrytxt = param->entry_msg; // text entry msg
		GtkWidget* textview = param->textview_discussion; // textview discussion
		GtkWidget* treeview = param->treeview_users_list; // pour vider la liste si on tape /quit
		GtkWidget* description_label = param->description_label; // pour afficher "deconnecté"

	// On récupère le message dans le champs de texte utilisateur et on l'envoie
	const char* message = gtk_entry_get_text(GTK_ENTRY(entrytxt));

	// On ne fait rien si le message est vide
	if(!strcmp(message,"")) return;
	else send(sd,message,strlen(message),0);

	if(message[0] == '/') // Si le message tapé est une commande, on change un peu l'affichage en local
	{
		if(!strncmp(message,"/quit",strlen("/quit")))
		{
			sprintf(buffer,"*** DECONNEXION ***\n"); // gestion de la commande QUIT
			mygtk_set_label_information_connection(GTK_LABEL(description_label),NULL,NULL); // Change le label "vous êtes déconnecté"
			gtkgui_show_users_list(NULL,(void*)treeview); // Vide la userslist
		}
		else
		{
			sprintf(buffer,"** %s ** \n",message); // gestion des autres commandes
		}
	}
	else // Si c'est un message, on affiche <Vous> [message]
	{
		sprintf(buffer,"<Vous> %s\n",message);
	}

	// Mise à jour du textview avec le buffer & vide le textentry
	mygtk_append_text_to_textview(GTK_TEXT_VIEW(textview),(gchar*) buffer);
	gtk_entry_set_text(GTK_ENTRY(entrytxt),"");
}

void mygtk_close_window(GtkWidget* widget, gpointer p)
{
	gtk_widget_hide((GtkWidget*)p);
}
