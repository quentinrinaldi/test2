//
//  main.c
//  test2
//
//  Created by Quentin Rinaldi on 11/10/2014.
//  Copyright (c) 2014 Quentin Rinaldi. All rights reserved.
//

#include "main.h"
#include <stdio.h>

// ----------------------------- Déclaration des variables globales -----------------------------\\

int choix_utilisateur = 0;      //  Entrée de l'utilisateur en console (Chiffre correspondant à une option)
char saisie_utilisateur[1];     //  Entrée de l'utilisateur en console (lettre correspondant à une option (o=oui/n=non)
char nom_BD[32];                //  Nom de la base de données

int chargement = 0;

// Instances de notre base de données
Schema_BD *schema_BD;       //Schema de la base de données (métadonnées)
Annuaire_pages *annuaire;    //Annuaire indiquant l'adresse des pages de nuplet associées aux relations
Donnees_BD *donnees_BD;

void demarrage () {
    annuaire = creer_annuaire_page(100);
    donnees_BD = creer_donnees_BD(100);
	printf("\nQue voulez-vous faire ?\n1- Charger une base de données Existante\n2- Creer une nouvelle base de données\n3- Quitter le programme\n Votre choix :");
	scanf("%d", &choix_utilisateur);
	switch (choix_utilisateur) {
		case 1: //Charger une base de données existante
            
			printf("\nIndiquez le nom de la base de données à charger : ");
			scanf("%s", nom_BD);
            schema_BD = charger_schema_BD(nom_BD);
            donnees_BD = charger_donnees_BD(nom_BD);
            annuaire = charger_annuaire_pages(nom_BD);
            
            // Vérification du chargement
			if (schema_BD && donnees_BD && annuaire) {
                printf("\nLe chargement de la base a bien été effectué.\n\n");
				menu_manipulation_BD();
			}
			else {
                printf("\nErreur lors du chargement de la base.");
				demarrage();
			}
			break;
            
        case 2: // Création d'une nouvelle base  de données
            creation_BD();
            break;
            
        case 3:
            break;
        default:
            //Le choix de l'utilisateur n'a pas ete compris. On repose la question
            demarrage();
            break;
    }
}

//---------------------------------------------------- SCHEMA ----------------------------------------------------\\

void creation_BD () {
    Meta_relation **meta_relations;
    schema_BD = (Schema_BD *) malloc(sizeof(Schema_BD));
    meta_relations = (Meta_relation **) malloc (sizeof (Meta_relation) * 8);
    
    int cpt_r = 0;
    
    
    printf("\nEntrez le nom de la base de données : ");
    scanf("%s", nom_BD);
    
    //Création d'une relation
    while (cpt_r<8) {
        
        ajouter_relation(meta_relations, cpt_r);
        cpt_r++;
        
        printf("\nVoulez-vous ajouter une relation supplémentaire ? o/n");
        scanf("%s", saisie_utilisateur);
        if (strcmp (saisie_utilisateur, "o") != 0)
            break;
    }
    strcpy(schema_BD->nom_BD, nom_BD);
    schema_BD->nb_relations=cpt_r;
    schema_BD->relations = meta_relations;
    
    sauvegarder_schema_BD(schema_BD);
    printf("\nLa base de données a été enregistrée en mémoire persistante et chargée en mémoire vive\n\n");
    menu_manipulation_BD();
}

void ajouter_relation (Meta_relation **meta_relations, int ind_r) {
    char id_relation[3];
    Meta_attribut **attributs = (Meta_attribut **) malloc(sizeof(Meta_attribut *) * 8);
    
    int cpt_att = 0;
    
    printf("\nIndiquez le nom de la relation à ajouter : ");
    scanf("%s", id_relation);
    id_relation[2]='\0';
    
    //Création d'un attribut
    while (cpt_att<8) {
        
        attributs[cpt_att] = ajouter_attribut(ind_r, cpt_att);
        cpt_att++;
        
        printf("\nVoulez-vous ajouter un attribut supplémentaire ? o/n");
        scanf("%s", saisie_utilisateur);
        if (strcmp (saisie_utilisateur, "o") != 0)
            break;
    }
    
    meta_relations[ind_r] = creer_meta_relation(id_relation, cpt_att, attributs);
}

