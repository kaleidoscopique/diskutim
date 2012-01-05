#include "../inc_define.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
	Enregistre la saisie sur stdin en évitant un buffer overflow et en prenant les espaces
	@param request La chaine de caractère qui reçoit la saisie
	@param size_of La taille de cette chaine
	@return Le code erreur (1=ok,0=fail)
*/

int lecture_stdin(char * request, int size_of)
{
		char* p = NULL;

		if(fgets(request, size_of, stdin))
		{
			// on cherche le saut de ligne ajouté par fgets
			p = strchr(request,'\n');

			// On le remplace par \0
			if (p != NULL) *p = '\0';
			return 1;
		}

		return 0;
}

/**
	Indique si les "tokenSize" prochains caractères sont le token
	@param src La chaine de caractère source
	@param token Le séparateur token
	@param position L'indice de la position à considérer comme début potentiel du token
	@param tokenSize Le nombre de caractère que mesure le token
	@return 1 si oui, 0 si non
*/
static int isitthetoken(char* src,char* token, int position,int tokenSize)
{
	if(strncmp(token,src+position,tokenSize) == 0) return 1;
	else return 0;
}

/**
	Cette fonction s'utilise pour traiter les DATA du serveur lorsqu'il envoie un message
	de la forme "TOKENauteurTOKENmsgTOKENserviceTOKENinfo"
	@param data Les données envoyées par le serveur avant traitement
	@param [out] author Reçoit le nom de l'auteur du message
	@param [out] msg Reçoit le message envoyé
	@param [out] service Reçoit le service de la personne qui envoie le message
	@param [out] others Reçoit des informations supplémentaires liées au message
*/
void complete_data_msg(char*data, char* author,char* msg,char* service, char* others)
{
	char packet[4][MAX_SIZE]; // contiendra author, msg, service & others
	char token[TOKEN_SIZE]; // contiendra le séparateur (XXXXXXX) de data
	int j,k,i;

	// RaZ du PACKET qui reste en mémoire /!\ important & vérifié
	memset(packet[0],0,MAX_SIZE);
	memset(packet[1],0,MAX_SIZE);
	memset(packet[2],0,MAX_SIZE);
	memset(packet[3],0,MAX_SIZE);
	memset(token,0,TOKEN_SIZE);

	// Récupère le token au début de DATA
	for (j=0;j<TOKEN_SIZE;j++)
	{
		token[j] = data[j];
	}
	//printf("Le token : %s\n",token);

	// On va remplit les 4 paquets (auteur, message, service, others)
	for(k=0;k<4;k++)
	{
		i=0;
		// tant qu'on ne tombe ni sur le token ni la fin du message
		while(!isitthetoken(data,token,j+i,TOKEN_SIZE) && data[i+j] != '\0')
		{
			packet[k][i]=data[j+i]; // On remplit le packet "k"
			i++; // On avance dans les rawdata
		}
		j = j+i+TOKEN_SIZE; // on va sur la prochaine section de texte [author->msg->services(->others)]
	}

	// On copie nos trouvailles dans les paramètres [OUT]
	strcpy(author,packet[0]);
	strcpy(msg,packet[1]);
	strcpy(service,packet[2]);
	strcpy(others,packet[2]);
}

/**
	Retourne 1 si DATA est un message utilisateur client2client
	@param data_type La chaine de caractère à tester
	@return 1 ou 0
*/
int is_a_usrmsg(int data_type)
{
	if(data_type == 3) return 1;
	else return 0;
}

/**
	Retourne 1 si DATA est la liste des services envoyée par le serveur
	@param data_type La chaine de caractère à tester
	@return 1 ou 0
*/
int is_listof_services(int data_type)
{
	if(data_type == 1) return 1;
	else return 0;
}

/**
	Retourne 1 si DATA est la liste des users connectés au même salon que le client
	(liste envoyée par le serveur après /users)
	@param data_type La chaine de caractère à tester
	@return 1 ou 0
*/
int is_listof_users(int data_type)
{
	if(data_type == 2) return 1;
	else return 0;
}

/**
	Retourne 1 si DATA est un message serveur type "@500"
	@param data_type La chaine de caractère à tester
	@return 1 ou 0
*/
int is_a_sysmsg(int data_type)
{
	if(data_type == 4) return 1;
	else return 0;
}

/**
	Retourne 1 CODE est le code erreur contenu dans DATA
	@param data La chaine de caractère à tester
	@return Le code erreur du serveur en INT
*/
int sysmsg2int(char* data)
{
	// On transforme "+XXX (-> char data)" en "XXX (-> char chCode)"
	char chCode[4];
	chCode[0] = data[1];
	chCode[1] = data[2];
	chCode[2] = data[3];
	chCode[3] = '\0';
	return atoi(chCode);
}

