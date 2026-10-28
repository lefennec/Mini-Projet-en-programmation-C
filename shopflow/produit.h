/* ==========================================================================
 * produit.h - Module "Catalogue produits"
 * CRUD des produits, gestion du stock et alertes de reapprovisionnement
 * ========================================================================== */

#ifndef PRODUIT_H
#define PRODUIT_H

#include "types.h"

/* Plafond de securite pour les saisies de stock / seuil */
#define STOCK_MAX 1000000

/* ----- Fonctions "techniques" (sans saisie clavier) --------------------- */
int  chercherProduit(Boutique *b, char *reference);   /* indice ou -1       */
int  agrandirCatalogue(Boutique *b);                  /* realloc : 1 ou 0   */
int  insererProduit(Boutique *b, Produit p);          /* 1 = ajoute, 0 = echec */
int  produitUtilise(Boutique *b, char *reference);    /* 1 si dans une commande non annulee */

/* ----- Fonctionnalites du menu (avec saisie / affichage) ---------------- */
void ajouterProduit(Boutique *b);
void modifierProduit(Boutique *b);
void supprimerProduit(Boutique *b);
void afficherCatalogue(Boutique *b);
void rechercherProduit(Boutique *b);
void afficherAlertesStock(Boutique *b);

#endif /* PRODUIT_H */
