/* ==========================================================================
 * produit.c - Module "Catalogue produits"
 * --------------------------------------------------------------------------
 * Notions du cours utilisees : struct et acces par '->' (CM8), tableau
 * dynamique + realloc (CM6), passage de pointeur en parametre (CM7),
 * strcmp / strcpy (CM7), boucles / conditions / switch (CM4).
 *
 * Le catalogue est le tableau  b->produits  :
 *   - taille LOGIQUE  = b->nbProduits       (cases reellement remplies)
 *   - taille PHYSIQUE = b->capaciteProduits (cases reservees en memoire)
 * Quand il est plein, on l'agrandit avec realloc.
 *
 * "Suppression logique" : on ne retire jamais un produit du tableau, on met
 * simplement actif = 0. Il disparait des affichages mais sa reference reste
 * reservee (controle d'unicite sur TOUS les produits, meme desactives).
 * ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "produit.h"
#include "utils.h"

/* --------------------------------------------------------------------------
 * chercherProduit : retourne l'indice du produit dans le tableau, -1 sinon.
 * Les produits desactives sont inclus (pour l'unicite des references).
 * -------------------------------------------------------------------------- */
int chercherProduit(Boutique *b, char *reference)
{
    for (int i = 0; i < b->nbProduits; i++)
    {
        if (egalSansCasse(b->produits[i].reference, reference) == 1)
        {
            return i;
        }
    }
    return -1;
}

/* --------------------------------------------------------------------------
 * agrandirCatalogue : double la capacite du tableau avec realloc.
 *
 * ATTENTION : on n'ecrit PAS  b->produits = realloc(b->produits, ...)
 * directement. Si realloc echoue il renvoie NULL et on perdrait l'ancien
 * pointeur (donc tout le catalogue + fuite memoire). On passe par une
 * variable temporaire "nouveau" qu'on teste avant de la recopier.
 * -------------------------------------------------------------------------- */
int agrandirCatalogue(Boutique *b)
{
    int nouvelleCapacite = b->capaciteProduits * 2;
    if (nouvelleCapacite == 0)
    {
        nouvelleCapacite = CAPACITE_INITIALE_PRODUITS;
    }

    Produit *nouveau = (Produit *) realloc(b->produits, nouvelleCapacite * sizeof(Produit));
    if (nouveau == NULL)
    {
        printf("Erreur : memoire insuffisante pour agrandir le catalogue.\n");
        return 0;                       /* l'ancien tableau reste valide */
    }
    b->produits = nouveau;
    b->capaciteProduits = nouvelleCapacite;
    return 1;
}

/* --------------------------------------------------------------------------
 * insererProduit : ajoute le produit p a la fin du catalogue.
 * p est passe PAR VALEUR (copie) : c'est la copie qui est rangee dans le tableau.
 * -------------------------------------------------------------------------- */
int insererProduit(Boutique *b, Produit p)
{
    if (chercherProduit(b, p.reference) != -1)
    {
        printf("Erreur : la reference %s existe deja (meme desactivee).\n", p.reference);
        return 0;
    }
    if (b->nbProduits == b->capaciteProduits)       /* tableau plein ? */
    {
        if (agrandirCatalogue(b) == 0)
        {
            return 0;
        }
    }
    b->produits[b->nbProduits] = p;                 /* on remplit la 1re case libre */
    b->nbProduits++;                                /* taille logique + 1           */
    return 1;
}

/* --------------------------------------------------------------------------
 * produitUtilise : 1 si le produit figure dans une ligne d'une commande
 * NON annulee. On parcourt les 3 niveaux : clients -> commandes -> lignes.
 * (En partie 1 il n'y a pas encore de commande, la fonction renvoie 0 ;
 * elle fonctionnera telle quelle des que les commandes existeront.)
 * -------------------------------------------------------------------------- */