/**
	Retourne un entier relatif au contenu du buffer envoyé par le serveur
	@param buffer Les rawdata à traiter
	@return 1 si les data sont une liste de services
					2 si les data sont une liste d'users
					3 si les data sont un message
					4 si les data sont un code erreur/info
					5 si les data sont la liste des fichiers shared
					6 si les data sont un fichier shared complet
*/
int get_type_buffer(char* buffer)
{
		char newbuffer[MAX_SIZE];

		// On compare les n premiers caractères du buffer avec une des commandes possibles
		// str'n'cmp ou n est le nb de char dans l'intitulé commande
		if(!strncmp(buffer,"/SERVICES",strlen("/SERVICES")))
		{
			// On copie le contenu du buffer, sauf le début qui contient l'intitulé de la commande
			// On le met dans un tampon pour ensuite MAJ le vrai buffer (param IN,OUT)
			strcpy(newbuffer,buffer+strlen("/SERVICES"));
			strcpy(buffer,newbuffer);
			return 1; // On retourne l'équivalence détéctée en INT
		}
		// Pour les cas prochains, se référer au premier cas
		else if(!strncmp(buffer,"/USERS",strlen("/USERS")))
		{
				strcpy(newbuffer,buffer+strlen("/USERS"));
				strcpy(buffer,newbuffer);
				return 2;
		}
		else if(!strncmp(buffer,"/SYS",strlen("/SYS")))
		{
				strcpy(newbuffer,buffer+strlen("/SYS"));
				strcpy(buffer,newbuffer);
				return 4;
		}
		else if(!strncmp(buffer,"/MSG",strlen("/SYS")))
		{
			strcpy(newbuffer,buffer+strlen("/SYS"));
			strcpy(buffer,newbuffer);
			return 3;
		}
		else if(!strncmp(buffer,"/FILES",strlen("/FILES")))
		{
			strcpy(newbuffer,buffer+strlen("/FILES"));
			strcpy(buffer,newbuffer);
			return 5;
		}
		else if(!strncmp(buffer,"/GET",strlen("/GET")))
		{
			strcpy(newbuffer,buffer+strlen("/GET"));
			strcpy(buffer,newbuffer);
			return 6;
		}
		else
		{
			return 0;
		}
}

/**
	Evalue le code erreur code et copie le message approprié dans buffer
	@param buffer Le retour de la fonction
	@param code Le code erreur reçu
	@param msg_content Le message brute (/SYS+XXX+........) sans modification; utile quelque fois (ex : code 700 ou retour /adduser)
*/
void codeErr2char(char *buffer, int code,const char * msg_content)
{
	switch(code)
	{
		// -- Code de permission
		case 200:strcpy(buffer,"Vous n'êtes pas autorisé à faire cela !\n"); break;

		// -- Code de connexion
		case 400:strcpy(buffer,"Vous êtes désormais connecté à votre service ! :-)\n"); break;
		case 300:strcpy(buffer,"ERREUR - Vous devez renseigner des informations de connexion !\n"); break;
		case 302:strcpy(buffer,"ERREUR - Vous n'avez indiqué aucun mot de passe !\n"); break;
		case 303:strcpy(buffer,"ERREUR - Vous devez choisir un service !\n"); break;
		case 304:strcpy(buffer,"ERREUR - Le service que vous avez indiqué n'existe pas !\n"); break;
		case 305:strcpy(buffer,"ERREUR - Votre mot de passe est incorrect !\n"); break;
		case 306:strcpy(buffer,"ERREUR - Votre login n'existe pas dans la base de donnée !\n"); break;

		// -- Code d'utilisation
		case 501:strcpy(buffer,"Vous devez être connecté pour faire cela.\n"); break;
		case 502:strcpy(buffer,"Cette commande n'est pas reconnue par le serveur.\n"); break;

		// -- Code de fichiers
		case 600:strcpy(buffer,"Le fichier que vous demandez n'existe pas.\n"); break;

		// -- Code de la gestion des utilisateurs
		case 800:
			strcpy(buffer,"[IMPORTANT] Mot de passe généré pour l'utilisateur : ");
			strcat(buffer,msg_content+5);
			strcat(buffer,"\n");
		break;

		case 801:
			strcpy(buffer,"Une erreur est survenue lors de l'ajout !\n");
		break;

		// -- Code de doc
		case 700:strcpy(buffer,msg_content+5);break; // quand on reçoit la doc qui commence par +700+............. (on vire juste "+700+")

		// -- Default
		default:strcpy(buffer,"Code erreur inconnu, veuillez contactez un administrateur (ou RTFM !).\n"); break;
	}
}


