#include "inc_define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manage_threads.h"
#include "manage_services.h"

/**
	Lit le fichier data/services.conf et remplit la structure Services avec le nom des services
	@param listof La structure de stockage des services (globale et unique)
*/
void complete_services_tab(TabServices * listof)
{
	int i = 0 ;
	char *p;
	FILE* file;

	if ((file = fopen("data/services.conf","r")) == NULL) perror("Le fichier services.conf n'existe pas : ");

	while (fgets(listof->tab[i].name, MAX_SIZE_SERVICE_NAME, file) != NULL)
	{
			// Supprime le \n à la fin de chaque ligne du fichier
			p = strchr(listof->tab[i].name,'\n');
			if (p != NULL) *p = '\0';

			// On initialise le tableau dynamique qui contient
			// les descripteurs de clients connectés au service (vide pour le moment bien sûr)
			initialiser_tab_pointer(&listof->tab[i].listof_client_descriptors);

		i++;
	}
	// Récupère le nombre de services ajoutés
	listof->nb_services = i;
	fclose(file);
}

/**
	Retourne le nom d'un service en fonction de son numéro d'ID
	@param listof La structure de stockage des services (globale et unique)
	@param number Le numéro d'un service
	@return Le nom d'un service
*/
const char* get_services_from_number(const TabServices * listof, int number)
{
	return listof->tab[number].name;
}

/**
	Retourne le numéro d'un service en fonction de son nom
	@param listof La structure de stockage des services (globale et unique)
	@param service Le nom du service
	@return Le numéro d'un service (-1 s'il n'existe pas)
*/
int get_num_from_services(const TabServices * listof, char* service)
{
	int i;
	for(i=0;i<listof->nb_services;i++)
	{
		if(strcasecmp(listof->tab[i].name,service) == 0) return i;
	}
	return -1;
}

/**
	Retourne le nombre d'utilisateurs connectés au service n°num_of_service
	@param listof La liste de tous les services
	@param num_of_service Le numéro du service dont on veut connaître le nb d'users
	@return Le nb d'users connectés au service num_of_service
*/
int get_numberof_user(const TabServices * listof, int num_of_service)
{
	return get_nb_elements(listof->tab[num_of_service].listof_client_descriptors);
}

/**
	Ajoute un SD à la liste des clients connectés à un service
	@param listof La liste de tous les services (+ leurs utilisateurs connectés)
	@param num_of_service Le service auquel on veut assigner un nouvel utilisateur online
	@param sd Le SD du client à rajouter dans le tableau des users connectés au service
*/
void add_SD_online_user(TabServices* listof, int num_of_service, int sd)
{
	ajoutElement_tab(listof->tab[num_of_service].listof_client_descriptors,sd);
}

/**
	Retourne le SD du i-ème utilisateur connecté à un service
	@param listof La liste de tous les services
	@param num_of_service Le numéro du service que l'on considère
	@param i Le numéro de l'utilisateur
	@return Le SD de l'utilisateur
*/
int get_ith_sd(const TabServices* listof, int num_of_service, int i)
{
	return valeurIemeElement_tab(listof->tab[num_of_service].listof_client_descriptors,i);
}

/**
	Extrait la liste des services de PARAM_THREAD
	@param param Le param thread classique
	@return La liste des services globale
*/
TabServices* get_listof_services(const Param_thread * param)
{
	return param->services_list;
}

/**
	Retourne le service numéro 'n' de la liste des services
	@param param le PARAM_THREAD classique
	@param n Le numéro du n-ième service que l'on veut récupérer
	@return La structure Service du bon services
*/
Service* get_the_nth_service(const Param_thread * param, int n)
{
	return &(param->services_list->tab[n]);
}

/**
	Plus précis que get_the_nth_service, cette fonction retourne
	directement le tableau dynamique contenant les SD des connecté à un service
	@param listof La liste des services
	@param n Le numéro du service qui nous intérèsse
	@return Le tab dyn des SD des clients connectés du service numéro 'n'
*/
Tab_Dynamique* get_the_nth_service_tab(const TabServices * listof, int n)
{
	return listof->tab[n].listof_client_descriptors;
}