Meta_attribut *ajouter_attribut(int ind_r, int ind_a) {
    char id_attribut[3];
    
    printf("\nIndiquez le nom de l'attribut à ajouter : ");
    scanf("%s", id_attribut);
    id_attribut[2]='\0';
    
    Meta_domaine *domaine = ajouter_domaine();
    return creer_meta_attribut(id_attribut, domaine, ind_a);
}

Meta_domaine *ajouter_domaine () {
    int taille = 0;
    printf("\nDe quel type sera votre attribut ?\n1- Entier\n2- Chaine de caractères de longueur variable\nVotre choix : ");
    scanf("%d", &choix_utilisateur);
    switch (choix_utilisateur) {
        case 1:
            return creer_meta_domaine(0, 4);
            break;
        case 2:
            printf("\nIndiquez la longueur maximale de la chaine : ");
            scanf("%d", &taille);
            return creer_meta_domaine(1, taille);
            break;
        default:
            return ajouter_domaine();
            break;
    }
}

//------------------------------------------------ MANIPULATION BD ----------------------------------------------\\

void menu_manipulation_BD() {
    int fin_de_session = 0;
    
    while (!fin_de_session) {
        printf("\n\n----------- Manipulation de la base ------------\n\n1- Insérer un n-uplet\n2- Supprimer un n-uplet\n3- Afficher le contenu exact d'une page (en ASCII)\n4- Afficher l'instance d'une relation\n5- FIN de session de travail.\nVotre choix: ");
        scanf("%d", &choix_utilisateur);
        switch (choix_utilisateur) {
            case 1:
                ajouter_nuplet ();
                break;
            case 2:
                supprimer_nuplet ();
                break;
            case 3:
                afficher_page();
                break;
            case 4:
                afficher_relation();
                break;
            case 5:
                fin_session();
                fin_de_session = 1;
                break;
            default:
                printf("\n\n Choix non reconnu.\n\n");
                break;
        }
    }
}

void ajouter_nuplet () {
    printf("\nÀ quelle relation voulez-vous ajouter un n-uplet?\n");
    Meta_relation *relation = proposer_relation();
    Buffer *buffer = creer_buffer(0, 64);
    for (int ind_att = 0; ind_att<relation->nb_attr; ind_att++) {
        Meta_attribut *attribut = relation->attributs[ind_att];
        
        printf("saisissez la valeur de l'attribut %s ", attribut->id);
        if (attribut->domaine->type == 0) {
            printf("de type INT\n");
            printf ("Votre saisie : ");
            scanf("%d", &choix_utilisateur);
            
            // Copie des octets dans le buffer
            unsigned char * octets = int_to_bytes(choix_utilisateur);
            memcpy(buffer->donnees+buffer->taille, octets, sizeof(unsigned char) * 4);
            
            // Mise à jour de la taille du buffer
            buffer->taille += 4;
            afficher_buffer(buffer);
        }
        else {
            printf("de type VARCHAR (%d)\n", attribut->domaine->taille);
            char *varchar = (char *) malloc (sizeof(attribut->domaine->taille));
            printf("Votre saisie : ");
            scanf("%s", varchar);
            unsigned long taille_varchar = strlen(varchar);
            
            //  AJOUT DE LA TAILLE REELLE DU VARCHAR
            buffer->donnees[buffer->taille] = int_to_char((int) taille_varchar);
            buffer->taille++;
            
            //  AJOUT DU VARCHAR
            strcpy(buffer->donnees + buffer->taille, varchar);
            buffer->taille+= taille_varchar;
            afficher_buffer(buffer);
        }
    }
    // ENREGISTREMENT DU NUPLET EN MEMOIRE VIVE
    enregistrer_nuplet(buffer, relation);
}

