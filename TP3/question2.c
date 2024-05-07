#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* Global variables */
int x = 0;
sem_t sync_ab, sync_ba; // Deux sémaphores distinctes pour les deux sens

/* Thread functions */
void a1(){
  sleep(10);
  printf("X = %d\n", x);
  x=55;
}

void a2(){
  printf("X = %d\n", x);
}

void b1(){
  sleep(5);
  x -= 10;
  printf("X = %d\n", x);
}

void b2(){
  printf("X = %d\n", x);
  x += 30;
}

void *p1(void *arg)
{
    a1();
    sem_post(&sync_ab);
    sem_wait(&sync_ba);
    a2();
    return NULL;
}

void *p2(void *arg)
{
    /* Wait for a1() to finish */
    sem_wait(&sync_ab);
    b2();
    /* Signal that b2() has finished */
    sem_post(&sync_ba);
    b1();
    return NULL;
}

int main () {
    pthread_t thread1, thread2;
    /* Initialize semaphores */
    sem_init(&sync_ab, 0, 0);
    sem_init(&sync_ba, 0, 0);

    if (pthread_create(&thread1, NULL, p1, NULL) < 0) {
        perror("Error: thread cannot be created");
        exit(1);
    }
    if (pthread_create(&thread2, NULL, p2, NULL) < 0) {
        perror("Error: thread cannot be created");
        exit(1);
    }
    /* wait for threads to terminate */
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    /* Destroy semaphores */
    sem_destroy(&sync_ab);
    sem_destroy(&sync_ba);
    exit(0);
}

