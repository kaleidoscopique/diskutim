/**
	 Routines d'utilisation des tableaux dynamiques
	 @version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include "TAD_element.h"
#include "TAD_tabDyn.h"


void supprimeDernierElement_tab(Tab_Dynamique * t)
{
	t->derniere_position--;
}

void modifierIemeElement_tab(Tab_Dynamique * t,Element e,unsigned int i)
{
	t->ad[i] = e;
}


Element valeurIemeElement_tab(const Tab_Dynamique * t, unsigned int i)
{
	return t->ad[i];
}

void affiche_tab(const Tab_Dynamique *t)
{
	int i;
	for (i=0; i<t->derniere_position;i++)
	{
	afficheElement(t->ad[i]);
	}
}

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


void initialiser_tab(Tab_Dynamique * tab)
{
	tab->ad = (Element*) malloc(sizeof(Element));
	tab->capacite = 1;
	tab->derniere_position = 0;
}

void initialiser_tab_pointer(Tab_Dynamique ** tab)
{
	*tab = (Tab_Dynamique*) malloc(sizeof(Tab_Dynamique));
	initialiser_tab(*tab);
}

void testament_tab(Tab_Dynamique * tab)
{
	if(tab->ad != NULL)
	{
		free(tab->ad);
		tab->ad = 0;
		tab->capacite = 0;
	}
}

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

void copycontent_taba_2_tabb(const Tab_Dynamique * taba,Tab_Dynamique * tabb)
{
	int i;
	for(i=0;i<taba->derniere_position;i++)
	{
		ajoutElement_tab(tabb,taba->ad[i]);
	}

	printf("%d elements dans taba\n",taba->derniere_position);
	printf("%d elements dans tabb\n",tabb->derniere_position);

}

void copystruct_taba_2_tabb(const Tab_Dynamique * taba,Tab_Dynamique * tabb)
{
		tabb->ad = taba->ad;
		tabb->capacite = taba->capacite;
		tabb->derniere_position = taba->derniere_position;
}

int get_nb_elements(const Tab_Dynamique * tab)
{
	return tab->derniere_position;
}

