

#include <stdio.h>
#include <string.h>
#include "utils.h"

/* --------------------------------------------------------------------------
 * lireLigne : lit une ligne au clavier dans dest (taille = taille du tableau)
 * -------------------------------------------------------------------------- */
int lireLigne(char *dest, int taille)
{
    /* fgets lit au plus (taille - 1) caracteres + ajoute '\0'.
     * Elle s'arrete aussi au retour a la ligne '\n' (qu'elle conserve). */
    if (fgets(dest, taille, stdin) == NULL)
    {
        dest[0] = '\0';      /* rien lu : on renvoie une chaine vide */
        return 0;            /* fin de l'entree                      */
    }

    /* strcspn(dest, "\n") = position du premier '\n' (technique du cours
     * pour supprimer le '\n' de fin de ligne). */
    int position = strcspn(dest, "\n");
    if (dest[position] == '\n')
    {
        dest[position] = '\0';   /* on remplace le '\n' par la fin de chaine */
        return 1;
    }

    /* Pas de '\n' trouve : soit la ligne est plus longue que le tableau,
     * soit c'est la derniere ligne du fichier. On vide le reste de la ligne
     * pour ne pas polluer la saisie suivante. */
    char reste[64];
    int tropLong = 0;
    while (1)
    {
        if (fgets(reste, sizeof(reste), stdin) == NULL)
        {
            break;                           /* plus rien a lire */
        }
        int n = strcspn(reste, "\n");
        if (n > 0)
        {
            tropLong = 1;                    /* il y avait des caracteres en trop */
        }
        if (reste[n] == '\n')
        {
            break;                           /* fin de la ligne atteinte */
        }
    }

    if (tropLong == 1)
    {
        return -1;
    }
    return 1;
}

/* --------------------------------------------------------------------------
 * convertirEntier : "123" -> 123 (retourne 0 si le texte n'est pas un entier)
 * Chaque caractere '0'..'9' vaut (caractere - '0'), on construit le nombre
 * chiffre par chiffre : valeur = valeur * 10 + chiffre.
 * -------------------------------------------------------------------------- */
int convertirEntier(char *texte, int *resultat)
{
    int longueur = strlen(texte);
    int i = 0;
    int signe = 1;
    int valeur = 0;

    if (longueur == 0)
    {
        return 0;
    }
    if (texte[0] == '-')
    {
        signe = -1;
        i = 1;
    }
    /* Au moins un chiffre, et 9 chiffres maximum (evite le depassement d'int) */
    if (i >= longueur || longueur - i > 9)
    {
        return 0;
    }
    while (i < longueur)
    {
        if (texte[i] < '0' || texte[i] > '9')
        {
            return 0;            /* caractere interdit */
        }
        valeur = valeur * 10 + (texte[i] - '0');
        i++;
    }
    *resultat = signe * valeur;  /* ecriture dans la variable de l'appelant */
    return 1;
}

/* --------------------------------------------------------------------------
 * convertirDecimal : "79.90" ou "79,90" -> 79.9 (retourne 0 si invalide)
 * Partie entiere et partie decimale sont construites separemment.
 * -------------------------------------------------------------------------- */
int convertirDecimal(char *texte, float *resultat)
{
    int longueur = strlen(texte);
    int nbChiffres = 0;
    int apresVirgule = 0;        /* 0 = on lit la partie entiere, 1 = decimale */
    double partieEntiere = 0;
    double partieDecimale = 0;
    double diviseur = 1;

    for (int i = 0; i < longueur; i++)
    {
        char c = texte[i];
        if (c == '.' || c == ',')
        {
            if (apresVirgule == 1)
            {
                return 0;        /* deux separateurs : invalide */
            }
            apresVirgule = 1;
        }
        else if (c >= '0' && c <= '9')
        {
            nbChiffres++;
            if (nbChiffres > 9)
            {
                return 0;        /* nombre trop grand */
            }
            if (apresVirgule == 0)
            {
                partieEntiere = partieEntiere * 10 + (c - '0');
            }
            else
            {
                partieDecimale = partieDecimale * 10 + (c - '0');
                diviseur = diviseur * 10;
            }
        }
        else
        {
            return 0;            /* caractere interdit */
        }
    }
    if (nbChiffres == 0)
    {
        return 0;
    }
    *resultat = (float)(partieEntiere + partieDecimale / diviseur);
    return 1;
}

/* --------------------------------------------------------------------------
 * saisirEntier : demande un entier compris entre min et max (inclus)
 * -------------------------------------------------------------------------- */
int saisirEntier(char *invite, int min, int max, int *resultat)
{
    char tampon[32];
    int valeur = 0;

    while (1)
    {
        printf("%s", invite);
        int etat = lireLigne(tampon, sizeof(tampon));
        if (etat == 0)
        {
            return 0;            /* fin de l'entree : on abandonne */
        }
        if (etat == 1 && convertirEntier(tampon, &valeur) == 1
            && valeur >= min && valeur <= max)
        {
            *resultat = valeur;
            return 1;
        }
        printf("  -> Erreur : entrez un entier entre %d et %d.\n", min, max);
    }
}

/* --------------------------------------------------------------------------
 * saisirPrix : demande un nombre decimal strictement positif
 * -------------------------------------------------------------------------- */
