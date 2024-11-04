//  implémenter un algorithme de génération aléatoire de graphes
// G(n,p) où n est le nombre de sommets et p la probabilité d'existence d'un lien
// Chaque paire (x, y) de sommets est connectée de façon indépendante selon la probabilité p
// p=0 et p=1 correspondant à un graphe sans connexion et un graphe complet

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

// generer random lien entre sommets et avec probabilite = p
void generationVer2(Graphe *g, int n, int p, bool oriente){
    printf("Helo");
    init_graphe(g, n, oriente);
    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires
    printf("Helo");

    // Créer une liste de toutes les paires (i, j) avec i != j
    int max_edges = oriente ? n * (n - 1) : n * (n - 1) / 2;
    int edges[max_edges][2];
    int index = 0;
    for (int i = 0; i < n; i++) { // remplir tableau avec toutes les paires
        for (int j = 0; j < n; j++) {
            if (i != j && (oriente || i < j)) { // pour avoir de places pour tous les liens de case non oriente
                edges[index][0] = i;
                edges[index][1] = j;
                index++;
            }
        }
    }

    // afficher les paires
    for (int i = 0; i < max_edges; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%d ", edges[i][j]);
        }
    }

    // Mélanger la liste des paires
    for (int i = max_edges - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp0 = edges[i][0];
        int temp1 = edges[i][1];
        edges[i][0] = edges[j][0];
        edges[i][1] = edges[j][1];
        edges[j][0] = temp0;
        edges[j][1] = temp1;
    }

    // Ajouter les t premières paires comme liens
    int t = p * max_edges / 100;
    for (int i = 0; i < t && i < max_edges; i++) {
        int id1 = edges[i][0];
        int id2 = edges[i][1];
        ajout_lien(g, id1, id2);
    }
}

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
    generationVer2(&g, n, pourcentage, oriente);
    afficher_simple(&g);
}