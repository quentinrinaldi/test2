//gestion_bd.c

#include "gestion_BD.h"

Page *creer_page () {
    Page *page = (Page *) malloc (sizeof (Page) * 1);
    for (int i=0; i<63; i++) {
        page->enregistrement[i] = '\0';
    }
    page->enregistrement[63] = int_to_bytes(0)[3];
    return page;
}

Donnees_BD *creer_donnees_BD (int taille) {
    Donnees_BD *donnees_BD = (Donnees_BD *) malloc (sizeof(Donnees_BD));
    donnees_BD->pages = (Page **) malloc (sizeof(Page *) * taille);
    donnees_BD->nb_pages = taille;
    for (int i = 0; i<taille; i++) {
        donnees_BD->pages[i] = creer_page ();
    }
    return donnees_BD;
}

Assoc_rel_page *creer_assoc_rel_page (char id_rel[3], int index_page) {
    Assoc_rel_page *assoc_rel_page = (Assoc_rel_page *) malloc (sizeof (Assoc_rel_page));
    if (id_rel == NULL) {
        assoc_rel_page->id_rel[0] = '\0';
    }
    else {
        strcpy(assoc_rel_page->id_rel, id_rel);
    }
        assoc_rel_page->index_page = index_page;
    return assoc_rel_page;
}

Annuaire_pages *creer_annuaire_page (int taille)  {
    Annuaire_pages *annuaire_page = (Annuaire_pages *) malloc (sizeof (Annuaire_pages) * 1);
    annuaire_page->nb_rel_pages=taille;
    annuaire_page->rel_pages = (Assoc_rel_page **) malloc (sizeof (Assoc_rel_page *) * taille);
    for (int i=0; i<taille; i++) {
        annuaire_page->rel_pages[i] = creer_assoc_rel_page(NULL, i);
    }
    return annuaire_page;
}

void decaler_gauche_zone_gestion(Page *page, int adresse, int taille, int limite_theorique) {
    int limite_relle = limite_zone_de_gestion(limite_theorique, page);
    
    for (int j = limite_relle; j<=adresse; j=j+2) {
        page->enregistrement[j-1] = page->enregistrement[j+1];
        page->enregistrement[j] = page->enregistrement[j+2];
    }
    page->enregistrement[adresse-1] = '1';
    page->enregistrement[adresse] = int_to_bytes(((int) page->enregistrement[adresse+2]) + taille)[3];
    
}

void inserer_donnees (Page *page, int adresse, Buffer *buffer) {
    memcpy(page->enregistrement + adresse, buffer->donnees, sizeof(char) *buffer->taille);
    page->enregistrement[63] = int_to_bytes (((int) page->enregistrement[63]) + 1)[3];
    
    afficher_contenu_page(page);

}

void afficher_contenu_page (Page *page) {
    printf("\n\n-------- Affichage Page --------\n");
    for (int i=0; i<63; i++) {
        printf("%c|", page->enregistrement[i]);
    }
    printf("%c\n", page->enregistrement[63]);

}

int limite_zone_de_gestion (int limite_theorique, Page *page) {
    int limite_reelle = limite_theorique;
    for (int j = 62; j > limite_theorique; j=j-2) {
        if (page->enregistrement[j-1] == '0') {
            limite_reelle -= 2;
        }
    }
    return limite_reelle;
}

void nettoyer_espace_vide (Page *page, int limite_zg, int index_descripteur) {
    if (index_descripteur - 2 > limite_zg) {
        if (page->enregistrement[index_descripteur - 2] == '0') {
            regrouper_espace_vide(page, limite_zg, index_descripteur);
        }
    }
    if (index_descripteur + 2 <= 61) {
        if (page->enregistrement[index_descripteur + 2] == '0') {
            regrouper_espace_vide(page, limite_zg, index_descripteur + 2);
        }
    }
}

void regrouper_espace_vide (Page *page, int limite_zg, int index_descripteur_droite) {
    for (int i = index_descripteur_droite; i > limite_zg + 2; i=i-2) {
        page->enregistrement[i+1] = page->enregistrement[i-1];
        page->enregistrement[i] = page->enregistrement[i-2];
    }
    
}

