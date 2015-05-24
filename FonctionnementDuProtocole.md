# Description du protocole et des commandes client #

## Description du protocole ##

Le serveur n'envoie que des messages commençant par un slash ("/").

  * **`/USERS+user1+user2+user3...`** : transmet une liste d'utilisateurs connectés au service de l'utilisateur;
  * **`/SERVICES+service1+service2+service3...`** : transmet une liste de services disponibles server-side;
  * **`/MSG[Token]Message[Token]Auteur[Token]Services`** : transmet un message, son auteur et le service concerné
  * **`/SYS+XXX <info>`** : transmet le code erreur XXX et les info
  * **`/GET+<file>+<content>`** : transmet le contenu d'un fichier et son nom  un client
  * **`/FILES+<file1>+<file2>+...`** : transmet la liste des fichiers partagés à un client

## Commandes client ##

Le client dispose des commandes suivantes :
  * **`/connect <username> <password> <service>`** : authentification au serveur
  * **`/services`** : demande la liste des services disponibles
  * **`/users`** : demande la liste des utilisateurs connectés au service
  * **`/files`** : demande la liste des fichiers partagés au serveur
  * **`/get <file>`** : demande le fichier partagé `<file>` au serveur
  * **`/adduser <prenom> <nom> <statut>`** : ajoute un utilisateur à la DB (none, hop, aop, sop)
  * **`/quit`** : déconnecte le client