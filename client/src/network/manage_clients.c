#include "../inc_alllib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manage_clients.h"

/**
	Enregistre la liste des clients connectés envoyée par le serveur dans un tableau de manière propre
	data = +Client1+Client2+Client3...
	@param data La chaine de caractère "sale" qui contient les clients connectés
	@param listof_online_clients La structure qui va recevoir les clients de manière ordonnée
*/
void get_listof_online_clients(char* data, Clients* listof_online_clients)
{
	int i;
	char* pch;

	// RaZ du tableau de SERVICES
	for (i=0;i<MAX_CLIENTS;i++)
		memset(listof_online_clients->tab[i],0,SHEET_MAX_SIZE*2);
	listof_online_clients->nb_online_clients = 0;

	i=0;
	data[strlen(data)-1] = '\0'; // on enlève le ~ final dans la chaine de caractères qui contient les users
	pch = strtok(data,TRAME_SEPARATOR);
	while (pch != NULL)
	{
		strcpy(listof_online_clients->tab[i],pch);
		pch = strtok(NULL, TRAME_SEPARATOR);
		i++;listof_online_clients->nb_online_clients++;
	}
}
