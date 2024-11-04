#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//A chaque sommet on associe un identifiant entier
typedef struct {
  int id;
} Sommet;

// une cellule d'une liste contient un sommet et un pointeur vers la cellule suivante 
typedef struct noeud {
    Sommet s;
    struct noeud *suivant;
} Noeud;

//une liste est un pointeur sur l'element de tete
typedef Noeud *Liste;

// graphe caracterise par
typedef struct {
    bool oriente; // true si le graphe est oriente, false sinon
    int n; // nombre de sommets
    int m; // nombre d'aretes (arcs)
    Liste *listes; // tableau de n listes
} Graphe;

void init_graphe(Graphe *g, int n, bool oriente) {
    g->oriente = oriente;
    g->n = n;
    g->m = 0;
    g->listes = (Liste *)malloc(n * sizeof(Liste)); // allocation dynamique
    for (int i = 0; i < n; i++) { // initialisation des elements
        g->listes[i] = NULL;
    }
}

// fonction qui verifie si un lien existe entre deux sommets
bool lien_existe(Graphe *g, int id1, int id2) {
    Noeud *current = g->listes[id1]; // id1 = index
    while (current != NULL) {
        if (current->s.id == id2) { // trouve lien
            return true;
        }
        current = current->suivant;
    }
    return false;
}

// fonction pour ajouter un arc (pour les graphes orientes)
void ajouterArc(Graphe *g, int id1, int id2) {
    // ajouter l'arc (i, j) ou ajoute l'arete (j, i)
    Noeud *n = (Noeud *)malloc(sizeof(Noeud)); // allocation dynamique pour noeud
    n->s.id = id2; // set sommet pour noeud
    n->suivant = g->listes[id1]; // ajouter debut de liste sommet liés
    g->listes[id1] = n; // changer le debut de liste
    g->m++; // incrementer le nombre d'aretes
}

// fonction pour ajouter une arete (pour les graphes non-orientes)
void ajouterArete(Graphe *g, int id1, int id2) {
    // ajouter l'arete (i, j)
    Noeud *n = (Noeud *)malloc(sizeof(Noeud)); // allocation dynamique
    n->s.id = id2; // set sommet = id2
    n->suivant = g->listes[id1]; 
    g->listes[id1] = n; 
    g->m++;
    if(id1 != id2) { // pas boucle
        ajouterArc(g, id2, id1); // on ajoute l'arete (j, i)
        g->m--; // decrementer car on a ajoute deux fois
    }
}

void ajout_lien(Graphe *g, int id1, int id2) {
    if (!lien_existe(g, id1, id2)) { // si le lien n'existe pas
        if (g->oriente) {
            ajouterArc(g, id1, id2);
        } else {
            ajouterArete(g, id1, id2);
        }
    }
    else{
        printf("Le lien existe deja\n");
    }
}

void supp_lien(Graphe *g, int id1, int id2) {
    int trouve = 0;
    Noeud *current = g->listes[id1]; // acceder liste de sommets liés de id1
    Noeud *prev = NULL; // pas de ->prec => definir *prev
    while (current != NULL) { 
        if (current->s.id == id2) { // trouve lien
            trouve = 1;
            if (prev == NULL) { // debut de liste
                g->listes[id1] = current->suivant;
            } else { // milieu ou fin de liste
                prev->suivant = current->suivant;
            }
            free(current); // liberer memoire
            g->m--; // decrementer le nombre d'aretes
            break;
        }
        prev = current;
        current = current->suivant;
    }
    if(trouve){
        // si graphe non oritente -> supprimer l'arete (j, i)
        if(!g->oriente){
            current = g->listes[id2];
            prev = NULL;
            while (current != NULL) {
                if (current->s.id == id1) {
                    if (prev == NULL) {
                        g->listes[id2] = current->suivant;
                    } else {
                        prev->suivant = current->suivant;
                    }
                    free(current);
                    break;
                }
                prev = current;
                current = current->suivant;
            }
        }
        printf("Suppression a ete faite\n");
    }
    else{
        printf("Le lien n'existe pas\n");
    }
}

