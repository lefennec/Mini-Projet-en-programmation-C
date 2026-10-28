

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "utils.h"

/* --------------------------------------------------------------------------
 * Recherches : retournent l'indice dans le tableau, -1 si introuvable.
 * Les clients desactives sont inclus (unicite sur tous les enregistrements).
 * -------------------------------------------------------------------------- */
int chercherClientParId(Boutique *b, int idClient)
{
    for (int i = 0; i < b->nbClients; i++)
    {
        if (b->clients[i].idClient == idClient)
        {
            return i;
        }
    }
    return -1;
}

int chercherClientParEmail(Boutique *b, char *email)
{
    for (int i = 0; i < b->nbClients; i++)
    {
        if (egalSansCasse(b->clients[i].email, email) == 1)
        {
            return i;
        }
    }
    return -1;
}

/* --------------------------------------------------------------------------
 * agrandirClients : double la capacite du repertoire (meme technique que
 * agrandirCatalogue : realloc dans une variable temporaire).
 * -------------------------------------------------------------------------- */
int agrandirClients(Boutique *b)
{
    int nouvelleCapacite = b->capaciteClients * 2;
    if (nouvelleCapacite == 0)
    {
        nouvelleCapacite = CAPACITE_INITIALE_CLIENTS;
    }

    Client *nouveau = (Client *) realloc(b->clients, nouvelleCapacite * sizeof(Client));
    if (nouveau == NULL)
    {
        printf("Erreur : memoire insuffisante pour agrandir le repertoire clients.\n");
        return 0;
    }
    b->clients = nouveau;
    b->capaciteClients = nouvelleCapacite;
    return 1;
}

/* --------------------------------------------------------------------------
 * insererClient : range une copie de c dans le repertoire.
 * L'appelant remplit nom, prenom, email, telephone et adresse ; ICI on
 * attribue l'identifiant et on initialise tout le reste.
 * -------------------------------------------------------------------------- */
int insererClient(Boutique *b, Client c)
{
    if (chercherClientParEmail(b, c.email) != -1)
    {
        printf("Erreur : l'adresse %s est deja utilisee (meme par un client desactive).\n", c.email);
        return 0;
    }
    if (b->nbClients == b->capaciteClients)
    {
        if (agrandirClients(b) == 0)
        {
            return 0;
        }
    }

    c.idClient = b->idClientSuivant;     /* identifiant fourni par la Boutique */
    b->idClientSuivant++;
    c.pointsFidelite = 0;
    c.nbCommandes = 0;
    c.capaciteCommandes = 0;
    c.commandes = NULL;                  /* pas encore de tableau de commandes */
    c.actif = 1;

    b->clients[b->nbClients] = c;
    b->nbClients++;
    return 1;
}

/* --------------------------------------------------------------------------
 * clientAUneCommandeActive : 1 si le client a au moins une commande dont le
 * statut n'est pas ANNULEE.
 * -------------------------------------------------------------------------- */
