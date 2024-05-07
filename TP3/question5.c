#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "queue.h"

#define NUM_PRODUCTEURS 2
#define NUM_CONSOMMATEURS 2
#define QUEUE_CAPACITY 10 
#define NUM_MESSAGES 10 

sem_t sem_producteur;
sem_t sem_consommateur;
pthread_mutex_t mutex;
struct Queue queue;

void *Producteur(void *arg) {
    int producer_id = *((int *)arg);
    for (int i = 1; i <= NUM_MESSAGES; i++) {
        char message = 'A' + i - 1;
        sem_wait(&sem_producteur);
        pthread_mutex_lock(&mutex);
        enqueue(&queue, message);
        printf("Producteur %d: Message déposé: %c\n", producer_id, message);
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_consommateur);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *Consommateur(void *arg) {
    int consumer_id = *((int *)arg);
    for (int i = 1; i <= NUM_MESSAGES * NUM_PRODUCTEURS / NUM_CONSOMMATEURS; i++) {
        sem_wait(&sem_consommateur);
        pthread_mutex_lock(&mutex);
        char message = dequeue(&queue);
        pthread_mutex_unlock(&mutex);
        printf("Consommateur %d: Message retiré: %c\n", consumer_id, message);
        sem_post(&sem_producteur);
        sleep(2);
    }
    pthread_exit(NULL);
}

int main() {
    sem_init(&sem_producteur, 0, QUEUE_CAPACITY);
    sem_init(&sem_consommateur, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    queue = *createQueue(QUEUE_CAPACITY);

    pthread_t producer_threads[NUM_PRODUCTEURS];
    pthread_t consumer_threads[NUM_CONSOMMATEURS];
    int producer_ids[NUM_PRODUCTEURS] = {1, 2};
    int consumer_ids[NUM_CONSOMMATEURS] = {1, 2};

    for (int i = 0; i < NUM_PRODUCTEURS; i++) {
        pthread_create(&producer_threads[i], NULL, Producteur, &producer_ids[i]);
    }

    for (int i = 0; i < NUM_CONSOMMATEURS; i++) {
        pthread_create(&consumer_threads[i], NULL, Consommateur, &consumer_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCTEURS; i++) {
        pthread_join(producer_threads[i], NULL);
    }

    for (int i = 0; i < NUM_CONSOMMATEURS; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    sem_destroy(&sem_producteur);
    sem_destroy(&sem_consommateur);
    pthread_mutex_destroy(&mutex);
    free(queue.array);

    return 0;
}