void supprimer_nuplet () {
    Meta_relation *relation = proposer_relation();
    printf("\nSur quel attribut voulez-vous rechercher le n-uplet à supprimer ? ");
    Meta_attribut *attribut = proposer_attribut(relation);
    Meta_attribut ** attributs = relation->attributs;
    
    int cpt_suppr = 0;
    char critere_suppr[9];
    
    printf("\nVeuillez saisir votre critère de suppression (type ");
    if (attribut->domaine->type == 0) {
        printf("INT): ");
    }
    else {
        printf("VARCHAR(%d): ", attribut->domaine->taille);
    }
    scanf("%s", critere_suppr);
    
    Assoc_rel_page *assoc;
    for (int cpt=0; cpt<annuaire->nb_rel_pages; cpt++) {    // PARCOURS DES PAGES
        
        if ((assoc = annuaire->rel_pages[cpt])->id_rel[0] != '\0') {    //SELECTION DES PAGES NON VIDES
            Page *page = donnees_BD->pages[assoc->index_page];
            
            if (strcmp(assoc->id_rel, relation->id) == 0) {
                
                int nb_nuplet = (int) page->enregistrement[63];
                int limite_zg = limite_zone_de_gestion(62-2*(nb_nuplet), page);
                int debut_attribut = 0;
                
                for (int i = 62; i > limite_zg; i=i-2) {        // PARCOURS DE LA ZONE DE GESTION
                    if (page->enregistrement[i-1] == '1') {   // SELECTION DES NUPLETS NON EFFACES
                        if (i != 62) {
                            debut_attribut = (int) page->enregistrement[i+2];
                        }
                        for (int ind_att = 0; ind_att<relation->nb_attr; ind_att++) {   //  PARCOURS DE CHAQUE ATTRIBUT
                            if (ind_att == attribut->rang) {    // SELECTION DE L'ATTRIBUT A RECHERCHER
                                
                                
                                int comparaison = 0; // BOOLEAN RESULTANT DE LA COMPARAISON
                                
                                // ATTRIBUT DE TYPE INT
                                if (attributs[ind_att]->domaine->type == 0) {
                                    unsigned char octets[4];
                                    memcpy(octets, page->enregistrement + debut_attribut, sizeof(char) * 4);
                                    int res = bytes_to_int(octets);
                                    int critere_de_recherche = (int) strtol(critere_suppr, NULL, 10);
                                    
                                    // COMPARAISON SELON LE CRITERE DE RECHERCHE (POUR UN INT)
                                    if (res == critere_de_recherche)
                                        comparaison = 1;
                                }
                                // ATTRIBUT DE TYPE VARCHAR
                                else {
                                    int taille_varchar = char_to_int (page->enregistrement[debut_attribut]);
                                    char *varchar = (char *) malloc(sizeof(char) * taille_varchar);
                                    memcpy(varchar, page->enregistrement + debut_attribut + 1, sizeof(char) * taille_varchar);
                                    
                                    // COMPARAISON
                                    if (strcmp(critere_suppr, varchar) == 0)
                                        comparaison = 1;
                                }
                                
                                // SUPRESSION DU NUPLET SI COMPARAISON POSITIVE
                                if (comparaison == 1) {
                                    page->enregistrement[i-1]='0';
                                    page->enregistrement[63] = int_to_bytes (((int) page->enregistrement[63]) - 1)[3];
                                    cpt_suppr++;
                                    nettoyer_espace_vide (page, limite_zg, i-1);
                                }
                            }
                            // PASSAGE A L'ATTRIBUT SUIVANT
                            if (attributs[ind_att]->domaine->type == 0) {
                                debut_attribut += 4;
                            }
                            else {
                                int taille_varchar = char_to_int(page->enregistrement[debut_attribut]);
                                debut_attribut = debut_attribut + taille_varchar + 1;
                            }
                            
                        }
                    }
                }
                desallocation_si_page_vide(page, assoc);
            }
        }
    }
}



