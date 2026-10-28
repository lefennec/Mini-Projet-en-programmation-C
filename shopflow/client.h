/* ==========================================================================
 * client.h - Module "Clients"
 * CRUD des clients, adresses de livraison, points de fidelite
 * ========================================================================== */

#ifndef CLIENT_H
#define CLIENT_H

#include "types.h"

/* ----- Fonctions "techniques" (sans saisie clavier) --------------------- */
int   chercherClientParId(Boutique *b, int idClient);   /* indice ou -1     */
int   chercherClientParEmail(Boutique *b, char *email); /* indice ou -1     */
int   agrandirClients(Boutique *b);                     /* realloc : 1 ou 0 */
int   insererClient(Boutique *b, Client c);             /* 1 = ajoute, 0 = echec */
int   clientAUneCommandeActive(Client *c);              /* 1 si commande non annulee */
float chiffreAffairesClient(Client *c);                 /* CA HT hors annulees */

/* ----- Fonctionnalites du menu ------------------------------------------ */
void ajouterClient(Boutique *b);
void modifierClient(Boutique *b);
void supprimerClient(Boutique *b);
void afficherListeClients(Boutique *b);
void afficherUnClient(Boutique *b);

#endif /* CLIENT_H */