void ajout_sommet(Graphe *g, int id){
    if(id< g->n-1){ // car id = index
        printf("Le sommet existe deja\n");
        return;
    }
    g->n++;
    g->listes = (Liste *)realloc(g->listes, g->n * sizeof(Liste)); 
    // utiliser realloc pour ne pas perdre les donnees existantes
    g->listes[id] = NULL; // initialiser la nouvelle liste
    Noeud *n = (Noeud *)malloc(sizeof(Noeud)); 
    n->s.id = id;
    n->suivant = NULL; 
}

void afficher_simple(Graphe *g){
    printf("Nombre de sommets: %d\n", g->n);
    printf("Nombre de connexions: %d\n", g->m);
    printf("Type: %s\n", g->oriente ? "Oriente" : "Non oriente");
    printf("Listes d'adjacence:\n"); // là liste những đỉnh nối với i
    for (int i = 0; i < g->n; i++) {
        if (g->listes[i] != NULL) {
            printf("Sommet %d: ", i);
            Noeud *current = g->listes[i];
            while (current != NULL) {
                printf("%d -> ", current->s.id);
                current = current->suivant;
            }
            printf("Null\n");
        }
    }
}

bool est_adjacent_direct(Graphe *g, int id1, int id2){ // pas sur avec version non oriente
    return lien_existe(g, id1, id2);
}

int longueur_liste(Liste l) {
    int longueur = 0;
    Noeud *current = l;
    while (current != NULL) {
        longueur++;
        current = current->suivant;
    }
    return longueur;
}

void degres(Graphe *g, int *de, int *dg){
    for(int i = 0; i < g->n; i++){ // pour chaque sommet
        dg[i] = longueur_liste(g->listes[i]); // degre sortant
        de[i] = 0;
    }
    if (g->oriente) {
        for (int i = 0; i < g->n; i++) {
            Noeud *current = g->listes[i];
            while (current != NULL) {
                de[current->s.id]++;
                current = current->suivant;
            }
        }
    } else {
        for (int i = 0; i < g->n; i++) {
            de[i] = dg[i]; // pour un graphe non orienté, degré entrant = degré sortant
        }
    }
}

void liberer_graphe(Graphe *g){
    for(int i = 0; i < g->n; i++){
        Noeud *current = g->listes[i];
        while (current != NULL) {
            Noeud *temp = current;
            current = current->suivant;
            free(temp); // liberer chaque noeud
        }
    }
    free(g->listes); // liberer le tableau de listes
    // pas besoin de liberer le graphe car graphe pas alloue dynamiquement
}

void charge(Graphe *g, const char *nom_fichier){
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return;
    }
    int n, m, oriente;
    fscanf(fichier, "%d %d %d", &oriente, &n, &m);
    init_graphe(g, n, oriente);

    int id1, id2;
    for (int i = 0; i < m; i++) {
        fscanf(fichier, "%d %d", &id1, &id2);
        ajout_lien(g, id1, id2); 
    }
    fclose(fichier);
}

void sauvegarder(Graphe *g, const char *nom_fichier){
    FILE *fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return;
    }
    fprintf(fichier, "%d %d %d\n", g->oriente, g->n, g->m);
    for (int i = 0; i < g->n; i++) {
        Noeud *current = g->listes[i];
        while (current != NULL) {
            if (g->oriente || i < current->s.id) { // prevent duplication in non or graph
                fprintf(fichier, "%d %d\n", i, current->s.id);
            }
            current = current->suivant;
        }
    }
    fclose(fichier);
}

int main(){
    Graphe g;
    charge(&g, "mon_graphe.txt");

    afficher_simple(&g);

    int de[g.n], dg[g.n];
    degres(&g, de, dg);
    for(int i = 0; i < g.n; i++){
        printf("Sommet %d: degre entrant = %d, degre sortant = %d\n", i, de[i], dg[i]);
    }

    ajout_lien(&g, 0, 3);
    sauvegarder(&g, "mon_graphe.txt");

    return 0;
}