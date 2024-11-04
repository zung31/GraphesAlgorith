// l’algorithme de parcours en profondeur d’un graphe permettant (notamment) de
// découvrir l’ensemble des sommets accessibles à partir d’un sommet source.

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

// DFS: Depth First Search
// blanc: sommet non découvert
// gris: sommet découvert mais pas encore traité
// noir: sommet traité
void DFS_visit(Graphe *G, int u, int *d, int *f, int *p, int *time, int *color) {
    color[u] = 1; // set color de 0 = gris
    d[u] = ++(*time); // increment temp decouverte
    Noeud *current = G->listes[u];
    while (current != NULL) {
        int v = current->s.id;
        if (color[v] == 0) { // blanc
            p[v] = u; // predecesseur de v est u
            DFS_visit(G, v, d, f, p, time, color); // recursive avec des sommets adjacents de v
        }
        current = current->suivant;
    }
    color[u] = 2; // noir (fin de traitement)
    f[u] = ++(*time); // increment temp fin
}

// implémente la version du cours de ce parcours : utilisation des couleurs des sommets, et retour
// via les tableaux : d des dates de découverte des sommets, f des dates de fin de traitement, et p
// des prédécesseurs des sommets. Le sommet source est passé via le paramètre id.
void parcours(Graphe *G, int id, int *d, int *f, int *p) {
    int *color = (int *)malloc(G->n * sizeof(int)); // 0: blanc, 1: gris, 2: noir
    for (int i = 0; i < G->n; i++) {
        color[i] = 0; // initialiser tous les sommets à blanc
        p[i] = -1; // initialiser tous les predecesseurs à -1
    }
    int time = 0; // set time ban đầu = 0
    DFS_visit(G, id, d, f, p, &time, color);
    free(color);
}

void afficher_parcours(int id, int *d, int *f, int *p, int n) {
    printf("Sommet\tDecouverte\tFin\tPredecesseur\n");
    for (int i = 0; i < n; i++) {
        if(i != id && p[i] == -1) continue; // si sommet n'est pas accessible (different origine mais pas precedent)
        printf("%d\t%d\t\t%d\t%d\n", i, d[i], f[i], p[i]);
    }
}

// à partir du tableau p obtenu via le parcours, construit le graphe de liaison GL du graphe initial G
void liaison(Graphe *GL, Graphe *G, int *p){
    // Initialiser le graphe de liaison GL avec le même nombre de sommets que G
    init_graphe(GL, G->n, G->oriente);

    // Ajouter les arêtes selon le tableau des prédécesseurs p
    for (int v = 0; v < G->n; v++) {
        if (p[v] != -1) { // Si v a un prédécesseur
            ajout_lien(GL, p[v], v); // Ajouter une arête de p[v] à v
        }
    }
}

// permettant de remplir le tableau c contenant la composante connexe de chaque sommet du graphe
void composantes(Graphe *G, int id, int *d, int *f, int *p, int *c) {
    int *color = (int *)malloc(G->n * sizeof(int)); // 0: blanc, 1: gris, 2: noir
    for (int i = 0; i < G->n; i++) {
        color[i] = 0; // initialiser tous les sommets à blanc
        p[i] = -1; // initialiser tous les predecesseurs à -1
        c[i] = -1; // initialiser tous les composantes à -1
    }
    int time = 0; // set time ban đầu = 0
    int component_id = 0; // ID de la composante connexe
    for (int i = 0; i < G->n; i++) {
        if (color[i] == 0) { // Si le sommet n'est pas encore découvert
            DFS_visit(G, i, d, f, p, &time, color); // un composant connexe de G (une partie de G)
             // Marquer tous les sommets de cette composante avec component_id
            for (int j = 0; j < G->n; j++) {
                if (color[j] == 2 && c[j] == -1) { // Si le sommet est noir et n'a pas encore de composante
                    c[j] = component_id; // marquer id de composante
                }
            }
            component_id++; // augmenter id de composante pour prochaine composante
        }
    }
    free(color);
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

// l’ensemble des sommets accessibles à partir d’un sommet source
// -> peut etre pas tous les sommets dans graphe original
int main() {
    Graphe g;
    printf("Entrer nombre de sommets: ");
    int n;
    scanf("%d", &n);
    printf("Entrer probabilite p: ");
    double pct;
    scanf("%lf", &pct);
    int pourcentage = pct * 100;
    printf("Generer un graphe oriente (1) ou non oriente (0): ");
    int oriente;
    scanf("%d", &oriente);
    generationVer1(&g, n, pourcentage, oriente);
    printf("Graphe original:\n");
    afficher_simple(&g);

    printf("\nEntrer sommet source: ");
    int id;
    scanf("%d", &id);
    // d: date decouverte, f: date fin, p: predecesseur
    int d[n], f[n], p[n];
    parcours(&g, id, d, f, p); 
    afficher_parcours(id,d, f, p, n);

    printf("\nGraphe de liaison:\n");
    Graphe gl;
    liaison(&gl, &g, p);
    afficher_simple(&gl);

    int c[n];
    composantes(&g, 0, d, f, p, c);
    printf("\nComposantes connexes:\n");
    for (int i = 0; i < n; i++) {
        printf("Sommet %d: Composante %d\n", i, c[i]);
    }

    free(g.listes);
    free(gl.listes);
    return 0;
}