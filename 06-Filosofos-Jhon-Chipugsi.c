#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5 // Número de filósofos
#define PENSANDO 0
#define HAMBRIENTO 1
#define COMIENDO 2
#define VUELTAS 3 // Número de vueltas que darán los filósofos

int estado[N]; // Estado de cada filósofo
pthread_mutex_t mutex; // Mutex para controlar acceso a estado
pthread_cond_t s[N]; // Semáforos para cada filósofo
pthread_barrier_t barrier; // Barrera para sincronización

void pensar(int i) {
    printf("Filósofo %d: pensando\n", i);
    sleep(rand() % 3); // Simula tiempo pensando
}

void comer(int i) {
    printf("Filósofo %d: comiendo\n", i);
    sleep(rand() % 3); // Simula tiempo comiendo
}

void probar(int i) {
    if (estado[i] == HAMBRIENTO && estado[(i + N - 1) % N] != COMIENDO && estado[(i + 1) % N] != COMIENDO) {
        estado[i] = COMIENDO;
        pthread_cond_signal(&s[i]);
    }
}

void tomar_tenedores(int i) {
    pthread_mutex_lock(&mutex);
    estado[i] = HAMBRIENTO;
    probar(i);
    while (estado[i] != COMIENDO) {
        pthread_cond_wait(&s[i], &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void poner_tenedores(int i) {
    pthread_mutex_lock(&mutex);
    estado[i] = PENSANDO;
    probar((i + N - 1) % N); // Verifica al filósofo de la izquierda
    probar((i + 1) % N); // Verifica al filósofo de la derecha
    pthread_mutex_unlock(&mutex);
}

void* filosofo(void* num) {
    int i = *(int*)num;
    for(int vuelta = 0; vuelta < VUELTAS; vuelta++) {
        printf("Filósofo %d comienza vuelta %d\n", i, vuelta + 1);
        
        pensar(i);
        
        // Esperar a que todos estén listos para comer
        pthread_barrier_wait(&barrier);
        
        tomar_tenedores(i);
        comer(i);
        poner_tenedores(i);
        
        // Esperar a que todos terminen de comer
        pthread_barrier_wait(&barrier);
    }
    return NULL;
}

int main() {
    pthread_t hilos[N];
    int ids[N];
    
    pthread_mutex_init(&mutex, NULL);
    // Inicializar la barrera
    pthread_barrier_init(&barrier, NULL, N);
    
    for (int i = 0; i < N; i++) {
        estado[i] = PENSANDO;
        ids[i] = i;
        pthread_cond_init(&s[i], NULL);
        pthread_create(&hilos[i], NULL, filosofo, &ids[i]);
    }
    
    for (int i = 0; i < N; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    for (int i = 0; i < N; i++) {
        pthread_cond_destroy(&s[i]);
    }
    
    return 0;
}