int produitUtilise(Boutique *b, char *reference)
{
    for (int i = 0; i < b->nbClients; i++)
    {
        for (int j = 0; j < b->clients[i].nbCommandes; j++)
        {
            Commande *c = &b->clients[i].commandes[j];
            if (strcmp(c->statut, STATUT_ANNULEE) == 0)
            {
                continue;               /* commande annulee : on l'ignore */
            }
            for (int k = 0; k < c->nbLignes; k++)
            {
                if (strcmp(c->lignes[k].refProduit, reference) == 0)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * demanderProduit : demande une reference et retourne l'indice du produit
 * ACTIF correspondant, ou -1 s'il n'existe pas. (fonction interne)
 * -------------------------------------------------------------------------- */
int demanderProduit(Boutique *b)
{
    char reference[TAILLE_REFERENCE];

    if (saisirTexte("Reference du produit : ", reference, TAILLE_REFERENCE) == 0)
    {
        return -1;
    }
    int indice = chercherProduit(b, reference);
    if (indice == -1 || b->produits[indice].actif == 0)
    {
        printf("Aucun produit actif avec la reference %s.\n", reference);
        return -1;
    }
    return indice;
}

/* --------------------------------------------------------------------------
 * afficherLigneProduit : une ligne de tableau pour un produit.
 * Le produit est passe par ADRESSE (pointeur) : pas de copie, et on accede
 * aux champs avec la fleche '->'.
 * -------------------------------------------------------------------------- */
void afficherLigneProduit(Produit *p)
{
    char *etat = "OK";
    if (p->quantiteStock <= p->seuilAlerte)
    {
        etat = "ALERTE";
    }
    printf("%-9s %-22.22s %-12.12s %9.2f %6d %6d %12.2f  %s\n",
           p->reference, p->designation, p->categorie, p->prixUnitaireHT,
           p->quantiteStock, p->seuilAlerte,
           p->prixUnitaireHT * p->quantiteStock, etat);
}

/* En-tete du tableau (partage par le catalogue, la recherche et les alertes) */
void afficherEnteteProduits(void)
{
    printf("%-9s %-22s %-12s %9s %6s %6s %12s  %s\n",
           "Ref", "Designation", "Categorie", "Prix HT", "Stock", "Seuil",
           "Valeur stock", "Etat");
    afficherLigne(96);
}

/* --------------------------------------------------------------------------
 * ajouterProduit : saisie complete d'un nouveau produit
 * -------------------------------------------------------------------------- */
void ajouterProduit(Boutique *b)
{
    Produit p;
    int saisieOk = 0;

    afficherTitre("AJOUTER UN PRODUIT");

    /* 1. Reference : on redemande tant qu'elle existe deja */
    while (saisieOk == 0)
    {
        if (saisirTexte("Reference (ex: PRD-0009) : ", p.reference, TAILLE_REFERENCE) == 0)
        {
            return;
        }
        if (chercherProduit(b, p.reference) != -1)
        {
            printf("  -> Erreur : cette reference existe deja (meme desactivee).\n");
        }
        else
        {
            saisieOk = 1;
        }
    }

    /* 2. Les autres champs : chaque fonction de saisie controle la valeur */
    if (saisirTexte("Designation : ", p.designation, TAILLE_DESIGNATION) == 0) return;
    if (saisirTexte("Categorie : ", p.categorie, TAILLE_CATEGORIE) == 0) return;
    if (saisirPrix("Prix unitaire HT (> 0) : ", &p.prixUnitaireHT) == 0) return;
    if (saisirEntier("Quantite en stock (>= 0) : ", 0, STOCK_MAX, &p.quantiteStock) == 0) return;
    if (saisirEntier("Seuil d'alerte (>= 0) : ", 0, STOCK_MAX, &p.seuilAlerte) == 0) return;
    p.actif = 1;

    if (insererProduit(b, p) == 1)
    {
        printf("Produit %s ajoute au catalogue.\n", p.reference);
    }
}

/* --------------------------------------------------------------------------
 * modifierProduit : modification du prix, du stock et du seuil d'alerte
 * -------------------------------------------------------------------------- */
void modifierProduit(Boutique *b)
{
    afficherTitre("METTRE A JOUR UN PRODUIT");

    int indice = demanderProduit(b);
    if (indice == -1)
    {
        return;
    }
    Produit *p = &b->produits[indice];      /* pointeur vers la case du tableau */
    int choix = -1;

    while (choix != 0)
    {
        printf("\n");
        afficherEnteteProduits();
        afficherLigneProduit(p);
        printf("\n1. Modifier le prix HT\n2. Modifier le stock\n3. Modifier le seuil d'alerte\n0. Terminer\n");
        if (saisirEntier("Votre choix : ", 0, 3, &choix) == 0)
        {
            return;
        }
        switch (choix)
        {
            case 1:
                saisirPrix("Nouveau prix HT (> 0) : ", &p->prixUnitaireHT);
                break;
            case 2:
                saisirEntier("Nouveau stock (>= 0) : ", 0, STOCK_MAX, &p->quantiteStock);
                break;
            case 3:
                saisirEntier("Nouveau seuil (>= 0) : ", 0, STOCK_MAX, &p->seuilAlerte);
                break;
            default:
                break;
        }
    }
}

/* --------------------------------------------------------------------------
 * supprimerProduit : suppression LOGIQUE (actif = 0), refusee si le produit
 * est utilise dans une commande non annulee.
 * -------------------------------------------------------------------------- */
void supprimerProduit(Boutique *b)
{
    afficherTitre("SUPPRIMER UN PRODUIT");

    int indice = demanderProduit(b);
    if (indice == -1)
    {
        return;
    }
    Produit *p = &b->produits[indice];

    if (produitUtilise(b, p->reference) == 1)
    {
        printf("Suppression refusee : %s est utilise dans une commande non annulee.\n", p->reference);
        return;
    }

    int confirmation = 0;
    printf("Produit : %s - %s\n", p->reference, p->designation);
    if (saisirEntier("Confirmer la suppression ? (1 = oui, 0 = non) : ", 0, 1, &confirmation) == 0)
    {
        return;
    }
    if (confirmation == 1)
    {
        p->actif = 0;
        printf("Produit %s supprime (suppression logique).\n", p->reference);
    }
    else
    {
        printf("Suppression annulee.\n");
    }
}

/* --------------------------------------------------------------------------
 * afficherCatalogue : liste des produits actifs + valorisation du stock
 * (valorisation = prix HT x quantite en stock, additionnee sur le catalogue)
 * -------------------------------------------------------------------------- */
void afficherCatalogue(Boutique *b)
{
    float totalStock = 0;
    int nbAffiches = 0;

    afficherTitre("CATALOGUE DES PRODUITS");
    afficherEnteteProduits();

    for (int i = 0; i < b->nbProduits; i++)
    {
        if (b->produits[i].actif == 1)
        {
            afficherLigneProduit(&b->produits[i]);
            totalStock = totalStock + b->produits[i].prixUnitaireHT * b->produits[i].quantiteStock;
            nbAffiches++;
        }
    }
    afficherLigne(96);
    printf("%d produit(s) - Valorisation totale du stock : %.2f EUR HT\n", nbAffiches, totalStock);
}

/* --------------------------------------------------------------------------
 * rechercherProduit : par reference exacte OU par mot-cle dans la designation
 * -------------------------------------------------------------------------- */
void rechercherProduit(Boutique *b)
{
    int choix = 0;

    afficherTitre("RECHERCHER UN PRODUIT");
    printf("1. Par reference exacte\n2. Par mot-cle dans la designation\n");
    if (saisirEntier("Votre choix : ", 1, 2, &choix) == 0)
    {
        return;
    }

    if (choix == 1)
    {
        int indice = demanderProduit(b);
        if (indice != -1)
        {
            afficherEnteteProduits();
            afficherLigneProduit(&b->produits[indice]);
        }
    }
    else
    {
        char motCle[TAILLE_DESIGNATION];
        int nbTrouves = 0;

        if (saisirTexte("Mot-cle : ", motCle, TAILLE_DESIGNATION) == 0)
        {
            return;
        }
        for (int i = 0; i < b->nbProduits; i++)
        {
            if (b->produits[i].actif == 1
                && contientMotCle(b->produits[i].designation, motCle) == 1)
            {
                if (nbTrouves == 0)
                {
                    afficherEnteteProduits();       /* en-tete avant le 1er resultat */
                }
                afficherLigneProduit(&b->produits[i]);
                nbTrouves++;
            }
        }
        if (nbTrouves == 0)
        {
            printf("Aucun produit ne contient \"%s\".\n", motCle);
        }
        else
        {
            printf("%d resultat(s).\n", nbTrouves);
        }
    }
}

/* --------------------------------------------------------------------------
 * afficherAlertesStock : produits actifs dont stock <= seuil d'alerte
 * -------------------------------------------------------------------------- */
void afficherAlertesStock(Boutique *b)
{
    int nbAlertes = 0;

    afficherTitre("ALERTES DE STOCK");
    for (int i = 0; i < b->nbProduits; i++)
    {
        if (b->produits[i].actif == 1
            && b->produits[i].quantiteStock <= b->produits[i].seuilAlerte)
        {
            if (nbAlertes == 0)
            {
                afficherEnteteProduits();
            }
            afficherLigneProduit(&b->produits[i]);
            nbAlertes++;
        }
    }
    if (nbAlertes == 0)
    {
        printf("Aucun produit en alerte de stock.\n");
    }
    else
    {
        printf("%d produit(s) a reapprovisionner.\n", nbAlertes);
    }
}
