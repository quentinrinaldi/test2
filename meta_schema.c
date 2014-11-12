//meta_schema.c

#include "meta_schema.h"

//--------------------------- CREATION DES INSTANCES ---------------------------\\

Meta_domaine *creer_meta_domaine (int type, int taille) {
    Meta_domaine *domaine = (Meta_domaine *) malloc(sizeof(Meta_domaine) * 1);
    domaine->taille=taille;
    domaine->type=type;
    
    return domaine;
}
Meta_attribut *creer_meta_attribut (char *id, Meta_domaine *domaine, int rang) {
    Meta_attribut *attribut = (Meta_attribut *) malloc(sizeof(Meta_attribut) * 1);
    attribut->domaine = domaine;
    strcpy(attribut->id, id);
    attribut->rang = rang;
    
    return attribut;
}
Meta_relation *creer_meta_relation (char *id, int nbattr, Meta_attribut **attributs) {
    Meta_relation *relation = (Meta_relation *) malloc (sizeof(Meta_relation) * 1);
    relation->attributs=attributs;
    strcpy(relation->id, id);
    relation->nb_attr = nbattr;
    
    return relation;
}

Schema_BD *creer_schema_BD (Meta_relation **relations, int nb_relations, char *nom_BD) {
    Schema_BD *schema = (Schema_BD *) malloc(sizeof(Schema_BD));
    schema->nb_relations=nb_relations;
    schema->relations=relations;
    strcpy(schema->nom_BD, nom_BD);
    
    return schema;
}


//------------------ SERIALISATION ET SAUVEGARDE DES DONNEES ----------------- \\

void serialiser_meta_domaine (Meta_domaine *domaine, Buffer *buffer) {

    if (domaine->type == 0) {
        (buffer->donnees)[buffer->ind]='I';
        (buffer->donnees)[buffer->ind + 1]='4';
    }
    else {
        (buffer->donnees)[buffer->ind]='V';
        char t = (char)(((int)'0')+domaine->taille);
        (buffer->donnees)[buffer->ind + 1] = t;
    }
    buffer->ind += 2;
}

void serialiser_meta_attribut (Meta_attribut *attribut, Buffer *buffer) {
    strcpy((buffer->donnees) + buffer->ind, attribut->id);
    buffer->ind+=2;
    serialiser_meta_domaine(attribut->domaine, buffer);
}

Buffer *serialiser_meta_relation (Meta_relation *relation) {
    Buffer *buffer = creer_buffer((relation->nb_attr * 4) + 3, (relation->nb_attr * 4) + 3);
    strcpy(buffer->donnees, relation->id);
    
    char c = (char)(((int)'0')+relation->nb_attr);
    printf("%c", c);
    strcpy(buffer->donnees + 2, &c);
    buffer->ind += 3;
    
    for (int i = 0; i< relation->nb_attr; i++) {
        serialiser_meta_attribut(relation->attributs[i], buffer);
    }
    return buffer;
}
void sauvegarder_schema_BD (Schema_BD *schema) {
    Buffer *buffer;
    FILE *f;
    char nom_fichier[128];
    strcpy(nom_fichier, "/Users/Quentin/Desktop/");
    strcat(nom_fichier, schema->nom_BD);
    strcat(nom_fichier, "_schema.txt");
	if ((f=fopen(nom_fichier,"wb")) == NULL) {
        printf("\nErreur lors de l'écriture du fichier : %s \n",nom_fichier);
        exit(1);
    }
    else {
        for (int i = 0; i<schema->nb_relations; i++) {
            buffer = serialiser_meta_relation(schema->relations[i]);
            fwrite(buffer->donnees, sizeof(char) * buffer->taille, 1, f);
        }
    }
}

// ----------------------- CHARGEMENT DES DONNEES ------------------------\\

Schema_BD *charger_schema_BD (char nom_BD[32]) {
    FILE *f;
    Schema_BD *schema;
    char nom_fichier[128];
    int cpt_rel = 0;
    strcpy(nom_fichier, "/Users/Quentin/Desktop/");
    strcat(nom_fichier, nom_BD);
    strcat(nom_fichier, "_schema.txt");
    
    if ((f=fopen(nom_fichier,"rb")) == NULL) {
        printf("\nErreur lors de la lecture du fichier : %s \n",nom_fichier);
        return NULL;
    }
    else {
        Meta_relation ** relations = (Meta_relation **) malloc(sizeof(Meta_relation *) * 9);
        char id_rel[3];
        
        
        while (fread(&id_rel,sizeof(char)*2, 1,f) == 1) {
            
            id_rel[2] = '\0';
            char tmp_nb_attr;
            char nom_att[3];
            nom_att[2]='\0';
            
            char tmp_type_domaine;
            char tmp_taille_domaine;
            
            int type_domaine;
            int taille_domaine;
            
            fread(&tmp_nb_attr,sizeof(char), 1,f);
            int nb_attr = char_to_int(tmp_nb_attr);
            
            Meta_attribut ** attributs = (Meta_attribut **) malloc(sizeof(Meta_attribut *) * nb_attr);
            
            // meta relation
            
            for (int i=0; i<nb_attr ; i++) {
                fread(&nom_att,sizeof(char) * 2, 1,f);
                fread(&tmp_type_domaine,sizeof(char), 1,f);
                fread(&tmp_taille_domaine,sizeof(char), 1,f);
                
                type_domaine = (tmp_type_domaine == 'I')? 0 : 1;
                taille_domaine = char_to_int(tmp_taille_domaine);
                
                attributs[i] = creer_meta_attribut(nom_att, creer_meta_domaine(type_domaine, taille_domaine), i);
            }
            
            relations[cpt_rel] = creer_meta_relation(id_rel, nb_attr, attributs);
            cpt_rel++;
        }
        schema = creer_schema_BD(relations, cpt_rel, nom_BD);
        return schema;
    }
}