int clientAUneCommandeActive(Client *c)
{
    for (int i = 0; i < c->nbCommandes; i++)
    {
        if (strcmp(c->commandes[i].statut, STATUT_ANNULEE) != 0)
        {
            return 1;
        }
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * chiffreAffairesClient : somme des montants HT des commandes non annulees.
 * (CALCUL EN CASCADE : Ligne -> Commande -> Client ; le montant HT de la
 * commande est calcule en partie 2.)
 * -------------------------------------------------------------------------- */
float chiffreAffairesClient(Client *c)
{
    float total = 0;
    for (int i = 0; i < c->nbCommandes; i++)
    {
        if (strcmp(c->commandes[i].statut, STATUT_ANNULEE) != 0)
        {
            total = total + c->commandes[i].montantHT;
        }
    }
    return total;
}

/* --------------------------------------------------------------------------
 * saisirAdresse : saisie des 4 champs d'une adresse. On ecrit dans *a
 * (passage par adresse) : l'appelant recupere l'adresse remplie.
 * -------------------------------------------------------------------------- */
int saisirAdresse(Adresse *a)
{
    if (saisirTexte("  Rue (numero et libelle) : ", a->rue, TAILLE_RUE) == 0) return 0;
    if (saisirTexte("  Ville : ", a->ville, TAILLE_VILLE) == 0) return 0;
    if (saisirChiffres("  Code postal (5 chiffres) : ", a->codePostal, TAILLE_CODE_POSTAL, 5) == 0) return 0;
    if (saisirTexte("  Pays : ", a->pays, TAILLE_PAYS) == 0) return 0;
    return 1;
}

/* --------------------------------------------------------------------------
 * saisirEmailUnique : redemande tant que l'email est invalide ou deja pris
 * par un AUTRE client (indiceExclu = client en cours de modification, -1
 * pour un nouveau client).
 * -------------------------------------------------------------------------- */
int saisirEmailUnique(Boutique *b, char *dest, int indiceExclu)
{
    while (1)
    {
        if (saisirEmail("Email : ", dest, TAILLE_EMAIL) == 0)
        {
            return 0;
        }
        int indice = chercherClientParEmail(b, dest);
        if (indice == -1 || indice == indiceExclu)
        {
            return 1;
        }
        printf("  -> Erreur : cette adresse email est deja utilisee.\n");
    }
}

/* --------------------------------------------------------------------------
 * demanderClient : demande un identifiant, retourne l'indice du client ACTIF
 * correspondant ou -1.
 * -------------------------------------------------------------------------- */
int demanderClient(Boutique *b)
{
    int idClient = 0;

    if (saisirEntier("Identifiant du client : ", 1, 999999, &idClient) == 0)
    {
        return -1;
    }
    int indice = chercherClientParId(b, idClient);
    if (indice == -1 || b->clients[indice].actif == 0)
    {
        printf("Aucun client actif avec l'identifiant %d.\n", idClient);
        return -1;
    }
    return indice;
}

/* --------------------------------------------------------------------------
 * ajouterClient
 * -------------------------------------------------------------------------- */
void ajouterClient(Boutique *b)
{
    Client c;

    afficherTitre("AJOUTER UN CLIENT");
    if (saisirTexte("Nom : ", c.nom, TAILLE_NOM) == 0) return;
    if (saisirTexte("Prenom : ", c.prenom, TAILLE_PRENOM) == 0) return;
    if (saisirEmailUnique(b, c.email, -1) == 0) return;
    if (saisirChiffres("Telephone (10 chiffres) : ", c.telephone, TAILLE_TELEPHONE, 10) == 0) return;
    printf("Adresse de livraison :\n");
    if (saisirAdresse(&c.adresse) == 0) return;

    if (insererClient(b, c) == 1)
    {
        /* Le client a ete copie dans le tableau : son identifiant est celui
         * de la derniere case remplie. */
        printf("Client ajoute avec l'identifiant %d.\n", b->clients[b->nbClients - 1].idClient);
    }
}

/* --------------------------------------------------------------------------
 * modifierClient : coordonnees et adresse de livraison
 * -------------------------------------------------------------------------- */
void modifierClient(Boutique *b)
{
    afficherTitre("METTRE A JOUR UN CLIENT");

    int indice = demanderClient(b);
    if (indice == -1)
    {
        return;
    }
    Client *c = &b->clients[indice];
    int choix = -1;

    while (choix != 0)
    {
        printf("\nClient %d : %s %s | %s | %s\n", c->idClient, c->prenom, c->nom, c->email, c->telephone);
        printf("Adresse : %s, %s %s, %s\n", c->adresse.rue, c->adresse.codePostal,
               c->adresse.ville, c->adresse.pays);
        printf("\n1. Nom\n2. Prenom\n3. Email\n4. Telephone\n5. Adresse de livraison\n0. Terminer\n");
        if (saisirEntier("Votre choix : ", 0, 5, &choix) == 0)
        {
            return;
        }
        switch (choix)
        {
            case 1:
                saisirTexte("Nouveau nom : ", c->nom, TAILLE_NOM);
                break;
            case 2:
                saisirTexte("Nouveau prenom : ", c->prenom, TAILLE_PRENOM);
                break;
            case 3:
            {
                /* On saisit dans une copie : l'ancien email n'est remplace
                 * que si la saisie est valide et unique. */
                char nouvelEmail[TAILLE_EMAIL];
                if (saisirEmailUnique(b, nouvelEmail, indice) == 1)
                {
                    strcpy(c->email, nouvelEmail);
                }
                break;
            }
            case 4:
                saisirChiffres("Nouveau telephone (10 chiffres) : ", c->telephone, TAILLE_TELEPHONE, 10);
                break;
            case 5:
            {
                Adresse nouvelle;
                printf("Nouvelle adresse de livraison :\n");
                if (saisirAdresse(&nouvelle) == 1)
                {
                    c->adresse = nouvelle;      /* copie de structure complete */
                }
                break;
            }
            default:
                break;
        }
    }
}

/* --------------------------------------------------------------------------
 * supprimerClient : suppression LOGIQUE, refusee s'il reste des commandes
 * non annulees.
 * -------------------------------------------------------------------------- */
void supprimerClient(Boutique *b)
{
    afficherTitre("SUPPRIMER UN CLIENT");

    int indice = demanderClient(b);
    if (indice == -1)
    {
        return;
    }
    Client *c = &b->clients[indice];

    if (clientAUneCommandeActive(c) == 1)
    {
        printf("Suppression refusee : %s %s a des commandes non annulees.\n", c->prenom, c->nom);
        return;
    }

    int confirmation = 0;
    printf("Client : %s %s (%s)\n", c->prenom, c->nom, c->email);
    if (saisirEntier("Confirmer la suppression ? (1 = oui, 0 = non) : ", 0, 1, &confirmation) == 0)
    {
        return;
    }
    if (confirmation == 1)
    {
        c->actif = 0;
        printf("Client %d supprime (suppression logique).\n", c->idClient);
    }
    else
    {
        printf("Suppression annulee.\n");
    }
}

/* --------------------------------------------------------------------------
 * afficherListeClients : vue d'ensemble (nombre de commandes + CA genere)
 * -------------------------------------------------------------------------- */
void afficherListeClients(Boutique *b)
{
    int nbAffiches = 0;

    afficherTitre("LISTE DES CLIENTS");
    printf("%-4s %-26s %-28s %6s %12s\n", "Id", "Client", "Email", "Cmd", "CA HT (EUR)");
    afficherLigne(80);

    for (int i = 0; i < b->nbClients; i++)
    {
        Client *c = &b->clients[i];
        if (c->actif == 1)
        {
            char nomComplet[TAILLE_NOM + TAILLE_PRENOM + 2];
            strcpy(nomComplet, c->prenom);
            strcat(nomComplet, " ");                /* strcat : concatenation (CM7) */
            strcat(nomComplet, c->nom);
            printf("%-4d %-26.26s %-28.28s %6d %12.2f\n", c->idClient, nomComplet,
                   c->email, c->nbCommandes, chiffreAffairesClient(c));
            nbAffiches++;
        }
    }
    afficherLigne(80);
    printf("%d client(s).\n", nbAffiches);
}

/* --------------------------------------------------------------------------
 * afficherUnClient : fiche detaillee + historique complet de ses commandes
 * -------------------------------------------------------------------------- */
void afficherUnClient(Boutique *b)
{
    afficherTitre("FICHE CLIENT");

    int indice = demanderClient(b);
    if (indice == -1)
    {
        return;
    }
    Client *c = &b->clients[indice];

    printf("\nIdentifiant   : %d\n", c->idClient);
    printf("Nom           : %s %s\n", c->prenom, c->nom);
    printf("Email         : %s\n", c->email);
    printf("Telephone     : %s\n", c->telephone);
    printf("Adresse       : %s, %s %s, %s\n", c->adresse.rue, c->adresse.codePostal,
           c->adresse.ville, c->adresse.pays);
    printf("Points fidelite : %d\n", c->pointsFidelite);
    printf("CA genere (HT)  : %.2f EUR\n", chiffreAffairesClient(c));

    printf("\nHistorique des commandes (%d) :\n", c->nbCommandes);
    if (c->nbCommandes == 0)
    {
        printf("  Aucune commande.\n");
        return;
    }
    printf("  %-8s %-12s %-10s %7s %12s\n", "Numero", "Date", "Statut", "Lignes", "Total TTC");
    afficherLigne(56);
    for (int i = 0; i < c->nbCommandes; i++)
    {
        Commande *cmd = &c->commandes[i];
        printf("  %-8d %02d/%02d/%-6d %-10s %7d %12.2f\n", cmd->numero,
               cmd->dateCommande.jour, cmd->dateCommande.mois, cmd->dateCommande.annee,
               cmd->statut, cmd->nbLignes, cmd->montantTTC);
    }
}
