#include "inc_define.h"
#include <string.h>
#include <stdio.h>
#include "manage_clients.h"
#include "manage_threads.h"
#include "TAD_tabDyn.h"

/*
============================
			LES ACCESSEURS
============================
*/

/**
	Retourne le pointeur sur l'index des lignes de la DB
	@param param Les param du thread
	@return Le pointeur sur l'index
*/
int* get_index_db_users(const Param_thread* param)
{
	return param->index_db_users;
}

/** Retourne le prénom d'un client
	@param sheet Sa fiche client personnelle
	@return Son prénom */
const char* get_firstname(const info_client * sheet)
{
	return sheet->firstname;
}

/** Retourne le nom d'un client
	@param sheet Sa fiche client personnelle
	@return Son nom */
const char* get_lastname(const info_client * sheet)
{
	return sheet->lastname;
}

/** Retourne le service d'un client
	@param sheet Sa fiche client personnelle
	@return Son service */
int get_service_int(const info_client * sheet)
{
	return sheet->service;
}

/** Retourne le service d'un client
	@param listof La liste des services
	@param sheet Sa fiche client personnelle
	@return Son service */
const char* get_service_char(const TabServices * listof, const info_client * sheet)
{
	return get_services_from_number(listof,sheet->service);
}

/** Retourne les info complémentaires d'un client
	@param sheet Sa fiche client personnelle
	@return Ses infos OTHERS */
const char* get_others(const info_client * sheet)
{
	return sheet->others;
}

/** Retourne le descripteur du client
	@param param Le param envoyé au thread pour chaque client
	@return L'ID du socket du client concerné par le thread */
int get_descriptor(const Param_thread * param)
{
	return param->client_descriptor;
}

/** Determine si un utilisateur est connecté
	@param sheet fiche de l'utilisateur
	@return 1 si oui, 0 si non
*/
int get_connected(const info_client * sheet)
{
	return sheet->connected;
}

/** Retourne la liste de toutes les fiches clients
	@param param Le param envoyé au thread pour chaque client
	@return L'adresse de la liste d'info de tous les clients (globale, unique) */
info_client* get_info_clients_list(const Param_thread * param)
{
	return param->info_clients_list;
}

/** Retourne la liste de toutes les fiches clients
	@param param Le param envoyé au thread pour chaque client
	@return L'adresse de la liste d'info de tous les clients (globale, unique) */
Tab_Dynamique* get_descriptors_clients_list(const Param_thread * param)
{
	return param->clients_list;
}

/** Retourne le descripteur client à la position n
	@param listof La liste des descripteurs clients actuels
	@param n La position du descripteur à renvoyer
	@return Le descripteur socket en position n (entier positif ou négatif selon CONNECTED ou NON) */
int getn_client_descriptor_value(const Tab_Dynamique* listof, int n)
{
	return listof->ad[n];
}

/** Accès à une fiche client via le descripteur client SD
	@param listof La liste des fiches de tous les clients connectés
	@param sd Le descripteur socket du client dont on veut la fiche
	@return L'adresse de sa fiche */
info_client* get_sheet(const info_client* listof, int sd)
{
	return (info_client*)(listof+sd);
}


/*
============================
			LES MUTATEURS
============================
*/

/** Modifie le prénom d'un client
	@param sheet Sa fiche client personnelle
	@param firstname Le prénom à MAJ  */
void set_firstname(info_client * sheet,char *firstname)
{
	strcpy(sheet->firstname,firstname);
}

/** Modifie le nom d'un client
	@param sheet Sa fiche client personnelle
	@param lastname Le nom à MAJ  */
void set_lastname(info_client * sheet,char *lastname)
{
	strcpy(sheet->lastname,lastname);
}

/** Modifie le service d'un client
	@param sheet Sa fiche client personnelle
	@param service Le service à MAJ  */
void set_service(info_client * sheet,int service)
{
	sheet->service = service;
}

/** Modifie le OTHERS d'un client
	@param sheet Sa fiche client personnelle
	@param others Les info à MAJ */
void set_others(info_client * sheet,char *others)
{
	strcpy(sheet->others,others);
}


/** Ajoute un DESCRIPTOR CLIENT dans la liste des descriptors
	@param listof La liste des descriptors clients
	@param sd Le descriptor à ajouter  */
void add_new_descriptor(Tab_Dynamique* listof, int sd)
{
	ajoutElement_tab(listof,sd);
}

/** Supprime un DESCRIPTOR CLIENT dans la liste des descriptors (en réalité, le passe en négatif)
	@param listof La liste des descriptors clients
	@param sd Le descriptor à droper  */
void remove_descriptor_form_list(Tab_Dynamique* listof, int sd)
{
	int position_client_list = rechercheElement_tab(listof, sd);
	modifierIemeElement_tab(listof,0-sd,position_client_list);
}

/** Affiche la liste des DESCRIPTORS clients (tous)
	@param listof La liste des descriptors clients */
void show_descriptors_list(const Tab_Dynamique* listof)
{
	affiche_tab(listof);
	printf("\n");
}
