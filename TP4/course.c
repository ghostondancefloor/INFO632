#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NB_VOITURES 3
#define NB_TOURS 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t depart = PTHREAD_COND_INITIALIZER;

// Structure pour représenter une voiture
typedef struct Voiture {
    int identifiant;
    int tours_completes;
    double temps_total;
    pthread_t thread;
} Voiture;

Voiture voitures[NB_VOITURES];  // Tableau de voitures

// Fonction de comparaison 
int comparer_voitures(const void *a, const void *b) {
    Voiture *voitureA = (Voiture *)a;
    Voiture *voitureB = (Voiture *)b;
    if (voitureA->temps_total < voitureB->temps_total) return -1;
    if (voitureA->temps_total > voitureB->temps_total) return 1;
    return 0;
}

// Fonction pour le thread d'une voiture
void *exec_voiture(void *arguments) {
    Voiture *voiture = (Voiture *)arguments;
    time_t debut, fin;

    // Attente du départ
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&depart, &mutex);
    pthread_mutex_unlock(&mutex);

    // Départ
    pthread_mutex_lock(&mutex);
    printf("Voiture %d : Départ !\n", voiture->identifiant);
    pthread_mutex_unlock(&mutex);

    // Course
    time(&debut);
    for (int i = 0; i < NB_TOURS; i++) {
        // Simulation d'un tour
        sleep(rand() % 3);

        // Annonce du passage
        pthread_mutex_lock(&mutex);
        printf("Voiture %d : Tour %d\n", voiture->identifiant, i + 1);
        pthread_mutex_unlock(&mutex);

        voiture->tours_completes++;
    }
    time(&fin);  // Temps à la fin de la course

    // Calcul du temps total de la course pour chaque voiture
    voiture->temps_total = difftime(fin, debut);

    // Fin de la course
    pthread_mutex_lock(&mutex);
    printf("Fin de course pour la voiture %d (temps total : %.2f secondes, tours complets : %d)\n", voiture->identifiant, voiture->temps_total, voiture->tours_completes);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

// Fonction pour trier les voitures
void classement() {
    qsort(voitures, NB_VOITURES, sizeof(Voiture), comparer_voitures);

    // Affichage du classement
    printf("\nAffichage du classement :\n");
    for (int i = 0; i < NB_VOITURES; i++) {
        printf("%d. Voiture %d (Temps total : %.2f secondes)\n", i + 1, voitures[i].identifiant, voitures[i].temps_total);
    }
}

void *exec_arbitre(void *arguments) {
    sleep(1);
    pthread_mutex_lock(&mutex);
    printf("Arbitre : Toutes les voitures sont en position!\n");
    pthread_mutex_unlock(&mutex);

    // L'arbitre donne le départ
    usleep(500000);
    pthread_mutex_lock(&mutex);
    printf("Arbitre : Départ donné !\n");
    pthread_cond_broadcast(&depart);  // Signal de départ à toutes les voitures
    pthread_mutex_unlock(&mutex);

    // Attente de la fin des threads des voitures
    for (int i = 0; i < NB_VOITURES; i++) {
        pthread_join(voitures[i].thread, NULL);
    }

    // Trier les voitures et afficher le classement final
    classement();

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));  // Initialisation du générateur de nombres aléatoires

    // Création des threads pour les voitures
    for (int i = 0; i < NB_VOITURES; i++) {
        voitures[i].identifiant = i + 1;
        voitures[i].tours_completes = 0;
        voitures[i].temps_total = 0.0;
        pthread_create(&voitures[i].thread, NULL, exec_voiture, (void *)&voitures[i]);
    }

    // Création du thread pour l'arbitre
    pthread_t arbitre_thread;
    pthread_create(&arbitre_thread, NULL, exec_arbitre, NULL);

    // Attente de la fin du thread de l'arbitre
    pthread_join(arbitre_thread, NULL);

    return 0;
}
