/* ==========================================================================
 * initialisation.c - Creation / destruction de la Boutique + jeu de donnees
 * --------------------------------------------------------------------------
 * Notions du cours utilisees : malloc + test de NULL (CM6), free (CM6),
 * tableau de structures initialise en dur (CM8), strcpy (CM7), pointeurs.
 *
 * REGLE D'OR DE LA MEMOIRE : tout malloc / realloc doit etre teste (NULL) et
 * tout bloc alloue doit etre libere UNE fois, dans l'ordre inverse de
 * l'allocation (les "enfants" d'abord, puis le "parent").
 * ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "initialisation.h"
#include "produit.h"
#include "client.h"

/* --------------------------------------------------------------------------
 * initialiserBoutique : prepare une Boutique vide mais utilisable
 * (deux tableaux dynamiques alloues, compteurs a leur valeur de depart).
 * Retourne 1 si tout s'est bien passe, 0 en cas de manque de memoire.
 * -------------------------------------------------------------------------- */
int initialiserBoutique(Boutique *b)
{
    strcpy(b->nom, "ShopFlow");

    /* Niveau 1 : le catalogue */
    b->capaciteProduits = CAPACITE_INITIALE_PRODUITS;
    b->nbProduits = 0;
    b->produits = (Produit *) malloc(b->capaciteProduits * sizeof(Produit));
    if (b->produits == NULL)
    {
        printf("Erreur : allocation du catalogue impossible.\n");
        return 0;
    }

    /* Niveau 1 : le repertoire clients */
    b->capaciteClients = CAPACITE_INITIALE_CLIENTS;
    b->nbClients = 0;
    b->clients = (Client *) malloc(b->capaciteClients * sizeof(Client));
    if (b->clients == NULL)
    {
        printf("Erreur : allocation du repertoire clients impossible.\n");
        free(b->produits);          /* on libere ce qui a deja ete alloue */
        b->produits = NULL;
        return 0;
    }

    /* Compteurs : les numeros et identifiants ne sont jamais saisis a la main.
     * Les commandes de test (partie 2) porteront les numeros 1001 a 1006. */
    b->numeroSuivant = 1001;
    b->idClientSuivant = 1;
    return 1;
}

/* --------------------------------------------------------------------------
 * ajouterClientTest : petite fonction interne qui evite de repeter 9 lignes
 * de strcpy pour chacun des 5 clients de demonstration.
 * -------------------------------------------------------------------------- */
int ajouterClientTest(Boutique *b, char *nom, char *prenom, char *email, char *telephone,
                      char *rue, char *ville, char *codePostal, char *pays)
{
    Client c;
    strcpy(c.nom, nom);
    strcpy(c.prenom, prenom);
    strcpy(c.email, email);
    strcpy(c.telephone, telephone);
    strcpy(c.adresse.rue, rue);
    strcpy(c.adresse.ville, ville);
    strcpy(c.adresse.codePostal, codePostal);
    strcpy(c.adresse.pays, pays);
    return insererClient(b, c);     /* attribue l'identifiant et initialise le reste */
}

/* --------------------------------------------------------------------------
 * initialiserDonneesTest : jeu de donnees de demonstration (enonce section 7)
 *  - 8 produits, 4 categories, 2 en alerte de stock (Souris : 8 <= 15 et
 *    Hub USB-C : 4 <= 10)
 *  - 5 clients avec adresses completes et identifiants distincts (1 a 5)
 *  - PARTIE 2 : les 6 commandes de test seront ajoutees ici.
 * -------------------------------------------------------------------------- */
int initialiserDonneesTest(Boutique *b)
{
    /* 1. Catalogue : tableau de structures initialise en dur.
     * Ordre des champs : reference, designation, categorie, prixHT,
     * stock, seuil d'alerte, actif. */
    Produit modele[] = {
        {"PRD-0001", "Clavier mecanique",   "Informatique", 79.90,  42, 10, 1},
        {"PRD-0002", "Souris sans fil",     "Informatique", 24.50,   8, 15, 1},
        {"PRD-0003", "Ecran 27 pouces",     "Informatique", 199.00,  6,  5, 1},
        {"PRD-0004", "Casque audio",        "Audio",        89.90,  25, 10, 1},
        {"PRD-0005", "Enceinte Bluetooth",  "Audio",        59.90,  12,  8, 1},
        {"PRD-0006", "Cable HDMI 2 m",      "Accessoires",  9.90,  120, 30, 1},
        {"PRD-0007", "Hub USB-C 7 ports",   "Accessoires",  49.90,   4, 10, 1},
        {"PRD-0008", "Disque SSD 1 To",     "Stockage",     109.00, 18,  6, 1}
    };
    int nbModeles = 8;

    for (int i = 0; i < nbModeles; i++)
    {
        if (insererProduit(b, modele[i]) == 0)
        {
            return 0;
        }
    }

    /* 2. Clients */
    if (ajouterClientTest(b, "Bernard", "Julie", "j.bernard@mail.fr", "0612345678",
                          "12 rue de la Paix", "Paris", "75002", "France") == 0) return 0;
    if (ajouterClientTest(b, "Dupont", "Marc", "marc.dupont@mail.fr", "0698765432",
                          "8 avenue Jean Jaures", "Lyon", "69007", "France") == 0) return 0;
    if (ajouterClientTest(b, "Martin", "Sophie", "sophie.martin@mail.fr", "0601020304",
                          "45 boulevard Gambetta", "Lille", "59000", "France") == 0) return 0;
    if (ajouterClientTest(b, "Leroy", "Thomas", "t.leroy@mail.fr", "0645678912",
                          "3 place du Capitole", "Toulouse", "31000", "France") == 0) return 0;
    if (ajouterClientTest(b, "Moreau", "Camille", "camille.moreau@mail.fr", "0677889900",
                          "27 quai des Chartrons", "Bordeaux", "33000", "France") == 0) return 0;

    /* PARTIE 2 : ici on creera les 6 commandes couvrant les 4 statuts. */
    return 1;
}

/* --------------------------------------------------------------------------
 * detruireBoutique : liberation COMPLETE (aucune fuite tolere).
 * Ordre : lignes -> commandes -> clients -> produits (enfants avant parents).
 * Si on liberait b->clients en premier, on perdrait l'adresse des tableaux
 * de commandes et ils ne pourraient plus jamais etre liberes (fuite).
 * -------------------------------------------------------------------------- */
void detruireBoutique(Boutique *b)
{
    for (int i = 0; i < b->nbClients; i++)
    {
        for (int j = 0; j < b->clients[i].nbCommandes; j++)
        {
            free(b->clients[i].commandes[j].lignes);    /* niveau 4 */
        }
        free(b->clients[i].commandes);                  /* niveau 3 */
    }
    free(b->clients);                                   /* niveau 2 */
    free(b->produits);                                  /* niveau 2 */

    b->clients = NULL;          /* on remet NULL : evite toute reutilisation */
    b->produits = NULL;
    b->nbClients = 0;
    b->nbProduits = 0;
    b->capaciteClients = 0;
    b->capaciteProduits = 0;
}
