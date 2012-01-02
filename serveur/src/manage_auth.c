#include "inc_alllib.h"
#include "inc_define.h"
#include "manage_auth.h"
#include <math.h>
#include <ctype.h>
#include <gcrypt.h>

/**
	Fonction de hachage pour l'indexation des entrées dans la DB ./data/users
	Si cette fonction est modifiée, penser à refaire tout le fichier users
	@param username La valeur dont on veut la clef de hachage
*/
int hash_index_users_bd(char* username)
{
	int res=1; int i=0;
	for(i=0;i<strlen(username);i++)
	{
		res *= (tolower(username[i]) ^ 0);
	}
	res %= MAX_CLIENTS;
	return abs(res);
}

/**
	Retourne l'empreinte md5 d'un buffer
	@param [in] buffer Le buffer à hasher
	@parma [out] md5sum Le résultat du hash
*/
void get_md5sum(const char* buffer,char* md5sum)
{
	char md5_binary[MAX_SIZE];
	gcry_md_hash_buffer(GCRY_MD_MD5, md5_binary, buffer, strlen(buffer));
	sprintf(md5sum,"%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
																			md5_binary[0],
																			md5_binary[1],
																			md5_binary[2],
																			md5_binary[3],
																			md5_binary[4],
																			md5_binary[5],
																			md5_binary[6],
																			md5_binary[7],
																			md5_binary[8],
																			md5_binary[9],
																			md5_binary[10],
																			md5_binary[11],
																			md5_binary[12],
																			md5_binary[13],
																			md5_binary[14],
																			md5_binary[15]);
}


/**
	Permet une indexation des lignes du fichier ./data/users permettant par la suite un accès SÉQUENTIEL.
	@param [in,out] index Le tableau d'index index[num_de_la_ligne(=ID de l'utlisateur)] = octet à lire
*/
int parse_users_db(int* index)
{
	int num_line=0;
	FILE * users_bd = fopen ("./data/users", "r");
    if(users_bd != NULL)
    {
        while(1)
        {
            index[num_line] = ftell (users_bd);
            while (fgetc(users_bd) != '\n' && !feof(users_bd));
            if (feof(users_bd)) break;
            num_line++;
        }
    }
    else
    {
        printf("Erreur lors de l'ouverture de la base de données USERS\n");
    }
    return num_line+1;
    fclose(users_bd);
}

