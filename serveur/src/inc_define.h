/**
	@mainpage Diskutim Serveur
		Bienvenue sur la documentation DISKUTIM SERVEUR.

		Pour comprendre comment fonctionne le protocole DKTM, cliquez ici : http://code.google.com/p/diskutim/wiki/FonctionnementDuProtocole


		Pour plus d'informations sur l'évolution du logiciel, rendez-vous sur notre espace Google Code : http://code.google.com/p/diskutim

		Enjoy !
*/

// Temps de bloquage du serveur entre chaque requête en MICROSECONDES
// 1 seconde = 1 000 000 microsecondes
#define SLEEP_NONBLOCK 500000

 // nb max de clients qui peuvent se connecter simultanément
#define MAX_CLIENTS 30

// nb max de services que peut contenir le fichier services.conf
#define MAX_SERVICES 10

#define TRAME_SEPARATOR "+"

// Taille max de chaque info d'un client (nom, prenom, services, others...)
#define SHEET_MAX_SIZE 40
// taille max du nom d'un service
#define MAX_SIZE_SERVICE_NAME 40

// Taille max des data échangées
#define MAX_SIZE 1024

// Port utilisé pour communiquer
#define PORT 7382

// Taille du token généré par le serveur pour l'envoi d'un message
#define TOKEN_SIZE 6

// Taille maximale du contenu d'un fichier
#define MAX_SIZE_FILE_CONTENT 5000
#define MAX_SIZE_FILE_NAME 50