void afficher_relation () {
    printf("\nQuelle relation voulez-vous afficher ?\n");
    Meta_relation *relation = proposer_relation();
    Meta_attribut ** attributs = relation->attributs;
    printf("\n");
    for (int ind_att = 0; ind_att<relation->nb_attr; ind_att++) {   //  PARCOURS DE CHAQUE ATTRIBUT
        printf("%s\t\t", attributs[ind_att]->id);
    }
    printf("\n\n");
    Assoc_rel_page *assoc;
    for (int cpt=0; cpt<annuaire->nb_rel_pages; cpt++) {    // PARCOURS DES PAGES
        
        if ((assoc = annuaire->rel_pages[cpt])->id_rel[0] != '\0') {    //SELECTION DES PAGES NON VIDES
            if (strcmp(assoc->id_rel, relation->id) == 0) {
                int debut_attribut = 0;
                Page *page = donnees_BD->pages[assoc->index_page];
                
                int nb_nuplet = (int) page->enregistrement[63];
                int limite_zg = limite_zone_de_gestion(62-2*(nb_nuplet), page);
                
                for (int i = 62; i > limite_zg; i=i-2) {    // PARCOURS DE LA ZONE DE GESTION
                    
                    if (page->enregistrement[i-1] == '1') { // SELECTION DES NUPLETS NON EFFACEES
                        if (i != 62) {
                            debut_attribut = (int) page->enregistrement[i+2];
                        }
                        
                        // PARCOURS DES ATTRIBUTS DE CHAQUE NUPLET
                        for (int j=0; j<relation->nb_attr; j++) {
                            
                            // DOMAINE = INT
                            if (relation->attributs[j]->domaine->type == 0) {
                                unsigned char octets[4];
                                memcpy(octets, page->enregistrement + debut_attribut, sizeof(char) * 4);
                                int n = bytes_to_int(octets);
                                printf("%d", n);
                                printf("\t\t");
                                debut_attribut+=4;
                                
                            }
                            // DOMAINE = VARCHAR
                            else {
                                int taille_varchar = (int) page->enregistrement[debut_attribut];
                                debut_attribut++;
                                char *varchar = (char *) malloc(sizeof(char) * taille_varchar);
                                memcpy(varchar, page->enregistrement + debut_attribut, sizeof(char) * taille_varchar);
                                printf("%s", varchar);
                                printf("\t\t");
                                debut_attribut+=taille_varchar;
                            }
                        }
                        printf("\n");
                    }
                }
                
            }
        }
    }
}
void fin_session () {
    sauvegarder_annuaire_pages(annuaire, nom_BD);
    sauvegarder_donnees_BD(donnees_BD, nom_BD);
}

