
#include <stdio.h>
#include "produit.h"
#include "types.h"
#include "utils.h"
#include "client.h"
#include "initialisation.h"


void menuProduits(Boutique *b)
{
    int choix = -1;

    while (choix != 0)
    {
        afficherTitre("CATALOGUE PRODUITS");
        printf("1. Afficher le catalogue\n");
        printf("2. Rechercher un produit\n");
        printf("3. Ajouter un produit\n");
        printf("4. Mettre a jour un produit (prix, stock, seuil)\n");
        printf("5. Supprimer un produit\n");
        printf("6. Alertes de stock\n");
        printf("0. Retour\n");
        if (saisirEntier("Votre choix : ", 0, 6, &choix) == 0)
        {
            return;                          /* fin de l'entree */
        }
        switch (choix)
        {
            case 1: afficherCatalogue(b);     attendreEntree(); break;
            case 2: rechercherProduit(b);     attendreEntree(); break;
            case 3: ajouterProduit(b);        attendreEntree(); break;
            case 4: modifierProduit(b);       attendreEntree(); break;
            case 5: supprimerProduit(b);      attendreEntree(); break;
            case 6: afficherAlertesStock(b);  attendreEntree(); break;
            default: break;
        }
    }
}

/* --------------------------------------------------------------------------
 * Menu "Clients"
 * -------------------------------------------------------------------------- */
void menuClients(Boutique *b)
{
    int choix = -1;

    while (choix != 0)
    {
        afficherTitre("CLIENTS");
        printf("1. Afficher la liste des clients\n");
        printf("2. Afficher la fiche d'un client\n");
        printf("3. Ajouter un client\n");
        printf("4. Mettre a jour un client\n");
        printf("5. Supprimer un client\n");
        printf("0. Retour\n");
        if (saisirEntier("Votre choix : ", 0, 5, &choix) == 0)
        {
            return;
        }
        switch (choix)
        {
            case 1: afficherListeClients(b);  attendreEntree(); break;
            case 2: afficherUnClient(b);      attendreEntree(); break;
            case 3: ajouterClient(b);         attendreEntree(); break;
            case 4: modifierClient(b);        attendreEntree(); break;
            case 5: supprimerClient(b);       attendreEntree(); break;
            default: break;
        }
    }
}

/* --------------------------------------------------------------------------
 * main : creation de la Boutique, boucle principale, liberation finale
 * -------------------------------------------------------------------------- */
int main(void)
{
    Boutique boutique;                       /* structure racine */

    if (initialiserBoutique(&boutique) == 0)
    {
        return 1;                            /* rien d'alloue : on s'arrete */
    }
    if (initialiserDonneesTest(&boutique) == 0)
    {
        printf("Erreur lors du chargement des donnees de test.\n");
        detruireBoutique(&boutique);         /* on libere ce qui existe deja */
        return 1;
    }

    int choix = -1;
    while (choix != 0)
    {
        printf("\n");
        afficherLigne(78);
        printf("  %s - GESTION DES COMMANDES D'UNE BOUTIQUE EN LIGNE\n", boutique.nom);
        afficherLigne(78);
        printf("1. Catalogue produits\n");
        printf("2. Clients\n");
        printf("   (Commandes et statistiques : partie 2)\n");
        printf("0. Quitter\n");
        if (saisirEntier("Votre choix : ", 0, 2, &choix) == 0)
        {
            break;                           /* fin de l'entree : on quitte proprement */
        }
        switch (choix)
        {
            case 1: menuProduits(&boutique); break;
            case 2: menuClients(&boutique);  break;
            default: break;
        }
    }

    detruireBoutique(&boutique);             /* liberation complete de la memoire */
    printf("Au revoir !\n");
    return 0;
}