int saisirPrix(char *invite, float *resultat)
{
    char tampon[32];
    float valeur = 0;

    while (1)
    {
        printf("%s", invite);
        int etat = lireLigne(tampon, sizeof(tampon));
        if (etat == 0)
        {
            return 0;
        }
        if (etat == 1 && convertirDecimal(tampon, &valeur) == 1 && valeur > 0)
        {
            *resultat = valeur;
            return 1;
        }
        printf("  -> Erreur : entrez un nombre strictement positif (ex: 79.90).\n");
    }
}

/* --------------------------------------------------------------------------
 * saisirTexte : demande un texte non vide qui tient dans dest
 * -------------------------------------------------------------------------- */
int saisirTexte(char *invite, char *dest, int taille)
{
    while (1)
    {
        printf("%s", invite);
        int etat = lireLigne(dest, taille);
        if (etat == 0)
        {
            return 0;
        }
        if (etat == -1)
        {
            printf("  -> Erreur : texte trop long (%d caracteres maximum).\n", taille - 1);
        }
        else if (strlen(dest) == 0)
        {
            printf("  -> Erreur : ce champ ne peut pas etre vide.\n");
        }
        else
        {
            return 1;
        }
    }
}

/* --------------------------------------------------------------------------
 * saisirChiffres : demande exactement nbChiffres chiffres (telephone, code postal)
 * -------------------------------------------------------------------------- */
int saisirChiffres(char *invite, char *dest, int taille, int nbChiffres)
{
    char tampon[32];

    while (1)
    {
        printf("%s", invite);
        int etat = lireLigne(tampon, sizeof(tampon));
        if (etat == 0)
        {
            return 0;
        }
        if (etat == 1 && estFormeDeChiffres(tampon, nbChiffres) == 1
            && nbChiffres < taille)
        {
            strcpy(dest, tampon);    /* copie de chaine (CM7) */
            return 1;
        }
        printf("  -> Erreur : il faut exactement %d chiffres.\n", nbChiffres);
    }
}

/* --------------------------------------------------------------------------
 * saisirEmail : demande une adresse email de forme valide
 * -------------------------------------------------------------------------- */
int saisirEmail(char *invite, char *dest, int taille)
{
    while (1)
    {
        printf("%s", invite);
        int etat = lireLigne(dest, taille);
        if (etat == 0)
        {
            return 0;
        }
        if (etat == 1 && estEmailValide(dest) == 1)
        {
            return 1;
        }
        printf("  -> Erreur : adresse invalide (elle doit contenir un '@', sans espace).\n");
    }
}

/* --------------------------------------------------------------------------
 * estEmailValide : un seul '@', ni en premiere ni en derniere position,
 * et aucun espace.
 * -------------------------------------------------------------------------- */
int estEmailValide(char *texte)
{
    int longueur = strlen(texte);
    int nbArobases = 0;

    for (int i = 0; i < longueur; i++)
    {
        if (texte[i] == ' ')
        {
            return 0;
        }
        if (texte[i] == '@')
        {
            nbArobases++;
            if (i == 0 || i == longueur - 1)
            {
                return 0;
            }
        }
    }
    return nbArobases == 1;
}

/* --------------------------------------------------------------------------
 * estFormeDeChiffres : 1 si texte contient exactement nb caracteres, tous
 * des chiffres.
 * -------------------------------------------------------------------------- */
int estFormeDeChiffres(char *texte, int nb)
{
    int longueur = strlen(texte);
    if (longueur != nb)
    {
        return 0;
    }
    for (int i = 0; i < longueur; i++)
    {
        if (texte[i] < '0' || texte[i] > '9')
        {
            return 0;
        }
    }
    return 1;
}

/* --------------------------------------------------------------------------
 * minuscule : transforme 'A'..'Z' en 'a'..'z' (en C, un char est un petit
 * entier : la difference entre 'a' et 'A' vaut 32).
 * -------------------------------------------------------------------------- */
char minuscule(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c + ('a' - 'A');
    }
    return c;
}

/* --------------------------------------------------------------------------
 * egalSansCasse : compare deux chaines sans tenir compte des majuscules
 * -------------------------------------------------------------------------- */
int egalSansCasse(char *a, char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (minuscule(a[i]) != minuscule(b[i]))
        {
            return 0;
        }
        i++;
    }
    return a[i] == b[i];     /* vrai si les deux chaines se terminent ensemble */
}

/* --------------------------------------------------------------------------
 * contientMotCle : 1 si motif apparait dans texte (sans tenir compte de la casse)
 * On essaie chaque position de depart possible dans texte.
 * -------------------------------------------------------------------------- */
int contientMotCle(char *texte, char *motif)
{
    int longTexte = strlen(texte);
    int longMotif = strlen(motif);

    if (longMotif == 0)
    {
        return 0;
    }
    for (int debut = 0; debut + longMotif <= longTexte; debut++)
    {
        int j = 0;
        while (j < longMotif && minuscule(texte[debut + j]) == minuscule(motif[j]))
        {
            j++;
        }
        if (j == longMotif)
        {
            return 1;        /* tout le motif a ete retrouve */
        }
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * Affichage
 * -------------------------------------------------------------------------- */
void afficherLigne(int longueur)
{
    for (int i = 0; i < longueur; i++)
    {
        printf("-");
    }
    printf("\n");
}

void afficherTitre(char *titre)
{
    printf("\n");
    afficherLigne(78);
    printf("  %s\n", titre);
    afficherLigne(78);
}

void attendreEntree(void)
{
    char tampon[8];
    printf("\nAppuyez sur Entree pour continuer...");
    lireLigne(tampon, sizeof(tampon));
}
