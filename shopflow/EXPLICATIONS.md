# ShopFlow – Partie 1 : explications du code

## 1. Ce que contient la partie 1

| Bloc | Contenu |
|---|---|
| Fondations | `types.h` : les 7 structures de l'énoncé (Date, Adresse, Produit, LigneCommande, Commande, Client, Boutique), déjà complètes pour la partie 2 |
| Catalogue produits | ajouter, mettre à jour, supprimer (logique), afficher avec valorisation du stock, rechercher (référence ou mot-clé), alertes de stock |
| Clients | ajouter, mettre à jour, supprimer (logique), liste (nb de commandes + CA), fiche détaillée |
| Mémoire | tableaux dynamiques `malloc` / `realloc` / `free`, tests de `NULL`, libération complète (valgrind : 0 fuite, 0 erreur) |
| Données de démo | 8 produits (4 catégories, 2 en alerte : Souris et Hub USB-C), 5 clients, chargés en dur dans `initialisation.c` |

**Reste pour la partie 2** : `commande.c/.h` (création, lignes, statuts, annulation, calcul remise/TVA/port/points), `statistique.c/.h` (CA, palmarès, top 5), les 6 commandes de test, les 2 fonctionnalités de créativité.
Les fonctions de la partie 1 sont déjà écrites pour fonctionner avec les commandes (`produitUtilise`, `clientAUneCommandeActive`, `chiffreAffairesClient`, `afficherUnClient` parcourent déjà les commandes ; il n'y en a simplement pas encore).

## 2. Architecture (les fichiers)

```
types.h            structures + constantes (inclus par tous les modules)
utils.h/.c         saisie sécurisée, validation, affichage
produit.h/.c       module Catalogue produits
client.h/.c        module Clients
initialisation.h/.c   création/destruction de la Boutique + jeu de données
main.c             menus + boucle principale
Makefile / CMakeLists.txt   compilation (CLion utilise CMakeLists.txt)
```

Chaque module = un `.h` (l'**interface** : prototypes) + un `.c` (l'**implémentation**), comme dans le CM7. Tous les `.h` commencent par `#ifndef / #define` pour ne pas être lus deux fois.

### Les 4 niveaux d'allocation dynamique

```
Boutique (variable de main)
 ├─ produits  → tableau de Produit          (malloc, realloc si plein)
 └─ clients   → tableau de Client           (malloc, realloc si plein)
      └─ commandes → tableau de Commande    (partie 2)
           └─ lignes → tableau de LigneCommande (partie 2)
```

Chaque tableau a une **taille logique** (`nbProduits`, `nbClients`… cases remplies) et une **taille physique** (`capaciteProduits`… cases réservées) : exactement la distinction du CM5.

## 3. Les points importants à savoir expliquer à l'oral

### a) Pourquoi ne pas utiliser `scanf("%d")` directement ? (`utils.c`)
Si l'utilisateur tape `abc`, `scanf` laisse `abc` dans le clavier et la boucle de saisie tourne à l'infini. Ici on lit **toute la ligne** avec `fgets` (CM5), on enlève le `\n` avec `strcspn` (CM5), puis on **vérifie et convertit le texte nous-mêmes** (`convertirEntier`, `convertirDecimal`) : chaque caractère `'0'..'9'` vaut `caractère - '0'`, et on construit le nombre avec `valeur = valeur * 10 + chiffre`.

Les fonctions `saisirEntier`, `saisirPrix`, `saisirTexte`, `saisirChiffres`, `saisirEmail` redemandent tant que la valeur est invalide, et rendent le résultat par **adresse** (passage par adresse, CM7 : `int *resultat`). Elles retournent 1 (valide) ou 0 (plus rien à lire : on abandonne l'opération).

### b) `realloc` dans une variable temporaire (`produit.c`, `client.c`)
```c
Produit *nouveau = (Produit *) realloc(b->produits, nouvelleCapacite * sizeof(Produit));
if (nouveau == NULL) { ... return 0; }   // l'ancien tableau reste valide
b->produits = nouveau;
```
Si on écrivait `b->produits = realloc(b->produits, ...)` et que `realloc` échouait, on obtiendrait `NULL` et on **perdrait** l'adresse de l'ancien tableau (données perdues + fuite mémoire). La capacité double à chaque agrandissement : peu d'appels à `realloc`.

### c) Suppression logique
On ne retire jamais un élément du tableau : `actif = 0`. Il disparaît des affichages mais sa référence / son email reste réservé (l'unicité est contrôlée sur **tous** les enregistrements, désactivés compris, comme demandé).
Un produit n'est pas supprimable s'il figure dans une commande non annulée ; un client non plus s'il a une commande non annulée.

### d) Identifiants jamais saisis à la main
`insererClient` prend `b->idClientSuivant` puis l'incrémente. Les numéros de commande suivront le même principe avec `numeroSuivant` (partie 2).

### e) Libération mémoire (`detruireBoutique`)
Ordre **enfants avant parents** : lignes → commandes → clients → produits. Si on libérait `b->clients` en premier, on perdrait les adresses des tableaux de commandes (fuite). Après `free`, on remet les pointeurs à `NULL` (conseil du CM6).

### f) Passage par valeur / par adresse
- `insererProduit(Boutique *b, Produit p)` : `b` par **adresse** (on modifie la boutique), `p` par **valeur** (une copie est rangée dans le tableau).
- `afficherLigneProduit(Produit *p)` : par adresse pour éviter de copier toute la structure ; on accède aux champs avec `->` (CM8).

### g) Composition
`Client` contient une `Adresse` : on écrit `c->adresse.ville` (flèche pour le pointeur `c`, point pour la structure imbriquée, CM8).

## 4. Notions du cours utilisées (rien d'autre)

| Cours | Notions |
|---|---|
| CM4 | `#define`, `printf`, `if/else`, `switch`, `for`, `while`, `break`, `continue`, types `int/float/double/char` |
| CM5 | tableaux, tailles logique/physique, chaînes `char[]`, `fgets`, `strcspn`, parcours de tableaux |
| CM6 | pointeurs, `&` / `*`, `malloc`, `realloc`, `free`, `NULL`, `sizeof` |
| CM7 | fonctions, passage par valeur/adresse, `<string.h>` (`strlen`, `strcpy`, `strcat`, `strcmp`), modules `.h/.c`, `#ifndef` |
| CM8 | `typedef`, `struct`, structures imbriquées, tableaux de structures, `->` |

## 5. Choix à connaître (non précisés par l'énoncé)

- **CA d'un client = chiffre d'affaires HT** des commandes non annulées (somme des `montantHT`).
- **Références produit** et **emails** comparés sans tenir compte de la casse (`prd-0001` = `PRD-0001`).
- Les textes affichés n'ont **pas d'accents** (évite les caractères illisibles dans certaines consoles Windows) ; les commentaires du code, eux, en ont.
- Le numéro de commande de départ est **1001** (comme dans l'exemple de l'énoncé).
- Stock et seuil plafonnés à 1 000 000 pour éviter les saisies absurdes.

## 6. Compiler et tester

- **CLion** : ouvrir le dossier `shopflow` (il détecte `CMakeLists.txt`), puis ▶ Run.
- **Terminal** : `make` puis `./shopflow` (`make clean` pour nettoyer).
- **Fuites mémoire (Linux/WSL)** : `valgrind --leak-check=full ./shopflow`
  Résultat obtenu : `All heap blocks were freed -- no leaks are possible`, `0 errors`, y compris après ajout de 12 produits et 7 clients (qui déclenchent bien les `realloc`).
- Compilation avec `-Wall -Wextra` : **0 avertissement**.

## 7. Tests rapides à faire avant la soutenance

1. Afficher le catalogue : 8 produits, 2 lignes `ALERTE`.
2. Ajouter un produit avec une référence existante → refusé ; avec un prix `abc` ou `-5` → redemandé.
3. Rechercher `souris` (mot-clé, sans casse) et `PRD-0003` (référence).
4. Supprimer un produit puis afficher le catalogue : il a disparu ; ré-ajouter la même référence → refusé.
5. Ajouter un client avec un email déjà pris, un téléphone de 9 chiffres, un code postal `44A00` → tout est refusé.
6. Afficher la fiche d'un client : adresse complète, « Aucune commande » (jusqu'à la partie 2).
7. Ajouter plus de 10 produits (ou plus de 8 clients) pour montrer l'agrandissement automatique du tableau.