void desallocation_si_page_vide (Page *page, Assoc_rel_page *assoc) {
    if (page->enregistrement[63] == '\0') {
        assoc->id_rel[0] = '\0';
        assoc->id_rel[1] = '\0';
        assoc->id_rel[2] = '\0';
    }
}
void sauvegarder_annuaire_pages (Annuaire_pages *annuaire, char *nom_BD) {
    FILE *f;
    char nom_fichier[128];
    strcpy(nom_fichier, "/Users/Quentin/Desktop/");
    strcat(nom_fichier, nom_BD);
    strcat(nom_fichier, "_annuaire.txt");
	if ((f=fopen(nom_fichier,"wb")) == NULL) {
        printf("\nErreur lors de l'écriture du fichier : %s \n",nom_fichier);
        exit(1);
    }
    else {
        for (int i = 0; i<annuaire->nb_rel_pages; i++) {
            fwrite(annuaire->rel_pages[i]->id_rel, sizeof(char) * 3, 1, f);
            fwrite(&(annuaire->rel_pages[i]->index_page), sizeof(int), 1, f);
        }
    }
    
}
Annuaire_pages *charger_annuaire_pages (char nom_BD[32]) {
    FILE *f;
    Annuaire_pages *annuaire = (Annuaire_pages *) malloc(sizeof(Annuaire_pages) * 1);
    annuaire->nb_rel_pages = 0;
    annuaire->rel_pages = (Assoc_rel_page **) malloc(sizeof(Assoc_rel_page *) * 100);
    char nom_fichier[128];
    
    strcpy(nom_fichier, "/Users/Quentin/Desktop/");
    strcat(nom_fichier, nom_BD);
    strcat(nom_fichier, "_annuaire.txt");
    
    if ((f=fopen(nom_fichier,"rb")) == NULL) {
        printf("\nErreur lors de la lecture du fichier : %s \n",nom_fichier);
        return NULL;
    }
    else {
        char id_rel_tmp[3];
        int index_page_tmp;
        
        while (fread(id_rel_tmp,sizeof(char)*3, 1,f) == 1 && fread(&index_page_tmp, sizeof(int), 1, f) == 1) {
            Assoc_rel_page *assoc = creer_assoc_rel_page(id_rel_tmp, index_page_tmp);
            annuaire->rel_pages[annuaire->nb_rel_pages] = assoc;
            annuaire->nb_rel_pages++;
        }
        return annuaire;
    }
}

void sauvegarder_donnees_BD (Donnees_BD *donnees, char *nom_BD) {
    FILE *f;
    char nom_fichier[128];
    strcpy(nom_fichier, "/Users/Quentin/Desktop/");
    strcat(nom_fichier, nom_BD);
    strcat(nom_fichier, "_donnees.txt");
	if ((f=fopen(nom_fichier,"wb")) == NULL) {
        printf("\nErreur lors de l'écriture du fichier : %s \n",nom_fichier);
        exit(1);
    }
    else {
        for (int i = 0; i<donnees->nb_pages; i++) {
            fwrite(donnees->pages[i], sizeof(char) * 64, 1, f);
        }
    }

}
Donnees_BD *charger_donnees_BD (char nom_BD[32]) {
    FILE *f;
    Donnees_BD *donnees = (Donnees_BD *) malloc(sizeof(Donnees_BD) * 1);
    donnees->nb_pages=0;
    donnees->pages = (Page **) malloc(sizeof(Page *) * 100);
    
    char nom_fichier[128];
 
    strcpy(nom_fichier, "/Users/Quentin/Desktop/");
    strcat(nom_fichier, nom_BD);
    strcat(nom_fichier, "_donnees.txt");
    
    if ((f=fopen(nom_fichier,"rb")) == NULL) {
        printf("\nErreur lors de la lecture du fichier : %s \n",nom_fichier);
        return NULL;
    }
    else {
        char page_tmp[64];
        while (fread(&page_tmp,sizeof(char)*64, 1,f) == 1) {
            Page *page = (Page *) malloc(sizeof(Page) * 1);
            memccpy(page->enregistrement, page_tmp, 1, sizeof(char) * 64);
            donnees->pages[donnees->nb_pages] = page;
            donnees->nb_pages++;
        }
    }
    return donnees;
}
