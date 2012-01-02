#ifndef __CREATE_WIDGET
#define __CREATE_WIDGET
#include "../../inc_define.h"
#include "../../inc_alllib.h"
#include "../../network/manage_threads.h"
#include "gtkgui_main.h"

//===========================
// Création de widgets & routines
//===========================

// Créer un treeview scrollable
void mygtk_create_treeview_1column(gchar* label,GtkWidget ** view,GtkWidget ** swindow);

// Créer un textview scrollable
void mygtk_create_textview_discussion(GtkWidget **swindow,GtkWidget ** textview);
void mygtk_append_text_to_textview (GtkTextView *textview, gchar * content_to_insert);
void mygtk_clear_textview (GtkTextView *textview);

// Configurer un label d'information (fonction très rigide et utile une seule fois)
void  mygtk_set_label_information_connection(GtkLabel * label,
																										const char* client_username,
																										const char* client_services);

// Créer une fenètre de taille sizex*sizey centrée sur l'écran
GtkWidget* mygtk_create_widget_window(int sizex,int sizey,int FLAG);

// Créer un entrytext dont le texte ne passé pas maxsize caractère
GtkWidget* mygtk_new_entrytext(int maxsize);

// Hide une GTK window
void mygtk_close_window(GtkWidget* window, gpointer p);

//===========================
// Création de fenêtres entières
//===========================

// Créer la fenêtre de discussion à priori
void callback_clicked_button_sendmsg(GtkWidget* widget, gpointer data);

#endif
