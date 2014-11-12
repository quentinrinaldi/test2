//gestion_bd.h

#include "outils.h"

typedef struct _assoc_rel_page {
    char id_rel[3];
    int index_page;
} Assoc_rel_page;

typedef struct _annuaire_page {
    Assoc_rel_page ** rel_pages;
    int nb_rel_pages;
} Annuaire_pages;

typedef struct _page {
    char enregistrement[64];
} Page;

typedef struct _donnees_BD {
    Page **pages;
    int nb_pages;
} Donnees_BD;

Donnees_BD *creer_donnees_BD (int taille);
Page *creer_page ();
Assoc_rel_page *creer_assoc_rel_page (char id_rel[3], int index_page);
Annuaire_pages *creer_annuaire_page (int taille);

void decaler_gauche_zone_gestion(Page *page, int adresse, int taille, int dernier_descripteur);
void inserer_donnees (Page *page, int adresse, Buffer *buffer);
void afficher_contenu_page (Page *page);
int limite_zone_de_gestion (int limite_theorique, Page *page);

void nettoyer_espace_vide (Page *page, int limite_zg, int index_descripteur);
void regrouper_espace_vide (Page *page, int limite_zg, int index_descripteur_droite);
void desallocation_si_page_vide (Page *page, Assoc_rel_page *assoc);

void sauvegarder_annuaire_pages (Annuaire_pages *annuaire, char *nom_BD);
Annuaire_pages *charger_annuaire_pages (char nom_BD[32]);

void sauvegarder_donnees_BD (Donnees_BD *donnees, char *nom_BD);
Donnees_BD *charger_donnees_BD (char nom_BD[32]);