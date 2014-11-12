#include "meta_schema.h"
#include "outils.h"
#include "gestion_BD.h"

void ajouter_relation(Meta_relation **relations, int ind_r);
Meta_attribut *ajouter_attribut(int ind_r, int ind_a);

void creation_BD();
Meta_domaine *ajouter_domaine ();
Meta_attribut *ajouter_attribut();
void menu_manipulation_BD();

void ajouter_nuplet ();
void afficher_page();
void supprimer_nuplet ();
void afficher_relation ();
void fin_session ();

void enregistrer_nuplet (Buffer *buffer, Meta_relation *relation);

int proposer_page();
Meta_relation *proposer_relation ();
Meta_attribut *proposer_attribut(Meta_relation *relation);