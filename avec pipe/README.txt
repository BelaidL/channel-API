# Vue globale de l'API et les differentes actions possibles

## Structure du canal "channel"

```
struct channel{
	int closed; // si le channel est fermé
	unsigned int size; // le nombre des elements
	unsigned int eltsize; // la taille de chaque element
	void *data; // le pointeur du debut de la zone memoire 
	void *pread; // un pointeur utilisé pour lire dans la zone 
	void *pwrite;// un pointeur pour ecrir dans la zone 
	pthread_mutex_t verrou; // un verrou pour géré les lectures et les ecriturs 
};
```
## Création d'un canal
La création et l'initialisation du canal se font à l'aide de la fonction **channel_create** qui prend en arguments le nombre maximume d'éléments ainsi la taille maximale de chaque élément du canal, si la creation du canal est réussi, la fonction renvoie un pointeur vers le canal créé.

```
struct channel *channel_create(int eltsize, int size, int flags){
 // on crré un channel et on enitialisent les champs 
 // on allou la memoire pour data de taille eltsize * size
 // on enitialise pread et pwrite
 // on retourne le channel crée 
}
```
## Libération du canal
La libération du canal se fait à laide de la fonction **channel_distroy** qui prend en arguments le canal à libérer, la fonction retourne -1 au cas d'erreur.

```
void channel_destroy(struct channel *channel){
	// detruire le channel on libérent la memoire de data 
	si (le channel n'est pas null)
 		on libérent la mémoire de data
	sinon
		on posistionne errno et on retourne -1
}
```
## Envoie des données dans le canal
L'écriture des données dans le canal se fait à l'aide de la fonction **channel_send**, elle prend en argummants le canal dans le quel on veut écrire et la données à écrire.

```
int channel_send(struct channel *channel, const void *data){
	si (le channel est fermé)
		// on positionne errno et on retourne -1
	sinon {
		si (le channel n'est pas plein)
			// on ecrit l'élement
		sinon
			// on bloque jusque la libération d'un élément
	}
}
```
## Fermeture de canal
La fermuture du canal se fait à l'aide de la fionction **channel_close**, elle prend en argument le canal à fermer, elle retourne -1 au cas d'erreur.

```
int channel_close(struct channel *channel){
	// on verifie channel->closeed 
	si (c'est 1) : on retourne 0
	si (c'est 0) : on retourne 1
	sinon on on positionne errno et on retourne -1	
}
```
## Reciption des données 
La lecture des données se fait à l'aide de la fonction **channel_recv**, elle prend en arguments le canal et une variable vers la quelle la donnée sera lu.

```
int channel_recv(struct channel *channel, void *data){
 si le channel est fermé 
 	on positionne errno et on retourne 0
 sinon {
	si le channel n'est pas vide
		on lis l'élement
	sinon 
		on bloque jusque la l'écriture d'un élément
	}
}
```
