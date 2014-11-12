//meta_schema.h

#include "outils.h"

// Definition des types


//type d'attributs pris en charge dans notre SGBD 
//typedef enum _type {VARCHAR, INT} Type;

// Structure du meta-domaine d'un attribut
typedef struct _meta_domaine {
// Type *type;  //Type (int ou varchar)
    int type;
    int taille;  // Taille en octets (si int alors 4 octets)
} Meta_domaine;

// Structure du meta-attribut
typedef struct _meta_attribut {
	char id[3]; //Identifiant (nom) de l'attribut
	Meta_domaine *domaine;
    int rang;
} Meta_attribut;

// Structure de la meta-relation
typedef struct _meta_relation {
	char id[3];
	int nb_attr;
	Meta_attribut **attributs;
} Meta_relation;

typedef struct _schema_BD {
	Meta_relation **relations;
	int nb_relations;
    char nom_BD[32];
} Schema_BD;

// Declaration des en-tetes de fonctions

Meta_domaine *creer_meta_domaine (int type, int taille);
Meta_attribut *creer_meta_attribut (char *id, Meta_domaine *domaine, int rang);
Meta_relation *creer_meta_relation (char *id, int nbattr, Meta_attribut **attributs);
Schema_BD *creer_schema_BD (Meta_relation **relations, int nb_relations, char *nom_BD);

void serialiser_meta_domaine (Meta_domaine *domaine, Buffer *buffer);
void serialiser_meta_attribut (Meta_attribut *attribut, Buffer *buffer);
Buffer *serialiser_meta_relation (Meta_relation *relation);
Buffer *creer_buffer (int taille_reelle, int taille_max);


void sauvegarder_schema_BD (Schema_BD *schema);
Schema_BD *charger_schema_BD (char nom_BD[32]);


