#include "inc_define.h"
#include "inc_alllib.h"
#include "network/manage_threads.h"
#include "network/manage_sockets.h"
#include "network/manage_protocole.h"
#include "network/manage_services.h"

#ifdef NOGUI
	#include "gui/nogui/nogui_main.h"
	#include "gui/nogui/nogui_adapted_functions.h"
#endif

#ifdef GTKGUI
	#include "gui/gtk/gtkgui_main.h"
	#include "gui/gtk/gtkgui_adapted_functions.h"
#endif

int main (int argc, char *argv[])
{

	/* ##########
		INITIALISATION
	    ########## */

	int sd; struct sockaddr_in addr;
	char buffer[MAX_SIZE];
	memset(buffer,0,MAX_SIZE);

	/* #############
		THREAD & RESEAU
	    ############# */

	Param_thread * param; // Contient les paramètres à passer au thread
	init_param_thread(&param);
	create_socketC2S(&sd,&addr); // Création de la socket principale

		/* ###########
		   CHOIX DU MAIN
	        ########### */

	#ifndef GTKGUI
	nogui_main(param,sd);
	#else
	gtkgui_main(param,sd);
	#endif

    return 0;
}
