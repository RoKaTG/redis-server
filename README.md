# Serveur Redis

Ce projet est un serveur Redis minimaliste implémenté en C. Il fournit un ensemble de commandes Redis de base telles que PING, SET, GET, DEL, et d'autres. Le but de ce serveur est de servir d'exemple éducatif pour comprendre le fonctionnement interne d'un serveur Redis simplifié.

## Installation

1. Clonez ce dépôt sur votre système local en utilisant la commande suivante :

```shell
git clone https://github.com/RoKaTG/redis-server.git
```
 Accédez au répertoire du projet :

```shell

cd serveur-redis-minimaliste
```
Compilez le serveur en utilisant la commande make :

```shell
make
```
Utilisation

   Démarrez le serveur en spécifiant un numéro de port (par exemple, 8080) :

```shell

./server 8080
```
Le serveur Redis minimaliste est maintenant en cours d'exécution sur le port spécifié.

Pour utiliser le client Redis en ligne de commande, ouvrez une nouvelle fenêtre de terminal et utilisez la commande redis-cli en spécifiant le même numéro de port que le serveur :

```shell

redis-cli -p 8080
```
Vous pouvez maintenant utiliser les commandes Redis prises en charge. Par exemple, pour obtenir de l'aide sur les commandes disponibles, tapez :

```shell

HELPER
```
   Cela affichera une liste de commandes prises en charge avec leurs descriptions.

Commandes prises en charge

    PING: Teste la connectivité au serveur.
    SET key value: Définit la valeur d'une clé.
    GET key: Récupère la valeur d'une clé.
    DEL key [key ...]: Supprime une ou plusieurs clés.
    EXISTS key [key ...]: Vérifie si une ou plusieurs clés existent.
    APPEND key value: Ajoute une valeur à la fin de la clé.
    RANDOMKEY: Retourne une clé aléatoire.
    EXPIRE key seconds: Définit un délai d'expiration sur une clé.
    PEXPIRE key milliseconds: Définit un délai d'expiration sur une clé en millisecondes.
    PERSIST key: Supprime le délai d'expiration d'une clé.
    TTL key: Obtient le temps restant avant expiration d'une clé en secondes.
    PTTL key: Obtient le temps restant avant expiration d'une clé en millisecondes.
    KEYS pattern: Trouve toutes les clés correspondant au motif donné.
    RENAME key newkey: Renomme une clé.
    COPY source destination: Copie la valeur d'une clé vers une autre.
    INCR key: Incrémente la valeur numérique d'une clé.
    DECR key: Décrémente la valeur numérique d'une clé.
    INCRBY key increment: Incrémente la valeur numérique d'une clé par un montant donné.
    DECRBY key decrement: Décrémente la valeur numérique d'une clé par un montant donné.

Au niveau des fonctionnalités, j'ai normalement compléter la partie BASE et INTERMEDIAIRE du sujet (sauf les requête concurrentes ou je n'ai pas eu le temps d'intégrer les mutex et les deadlock dans chaque fonctionnalités faites) et je n'ai pas pu touché à la partie AVANCEE du projet.
©MSILINI Yassine
