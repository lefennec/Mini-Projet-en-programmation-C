/* ==========================================================================
 * initialisation.h - Creation / destruction de la Boutique et jeu de donnees
 * de demonstration charge en memoire (aucune persistance : pas de fichier).
 * ========================================================================== */

#ifndef INITIALISATION_H
#define INITIALISATION_H

#include "types.h"

int  initialiserBoutique(Boutique *b);        /* alloue les tableaux vides : 1 ou 0 */
int  initialiserDonneesTest(Boutique *b);     /* remplit le catalogue et les clients */
void detruireBoutique(Boutique *b);           /* libere TOUTE la memoire             */

#endif /* INITIALISATION_H */
