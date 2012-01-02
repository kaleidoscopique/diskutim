#include "../../inc_define.h"
#include "../../inc_alllib.h"
#include "../../network/manage_threads.h"
#include "../../network/manage_sockets.h"
#include "../../network/manage_protocole.h"
#include "../../network/manage_services.h"
	#include "nogui_main.h"
	#include "nogui_adapted_functions.h"

/**
	Boucle principale pour gérer l'affichage du client /w NO GUI
	@param param Les paramètres à passer au thread
	@param sd Le descripteur client
*/
void nogui_main (Param_thread * param,int sd)
{
	bind_param_thread(&param,sd,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	init_functions_thread_for_nogui(&param);
	pthread_t pth = new_thread(check_server_reply,(void*)param);

	char request[MAX_SIZE];

	printf("=======================================\n");
	printf("BIENVENUE INCONNU ! \n");
	printf("Pour vous connecter à votre service, tapez la commande suivante :\n");
	printf("/connect <prenom> <nom> <service>\n");
	printf("N'hésitez pas à demander de l'aide en tapant /help !\n");
	printf("=======================================\n\n");
	printf("> ");

	while(strcmp(request,"/quit") != 0)
	{
		// Prompt de la requete demandée
		memset(request,0,MAX_SIZE);

		if(lecture_stdin(request,sizeof(request)) == 1);
		else printf("Erreur de saisie.");

		// Envoi de la commande au serveur
		send(sd, request, strlen(request), 0);
	}

	close(sd); // Clot la connexion);
	free(param); // Libère la mémoire
	pthread_cancel(pth); // Annule le thread
	pthread_exit(NULL);
}
