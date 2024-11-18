#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Número de filósofos, tenedores y ciclos
#define N 5
#define CICLOS 3 // Número de ciclos de pensar/comer
#define MAX_COMIENDO 2 // Número máximo de filósofos que pueden comer al mismo tiempo

// Declaración de la barrera y el semáforo
pthread_barrier_t barrier;
sem_t sem_tenedores;

// Simulación de las funciones de los filósofos
void pensar(int id) {
    printf("Filósofo %d está pensando.\n", id);
    sleep(rand() % 3 + 1); // Tiempo aleatorio pensando
}

void comer(int id) {
    printf("Filósofo %d está comiendo.\n", id);
    sleep(rand() % 3 + 1); // Tiempo aleatorio comiendo
}

void tomar_tenedores(int id) {
    printf("Filósofo %d toma los tenedores.\n", id);
}

void poner_tenedores(int id) {
    printf("Filósofo %d deja los tenedores.\n", id);
}

// Función de los filósofos
void* filosofo(void* arg) {
    int id = *(int*)arg;

    for (int ciclo = 0; ciclo < CICLOS; ciclo++) {
        pensar(id);

        // Sincronizar antes de tomar los tenedores
        pthread_barrier_wait(&barrier);

        // Esperar disponibilidad para comer (máximo 2 comiendo a la vez)
        sem_wait(&sem_tenedores);

        tomar_tenedores(id);

        comer(id);

        poner_tenedores(id);

        // Liberar espacio para que otro filósofo pueda comer
        sem_post(&sem_tenedores);

        // Sincronizar después de comer
        pthread_barrier_wait(&barrier);
    }

    printf("Filósofo %d ha terminado sus ciclos.\n", id);
    return NULL;
}

int main() {
    pthread_t filosofos[N];
    int ids[N];

    // Inicializar la barrera
    pthread_barrier_init(&barrier, NULL, N);

    // Inicializar el semáforo (máximo 2 filósofos pueden comer al mismo tiempo)
    sem_init(&sem_tenedores, 0, MAX_COMIENDO);

    // Crear los hilos de los filósofos
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&filosofos[i], NULL, filosofo, &ids[i]);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < N; i++) {
        pthread_join(filosofos[i], NULL);
    }

    // Destruir la barrera y el semáforo
    pthread_barrier_destroy(&barrier);
    sem_destroy(&sem_tenedores);

    printf("Todos los filósofos han terminado.\n");
    return 0;
}

