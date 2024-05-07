#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TAILLE_BUFFER 5

char buffer[TAILLE_BUFFER]; 
int depot = 0; 
int retrait = 0; 

sem_t sem_depot;
sem_t sem_retrait; 

// Fonction pour déposer un message dans le tampon
void Deposer(char message) {
    sem_wait(&sem_depot); 
    buffer[depot] = message; 
    depot = (depot + 1) % TAILLE_BUFFER; 
    sem_post(&sem_retrait); 
}

// Fonction pour retirer un message du tampon
char Retirer() {
    sem_wait(&sem_retrait); 
    char message = buffer[retrait]; 
    retrait = (retrait + 1) % TAILLE_BUFFER; 
    sem_post(&sem_depot); 
    return message; // Retourner le message retiré
}

// Fonction du producteur pour produire des messages
void *Producteur(void *arg) {
    for (int i = 1; i <= 10; i++) {
        char message = 'A' + i - 1;
        Deposer(message); 
        printf("Producteur : Message déposé : %c\n", message);
        sleep(1); 
    }
    pthread_exit(NULL);
}

// Fonction du consommateur pour consommer les messages
void *Consommateur(void *arg) {
    for (int i = 1; i <= 10; i++) {
        char message = Retirer(); 
        printf("Consommateur : Message retiré : %c\n", message);
        sleep(2); 
    }
    pthread_exit(NULL);
}

int main() {
    // Initialiser les sémaphores
    sem_init(&sem_depot, 0, TAILLE_BUFFER); 
    sem_init(&sem_retrait, 0, 0); 

    pthread_t producteur_thread, consommateur_thread;

    // Créer les threads du producteur et du consommateur
    pthread_create(&producteur_thread, NULL, Producteur, NULL);
    pthread_create(&consommateur_thread, NULL, Consommateur, NULL);

    // Attendre la fin des threads
    pthread_join(producteur_thread, NULL);
    pthread_join(consommateur_thread, NULL);

    // Détruire les sémaphores
    sem_destroy(&sem_depot);
    sem_destroy(&sem_retrait);

    return 0;
}