/**
	Récupère le tuple numéro \numindex dans la DB.
	@param num_index Le numếro du tuple à récupérer
	@param index_db_users La table d'index "num tuple => num octet"
	@param [out] surname, name, password, info Reçoivent les données trouvées
					Peuvent être mis à NULL pour simplement avoir le retour de la fonction (sans remplir quoi que ce soit)
	@return 0 si aucun tuple n'a été trouvé à l'indice \numindex
					1 si un tuple a été trouvé
*/
int get_entry_db(int num_index,
									const int* index_db_users,
									char* surname,
									char* name,
									char* password,
									char* info)
{
	FILE* file = fopen("./data/users","r");
	if (file ==  NULL) printf("[ALERTE] Impossible d'ouvrir la DB users !\n");

	// Deux info sheet (prénom + nom) + le nom du service + 32 caractères du hash md5 + les 3-4 espaces séparateurs
	char rawentry[SHEET_MAX_SIZE*2+MAX_SIZE_SERVICE_NAME+32+4];
	char thename[SHEET_MAX_SIZE];
	char* pch;
	char* p;

	// On lit le fichier à la bonne ligne en se servant de l'index et du résultat de la fonction de hash sur USERNAME
	fseek (file, index_db_users[num_index], SEEK_SET);
	fgets(rawentry,SHEET_MAX_SIZE*2+MAX_SIZE_SERVICE_NAME+32+4, file);
	fclose(file);

	if(strlen(rawentry) > 2) // > 2 car une ligne vide contient au moins "\n"
	{
		if(surname != NULL && name != NULL && info != NULL && password != NULL)
		{
			pch = strtok (rawentry," "); // Strtok l'ID
			pch = strtok (NULL, " "); strcpy(surname,pch); // Strtok & save le surname
			pch = strtok (NULL, " "); strcpy(thename,pch); // Strtok & save le name
			pch = strtok (NULL, " "); strcpy(password,pch); // Strtok & save le password
			pch = strtok (NULL, " "); strcpy(info,pch); // Strtok & save les info
														p = strchr(info,'\n'); if (p != NULL) *p = '\0'; // on enlève le saut de fin de ligne

			printf("Entrée %d\n",num_index);
			printf("	Prenom : %s\n",surname);
			printf("	Nom : %s\n",thename);
			printf("	Hash : %s\n",password);
			printf("	Info : %s\n",info);
			strcpy(name,thename);
			return 1;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

/**
	Ajoute un utilisateur dans la DB users
	@param buffer La commande /adduser .... envoyée par le client
	@param [out] password Le mot de passe généré lors de l'ajout, à communiquer à l'utilisateur
	@return 0 si l'utilisateur est ajouté, 1 s'il n'y a plus de place dans la DB
	@note /adduser nom prenom rang
*/
int add_user(char* buffer,int* index_db_users,char* password)
{
	// Ouverture de la DB
	FILE* file = fopen("./data/users","r+");

	// Variables pour parser la commande /adduser
	char firstname[SHEET_MAX_SIZE];
	char lastname[SHEET_MAX_SIZE];
	char username[SHEET_MAX_SIZE];
	char password_md5[40];
	char status[3];

	// Pour stocker le fichier dans un tableau de char [nb de clients][taille max d'un tuple ID + PRÉNOM + NOM + MD5 + STATUT]
	char db2char[MAX_CLIENTS][SHEET_MAX_SIZE*2+MAX_SIZE_SERVICE_NAME+32+4];
	char getfile[SHEET_MAX_SIZE*2+MAX_SIZE_SERVICE_NAME+32+4];
	int i=0;
	int hash_value,starting_value; // starting_value : pour éviter de tourner en rond dans les collisions

	// Parse la commande
	sscanf(buffer,"/adduser %s %s %s",firstname,lastname,status); // récupère les paramètres de la commande
	sprintf(username,"%s%c",lastname,firstname[0]); // formate l'username NOM+X
	printf("USERNAME = %s\n",username);
	hash_value = hash_index_users_bd(username);
	printf("HASH VALUE = %d\n",hash_value);

	// Génération du mot de passe aléatoire de 10 caractères
	// ASCII : 48 -> 122
	for(i=0;i<10;i++)
	{
		password[i] = 48+rand()%74;
	}
	password[10] = '\0';

	printf("Mot de passe généré : %s\n",password);
	get_md5sum(password,password_md5);
	printf("Mot de passe md5 : %s\n",password_md5);

	// On récupère chaque ligne du fichier dans le tableau db2char
	i=0;
	while(fgets(getfile, SHEET_MAX_SIZE*2+MAX_SIZE_SERVICE_NAME+32+4, file) != NULL)
	{
		strcpy(db2char[i],getfile);
		i++;
	}

	// Réouverture de la DB dans un mode écrasement
	fclose(file); file = fopen("./data/users","w+");

	// On cherche une ligne vide (hashtable/collision) pour ajouter notre tuple
	starting_value = hash_value;
	while(strlen(db2char[hash_value-1]) > 4)
	{
		hash_value++;
		if(hash_value == starting_value) return 1; // on quitte, il n'y a plus de place dans la DB
		hash_value%=MAX_CLIENTS-1;
	}

	// Mise en forme du tuple à ajouter
	printf("Ajout d'un utilisateur => %d %s %s %s %s\n", hash_value,firstname,lastname,password_md5,status);
	sprintf(db2char[hash_value-1],"%d %s %s %s %s\n", hash_value,firstname,lastname,password_md5,status);

	// On ré-écrit le contenu de la DB
	for(i=0;i<MAX_CLIENTS-1;i++)
	{
		fputs(db2char[i], file);
	}

	fclose(file);

	// Regénération de l'index
	parse_users_db(index_db_users);

	return 0;
}


