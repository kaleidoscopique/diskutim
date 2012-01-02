// Temps de bloquage du serveur entre chaque requête en MICROSECONDES
// 1 seconde = 1 000 000 microsecondes
#define SLEEP_NONBLOCK 50//0//000
/* En mettant un sleep trop long (avec les n zéros commenté) alors il y a des bugs
lors de la connexion : le client envoie un /connect, attend la réponse de retour.
En attendant, il ouvre la fenêtre de discussion qui, elle, demande la liste des services.
Tout s'enchaine et tout se mélange = bug.
En diminuant le SLEEP, tout va vite et tout fonctionne. Voir poll pour plus tard.
*/

// nb max de services que peut contenir le fichier services.conf
#define MAX_SERVICES 10
// taille max du nom d'un service
#define MAX_SIZE_SERVICE_NAME 40

 // nb max de clients qui peuvent se connecter simultanément
#define MAX_CLIENTS 30

// Taille max des data échangées
#define MAX_SIZE 1024

// Taille maximale d'une donnée client (identity, etc)
#define SHEET_MAX_SIZE 40

// Séparateur des données dans les paquets du protocole
#define TRAME_SEPARATOR "+"

// Taille du token généré par le serveur pour l'envoi d'un message
#define TOKEN_SIZE 6

// Le titre de la fenêtre avec GTK
#define TITLE_MAINWINDOW "Diskutim"

// Define concernant le partage de fichiers
#define MAX_NB_FILES 30
#define MAX_SIZE_FILE_NAME 50
// Taille maximale du contenu d'un fichier
#define MAX_SIZE_FILE_CONTENT 5000

#define DEBUG(x) printf(x)
