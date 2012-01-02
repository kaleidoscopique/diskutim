#include "../inc_alllib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manage_services.h"

/**
	Enregistre la liste des services envoyée par le serveur dans un tableau de manière propre
	data = +Service1+Service2+Service3+ (où + = TRAME_SEPARATOR)
	@param data La chaine de caractère "sale" qui contient les services
	@param listof_services La structure qui va recevoir les services de manière ordonnée
*/
void get_listof_services(char* data, Services* listof_services)
{
	int i;
	char* pch;

	// RaZ du tableau de SERVICES
	for (i=0;i<MAX_SERVICES;i++)
		memset(listof_services->tab[i],0,MAX_SIZE_SERVICE_NAME);
	listof_services->nb_services = 0;

	i=0;
	data[strlen(data)-1] = '\0'; // on enlève le ~ final dans la chaine de caractères qui contient les services
	pch = strtok(data,TRAME_SEPARATOR);
	while (pch != NULL)
	{
		strcpy(listof_services->tab[i],pch);
		pch = strtok(NULL, TRAME_SEPARATOR);
		i++;listof_services->nb_services++;
	}
}

