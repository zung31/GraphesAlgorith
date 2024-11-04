// à partir d’un graphe donné et représenté par ses listes d’adjacence,
// de construire un autre graphe.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  int id;
} Sommet;

typedef struct noeud {
    Sommet s;
    struct noeud *suivant;
} Noeud;

typedef Noeud *Liste;

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

bool lien_existe(Graphe *g, int id1, int id2) {
    Noeud *current = g->listes[id1];
    while (current != NULL) {
        if (current->s.id == id2) {
            return true;
        }
        current = current->suivant;
    }
    return false;
}

void ajouterArc(Graphe *g, int id1, int id2) {
    if (!lien_existe(g, id1, id2)) {
        Noeud *n = (Noeud *)malloc(sizeof(Noeud)); // allocation dynamique
        n->s.id = id2; // set sommet = id2
        n->suivant = g->listes[id1]; 
        g->listes[id1] = n;
        g->m++;
    }
}

void ajouterArete(Graphe *g, int id1, int id2) {
    if (!lien_existe(g, id1, id2)) {
        Noeud *n = (Noeud *)malloc(sizeof(Noeud)); // allocation dynamique
        n->s.id = id2; // set sommet = id2
        n->suivant = g->listes[id1]; 
        g->listes[id1] = n;
        g->m++;
        if (id1 != id2) { // eviter la boucle
            ajouterArc(g, id2, id1); // on ajoute l'arete (j, i)
        }
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
}

// generer random lien entre sommets et avec probabilite <= p
void generationVer1(Graphe *g, int n, int p, bool oriente){
    init_graphe(g, n, oriente);
    int adjusted_p = oriente ? p : p / 2; // si graphe non oriente, on ajoute deux fois moins de liens
    for(int i = 0; i < n; i++){ // loop 2 fois pour chaque sommet pour creer les liens
        for(int j = 0; j < n; j++){
            if(i != j){
                int r = rand() % 100; // random entre 0 et 99
                // tưởng tượng generate và chỉ lấy một phần của một graphe dense
                if(r < adjusted_p){ // si r < p, on ajoute le lien car p est probabilite et on peut ajouter seulement selon prob
                // peut etre varie car r est random
                    ajout_lien(g, i, j);
                }
            }
        }
    }
}

// construire le sous graphe SG obtenu à partir du graphe G 
// et de l’ensemble de nb sommets S (un tableau)
void sous_graphe(Graphe *SG, Graphe *G, int nb, Sommet *S){
    init_graphe(SG, nb, G->oriente);
    for(int i = 0; i < nb; i++){ // loop through S
        Sommet sommet = S[i];
        int id = sommet.id;

        // loop through list sommet adjacent de id
        Noeud *current = G->listes[id];
        while (current != NULL) {
            // verifier si le sommet adjacent est dans S
            for (int j = 0; j < nb; j++) {
                if (current->s.id == S[j].id) {
                    // ajouter lien dans listes SG
                    ajout_lien(SG, id, current->s.id);
                    break; // vì mỗi lien chỉ được thêm 1 lần nên break để tránh tìm thêm
                }
            }
            current = current->suivant;
        }
    }
}

// construire le graphe complémentaire (ou inverse) GC du graphe G
void complementaire(Graphe *GC, Graphe *G){
    init_graphe(GC, G->n, G->oriente);
    for(int i = 0; i < G->n; i++){ // loop through all sommets 2 fois
        for(int j = 0; j < G->n; j++){
            if(i != j && !lien_existe(G, i, j)){
                ajout_lien(GC, i, j);
            }
        }
    }
}

// en fait on peut trier le graphe avant entrer dans cette fonction
// par ex: les grpahes non orientes ne rentrent pas dans cette fonction
void transposee(Graphe *GT, Graphe *G){
    init_graphe(GT, G->n, G->oriente);
    if(!G->oriente){ // Gt est identique à G
        for(int i = 0; i< G->n; i++){
            Noeud *current = G->listes[i];
            while (current != NULL) {
                ajout_lien(GT, i, current->s.id);
                current = current->suivant;
            }
        }
    }
    else{ // inverser les arcs
        for(int i = 0; i< G->n; i++){
            Noeud *current = G->listes[i];
            while (current != NULL) {
                ajout_lien(GT, current->s.id, i);
                current = current->suivant;
            }
        }
    }
}

// construire le graphe de ligne (ou adjoint) GA de G
void adjoint(Graphe *GA, Graphe *G){
    init_graphe(GA, G->n, G->oriente);
    for (int i = 0; i < G->n; i++) {
        Noeud *current = G->listes[i];
        while (current != NULL) { // list sommet adjacent de i
            Noeud *temp = G->listes[current->s.id];
            while (temp != NULL) {  // list sommet adjacent de current (sommet adjacent de i)
                if (temp->s.id != i) {
                    ajout_lien(GA, i, temp->s.id);
                }
                temp = temp->suivant;
            }
            current = current->suivant;
        }
    }
}

// on va /2 g->m dans afficher_simple pour les graphes non orientes (si on veut qu'on peut faire dans chaque fonction)
void afficher_simple(Graphe *g){
    printf("Nombre de sommets: %d\n", g->n);
    printf("Nombre de connexions: %d\n", g->oriente? g->m : g->m / 2); // car on incremente 2 fois g->m pour les graphes non orientes
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

int main(){
    Graphe g;
    printf("Entrer nombre de sommets: ");
    int n;
    scanf("%d", &n);
    printf("Entrer probabilite p: ");
    double p;
    scanf("%lf", &p);
    int pourcentage = p * 100;
    printf("Generer un graphe oriente (1) ou non oriente (0): ");
    int oriente;
    scanf("%d", &oriente);
    generationVer1(&g, n, pourcentage, oriente);
    printf("Graphe original:\n");
    afficher_simple(&g);

    Graphe SG;
    Sommet S[3] = {{0}, {1}, {2}};
    sous_graphe(&SG, &g, 3, S);
    printf("\nSous graphe:\n");
    afficher_simple(&SG);

    Graphe GC;
    complementaire(&GC, &g);
    printf("\nGraphe complementaire:\n");
    afficher_simple(&GC);

    Graphe GT;
    transposee(&GT, &g);
    printf("\nGraphe transposee:\n");
    afficher_simple(&GT);

    Graphe GA;
    if(g.oriente){
        printf("Seulement les graphes non oriente peuvent utiliser la fonction adjoint\n");
    }
    else{
        adjoint(&GA, &g);
        printf("\nGraphe adjoint:\n");
        afficher_simple(&GA);
    }
    
    return 0;
}