/* ==========================================================================
 * types.h - Structures et constantes partagees par TOUS les modules
 * --------------------------------------------------------------------------
 * Notions du cours utilisees : typedef (CM8), struct (CM8), structures
 * imbriquees = composition (CM8), pointeurs (CM6), #define (CM4),
 * protection contre la double inclusion #ifndef (CM7).
 *
 * Pourquoi un fichier a part ? Client contient des Commande, Commande
 * contient des LigneCommande... Si chaque module redeclarait ses structures,
 * on aurait des inclusions circulaires (client.h <-> commande.h).
 * Tous les modules incluent donc types.h, et c'est tout.
 * ========================================================================== */

#ifndef TYPES_H      /* Si TYPES_H n'est pas encore defini...              */
#define TYPES_H      /* ... on le definit et on lit le fichier (une seule fois) */

/* ----- Constantes de taille (tirees de l'enonce, section 3) ------------- */
#define TAILLE_REFERENCE   12   /* "PRD-0001" + '\0' (11 caracteres utiles)  */
#define TAILLE_DESIGNATION 60
#define TAILLE_CATEGORIE   30
#define TAILLE_RUE         80   /* 79 caracteres maximum                     */
#define TAILLE_VILLE       50
#define TAILLE_CODE_POSTAL 6    /* 5 chiffres + '\0'                         */
#define TAILLE_PAYS        50
#define TAILLE_NOM         50   /* 49 caracteres maximum                     */
#define TAILLE_PRENOM      50
#define TAILLE_EMAIL       80
#define TAILLE_TELEPHONE   15   /* 10 chiffres + '\0'                        */
#define TAILLE_STATUT      20
#define TAILLE_NOM_BOUTIQUE 50

/* ----- Capacite de depart des tableaux dynamiques ----------------------- */
#define CAPACITE_INITIALE_PRODUITS 10
#define CAPACITE_INITIALE_CLIENTS  8

/* ----- Les 4 statuts possibles d'une commande (utilises en partie 2) ---- */
#define STATUT_EN_COURS "EN_COURS"
#define STATUT_EXPEDIEE "EXPEDIEE"
#define STATUT_LIVREE   "LIVREE"
#define STATUT_ANNULEE  "ANNULEE"

/* ==========================================================================
 * 3.1.1 Date  (sera utilisee dans Commande : COMPOSITION)
 * ========================================================================== */
typedef struct
{
    int jour;    /* 1..31 (valide selon le mois)  */
    int mois;    /* 1..12                         */
    int annee;   /* 2000..2100                    */
} Date;

/* ==========================================================================
 * 3.1.2 Adresse  (utilisee dans Client et Commande : COMPOSITION)
 * ========================================================================== */
typedef struct
{
    char rue[TAILLE_RUE];
    char ville[TAILLE_VILLE];
    char codePostal[TAILLE_CODE_POSTAL];
    char pays[TAILLE_PAYS];
} Adresse;

/* ==========================================================================
 * 3.1.3 Produit
 * ========================================================================== */
typedef struct
{
    char  reference[TAILLE_REFERENCE];     /* unique, non vide              */
    char  designation[TAILLE_DESIGNATION];
    char  categorie[TAILLE_CATEGORIE];
    float prixUnitaireHT;                  /* > 0                           */
    int   quantiteStock;                   /* >= 0                          */
    int   seuilAlerte;                     /* >= 0                          */
    int   actif;                           /* 1 = actif, 0 = supprime (logique) */
} Produit;

/* ==========================================================================
 * 3.1.4 LigneCommande  (utilisee en partie 2)
 * ========================================================================== */
typedef struct
{
    char  refProduit[TAILLE_REFERENCE];    /* REFERENCE vers Produit.reference */
    char  designation[TAILLE_DESIGNATION]; /* libelle fige au moment de la vente */
    int   quantite;
    float prixUnitaireHT;                  /* prix fige au moment de la vente */
    float sousTotalHT;                     /* quantite x prixUnitaireHT      */
} LigneCommande;

/* ==========================================================================
 * 3.1.5 Commande  (utilisee en partie 2)
 *  - contient une Date et une Adresse        -> COMPOSITION
 *  - possede un tableau dynamique de lignes   -> AGREGATION
 *  - est rattachee a un Client via idClient   -> ASSOCIATION
 * ========================================================================== */
typedef struct
{
    int            numero;
    int            idClient;
    Date           dateCommande;
    char           statut[TAILLE_STATUT];
    int            nbLignes;            /* taille LOGIQUE du tableau  */
    int            capaciteLignes;      /* taille PHYSIQUE du tableau */
    LigneCommande *lignes;              /* tableau alloue avec malloc */
    float          montantBrutHT;
    float          tauxRemise;
    float          montantRemise;
    float          montantHT;
    float          montantTVA;
    float          montantTTC;
    float          fraisPort;
    Adresse        adresseLivraison;
} Commande;

/* ==========================================================================
 * 3.1.6 Client
 * ========================================================================== */
typedef struct
{
    int       idClient;                 /* unique, attribue par la Boutique */
    char      nom[TAILLE_NOM];
    char      prenom[TAILLE_PRENOM];
    char      email[TAILLE_EMAIL];      /* contient '@', unique             */
    char      telephone[TAILLE_TELEPHONE]; /* 10 chiffres                   */
    Adresse   adresse;                  /* COMPOSITION                      */
    int       pointsFidelite;
    int       nbCommandes;              /* taille logique                   */
    int       capaciteCommandes;        /* taille physique                  */
    Commande *commandes;                /* tableau dynamique (historique)   */
    int       actif;                    /* 1 = actif, 0 = supprime (logique) */
} Client;

/* ==========================================================================
 * 3.1.7 Boutique : structure RACINE, possedee par main().
 * Elle donne acces a TOUT le reste : Boutique -> Client -> Commande -> Ligne
 * (quatre niveaux d'allocation dynamique).
 * ========================================================================== */
typedef struct
{
    char     nom[TAILLE_NOM_BOUTIQUE];
    Produit *produits;                  /* catalogue (tableau dynamique)    */
    int      nbProduits;
    int      capaciteProduits;
    Client  *clients;                   /* repertoire (tableau dynamique)   */
    int      nbClients;
    int      capaciteClients;
    int      numeroSuivant;             /* compteur des numeros de commande */
    int      idClientSuivant;           /* compteur des identifiants client */
} Boutique;

#endif /* TYPES_H */
