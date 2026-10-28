/* ==========================================================================
 * utils.h - Saisie securisee, validation des donnees, affichage
 * ========================================================================== */

#ifndef UTILS_H
#define UTILS_H

/* ----- Lecture d'une ligne au clavier ------------------------------------
 * Retourne :  1 = ligne lue correctement
 *             0 = fin de l'entree (plus rien a lire : Ctrl+D / fin de fichier)
 *            -1 = ligne trop longue pour le tableau (elle est ignoree)      */
int lireLigne(char *dest, int taille);

/* ----- Conversion texte -> nombre (retourne 1 si OK, 0 sinon) ------------
 * Le resultat est renvoye par ADRESSE (passage par adresse, CM7).          */
int convertirEntier(char *texte, int *resultat);
int convertirDecimal(char *texte, float *resultat);

/* ----- Saisies securisees : redemandent tant que la valeur est invalide --
 * Elles retournent 1 si une valeur valide a ete saisie, 0 si l'entree est
 * terminee (dans ce cas on abandonne l'operation en cours).                */
int saisirEntier(char *invite, int min, int max, int *resultat);
int saisirPrix(char *invite, float *resultat);                 /* > 0       */
int saisirTexte(char *invite, char *dest, int taille);         /* non vide  */
int saisirChiffres(char *invite, char *dest, int taille, int nbChiffres);
int saisirEmail(char *invite, char *dest, int taille);

/* ----- Validation --------------------------------------------------------*/
int estEmailValide(char *texte);                 /* contient un '@'         */
int estFormeDeChiffres(char *texte, int nb);     /* exactement nb chiffres  */

/* ----- Outils sur les chaines -------------------------------------------*/
char minuscule(char c);
int  egalSansCasse(char *a, char *b);            /* 1 si egales ("a"=="A")  */
int  contientMotCle(char *texte, char *motif);   /* recherche sans casse    */

/* ----- Affichage ---------------------------------------------------------*/
void afficherTitre(char *titre);
void afficherLigne(int longueur);
void attendreEntree(void);

#endif /* UTILS_H */