void enregistrer_nuplet (Buffer *buffer, Meta_relation *relation) {
    int ajout = 0;
    int index = -1;
    Assoc_rel_page *assoc = NULL;
    for (int cpt=0; cpt<annuaire->nb_rel_pages; cpt++) {
        
        if ((assoc = annuaire->rel_pages[cpt])->id_rel[0] != '\0') {
            if (strcmp(assoc->id_rel, relation->id) == 0) {
                Page *page = donnees_BD->pages[assoc->index_page];
                int nb_nuplet = (int) page->enregistrement[63];
                
                // RECHERCHE DU DERNIER DESCRIPTEUR
                int limite_zg = limite_zone_de_gestion(62-2*(nb_nuplet), page);
                
                for (int i = 62; i > limite_zg; i=i-2) {
                    
                    // PLACE EN PLEIN MILLIEU
                    if (page->enregistrement[i-1] == '0') {
                        
                        //VERIFICATION TAILLE LIBRE
                        int taille_libre = ((int) (page->enregistrement[i])) - ((int)(page->enregistrement[i+2]));
                        
                        //TAILLE LIBRE = TAILLE A INSERER
                        if (taille_libre == buffer->taille) {
                            page->enregistrement[i-1] = '1';
                            inserer_donnees(page, ((int) page->enregistrement[i+2]), buffer);
                            ajout = 1;
                            break;
                        }
                        
                        //TAILLE LIBRE > TAILLE A INSERER
                        else if (taille_libre > buffer->taille) {
                            
                            //VERIFICATION TAILLE DISPONIBLE POUR ZONE DE GESTION
                            if (((int) page->enregistrement[limite_zg+2]) < limite_zg) {
                                decaler_gauche_zone_gestion(page, i, buffer->taille, limite_zg);
                                inserer_donnees(page, ((int) page->enregistrement[i+2]), buffer);
                                ajout = 1;
                                break;
                            }
                        }
                    }
                }
                
                // NUPLET NON AJOUTE AU MILIEU DE LA PAGE
                if (ajout == 0) {
                    
                    // AJOUT A LA SUITE DE LA PAGE
                    int prochaine_position = ((int) page->enregistrement[limite_zg + 2]) + buffer->taille;
                    
                    if (prochaine_position + 2 <= limite_zg) {
                        page->enregistrement[limite_zg - 1] = '1';
                        page->enregistrement[limite_zg] = int_to_bytes(prochaine_position)[3];
                        inserer_donnees(page, ((int) page->enregistrement[limite_zg+2]), buffer);
                        ajout=1;
                        break;
                    }
                }
                else {
                    // ON SORT DE LA RECHERCHE
                    break;
                }
            }
        }
        else {
            if (index == -1) {
                index = assoc->index_page;
            }
        }
    }
    if (ajout == 0) {
        //INSERTION DANS UNE NOUVELLE PAGE
        assoc=annuaire->rel_pages[index];
        strcpy (annuaire->rel_pages[index]->id_rel, relation->id);
        annuaire->rel_pages[index]->index_page=index;
        Page *page = donnees_BD->pages[index];
        page->enregistrement[61] = '1';
        page->enregistrement[62] = int_to_bytes(buffer->taille)[3];
        inserer_donnees(page, 0, buffer);
        
    }
    printf("\nLe nuplet a été enregistré dans la page n°%d\n", assoc->index_page);
}

void afficher_page () {
    printf("\nSelectionnez une page à afficher : \n");
    int index_page = proposer_page();
    if (annuaire->rel_pages[index_page]->id_rel[0] == '\0') {
        printf("La page indiquée ne contient aucune donnée\n");
    }
    else {
        Page *page = donnees_BD->pages[index_page];
        afficher_contenu_page(page);
        printf("Délimiteur : |");
    }
}

int proposer_page () {
    Assoc_rel_page *assoc;
    printf("\n");
    int page_flag = 0;
    for (int cpt=0; cpt<annuaire->nb_rel_pages; cpt++) {
        if ((assoc = annuaire->rel_pages[cpt])->id_rel[0] != '\0') {
            printf("Page n°%d (%s)\n", assoc->index_page, assoc->id_rel);
            page_flag=1;
        }
    }
    if (page_flag == 0) {
        printf("\n Attention: toutes les pages sont actuellement vides\n");
    }
    printf("\nVotre choix: ");
    scanf("%d", &choix_utilisateur);
    return choix_utilisateur;
}

Meta_relation *proposer_relation () {
    Meta_relation ** relations = schema_BD->relations;
    for (int ind_rel = 0; ind_rel< schema_BD->nb_relations; ind_rel++) {
        printf("%d- %s\n", ind_rel, relations[ind_rel]->id);
    }
    printf("Votre choix :");
    scanf("%d", &choix_utilisateur);
    return relations[choix_utilisateur];
}

Meta_attribut *proposer_attribut (Meta_relation *relation) {
    
    Meta_attribut ** attributs = relation->attributs;
    printf("\n");
    for (int ind_att = 0; ind_att<relation->nb_attr; ind_att++) {
        printf("%d- %s\n", ind_att, attributs[ind_att]->id);
    }
    printf("Votre choix :");
    scanf("%d", &choix_utilisateur);
    return attributs[choix_utilisateur];
}

int main(int argc, const char * argv[])
{
    demarrage();
    
    /* char c = int_to_bytes(13)[3];
     int d = (int) c;
     printf("%.2x %d",c, d);*/
    
    return 0;
}



