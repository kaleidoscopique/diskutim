#ifndef _TAD_TabDyn
#define _TAD_TabDyn
#include "TAD_element.h"

struct Tab_Dyn
{
	Element* ad;
	int capacite;
	int derniere_position;
};
typedef struct Tab_Dyn Tab_Dynamique;

void initialiser_tab(Tab_Dynamique *);
void initialiser_tab_pointer(Tab_Dynamique ** tab);
void testament_tab(Tab_Dynamique *);
void ajoutElement_tab(Tab_Dynamique *,Element);
Element valeurIemeElement_tab(const Tab_Dynamique *, unsigned int);
void affiche_tab(const Tab_Dynamique *);
void supprimeDernierElement_tab(Tab_Dynamique *);
void modifierIemeElement_tab(Tab_Dynamique *,Element,unsigned int);
int rechercheElement_tab(const Tab_Dynamique *,Element);
void copy_taba_2_tabb(const Tab_Dynamique * taba,Tab_Dynamique * tabb);
void copystruct_taba_2_tabb(const Tab_Dynamique * taba,Tab_Dynamique * tabb);
int get_nb_elements(const Tab_Dynamique * tab);

#endif
