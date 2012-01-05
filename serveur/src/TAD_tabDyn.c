#include <stdio.h>
#include <stdlib.h>
#include "TAD_element.h"
#include "TAD_tabDyn.h"

/**
	Supprime le dernier élément d'un TD
	@param t Le TD
*/
void supprimeDernierElement_tab(Tab_Dynamique * t)
{
	t->derniere_position--;
}

/**
	Modifie le i-ème élément d'un TD
	@param t Le TD
	@param e Le nouvel élément
	@param i La position i
*/
void modifierIemeElement_tab(Tab_Dynamique * t,Element e,unsigned int i)
{
	t->ad[i] = e;
}

/**
	Retourne la valeur du i-ième élément
	@param t le Td
	@param i La position du i-ème
	@return La valeur du i-ème élément
*/
Element valeurIemeElement_tab(const Tab_Dynamique * t, unsigned int i)
{
	return t->ad[i];
}

/**
	Affiche le tableau dynamique
	@param t Le TD à afficher
*/
void affiche_tab(const Tab_Dynamique *t)
{
	int i;
	for (i=0; i<t->derniere_position;i++)
	{
	afficheElement(t->ad[i]);
	}
}

/**
	Recherche un élément dans un TD
	@param t Le TD
	@param e L'élément à rechercher
	@return Sa position
*/
int rechercheElement_tab(const Tab_Dynamique *t,Element e)
{
	int i;
	for (i=0; i<t->derniere_position;i++)
	{
		if (t->ad[i] == e)
		return i;
	}

	return -1;
}

/**
	Initialise les valeurs d'un TD en mémoire
	@param tab Le pointeur sur le TD
*/
void initialiser_tab(Tab_Dynamique * tab)
{
	tab->ad = (Element*) malloc(sizeof(Element));
	tab->capacite = 1;
	tab->derniere_position = 0;
}

/**
	Une façon particulière d'initialiser un TD en mémoire (en passant l'adresse de son pointeur)
	@param tab L'adresse du pointeur
*/
void initialiser_tab_pointer(Tab_Dynamique ** tab)
{
	*tab = (Tab_Dynamique*) malloc(sizeof(Tab_Dynamique));
	initialiser_tab(*tab);
}

/**
	Détruit un TD
	@param tab Le tableau à détruire
*/
void testament_tab(Tab_Dynamique * tab)
{
	if(tab->ad != NULL)
	{
		free(tab->ad);
		tab->ad = 0;
		tab->capacite = 0;
	}
}

/**
	Ajoute un élément au TD
	@param tab Le TD
	@param x L'élément à ajouter à la fin
*/
void ajoutElement_tab(Tab_Dynamique * tab,Element x)
{

	int i;
	Element* temp = tab->ad;

	if (tab->capacite == tab->derniere_position)
	{

	tab->ad = (Element*)malloc(sizeof(Element)*(tab->capacite)*2);

		for(i=0;i<tab->capacite;i++)
		{
			 tab->ad[i] = temp[i];
		}

		tab->capacite *= 2;
		free(temp);

	}

	tab->ad[tab->derniere_position] = x;
	tab->derniere_position++;

}

/**
	Copie le contenu d'un TD dans un autre
	@param taba Le tableau à copier
	@param tabb Le tableau qui reçoit la copie
	@note tabb est censé être initialisé mais vide
*/
void copycontent_taba_2_tabb(const Tab_Dynamique * taba,Tab_Dynamique * tabb)
{
	int i;
	for(i=0;i<taba->derniere_position;i++)
	{
		ajoutElement_tab(tabb,taba->ad[i]);
	}
}

/**
	Copie la structure d'un TD dans un autre
	@param taba Le tableau à copier
	@param tabb Le tableau qui reçoit la copie
*/
void copystruct_taba_2_tabb(const Tab_Dynamique * taba,Tab_Dynamique * tabb)
{
		tabb->ad = taba->ad;
		tabb->capacite = taba->capacite;
		tabb->derniere_position = taba->derniere_position;
}

/**
	Retourne le nombre d'éléments d'un TD
	@param tab Le TD
	@return le nb d'éléments
*/
int get_nb_elements(const Tab_Dynamique * tab)
{
	return tab->derniere_position;
}

