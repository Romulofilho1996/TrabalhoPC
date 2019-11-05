#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define PNE 2 //numero de pessoas com necessidades especiais
#define PREF 3  //numero de pessoas preferenciais 
#define NG 4 //numero de pessoas que não se encaixam nos requisitos acima (geral)
#define ASSENTOS 5 //numero de assentos disponíveis na sala

void * especiais(void * meuid); //declaração de protipagem da função
void * preferencial (void * meuid); //declaração de protipagem da função
void * geral (void * meuid); //declaração de protipagem da função

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t pne_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pref_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t geral_cond = PTHREAD_COND_INITIALIZER;

// numero de assentos ocupados e numero de pessoas querendo entrar na sala
int numOCUPADOS = 0; 
int numPNE = 0;
int numPREF = 0;
int numGERAL = 0;


int main(int argc, char *argv[]){

  int erro;
  int i; //contador pra for's
  int *id; //id da thread

  pthread_t tPNE[PNE]; //cria thread
  for (i = 0; i < PNE; i++){ //cria pne threads
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tPNE[i], NULL, especiais, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }


  pthread_t tPREF[PREF]; //cria thread
  for (i = 0; i < PREF; i++){ //cria pref threads
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tPREF[i], NULL, preferencial, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tNG[NG]; //cria thread
  for (i = 0; i < NG; i++){ //cria ng threads
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tNG[i], NULL, geral, (void *) (id));
    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
  pthread_join(tPNE[0],NULL);
  return 0;
} 

void * especiais (void* pid){
  while(1){
    //sleep(2);
    pthread_mutex_lock(&mutex);
    printf("Pessoa PNE %d: Vou me sentar. \n", *(int *)(pid));
    numPNE++;
    while(numOCUPADOS == ASSENTOS) {
      //printf("Especiais %d: Vou aguardar um assento \n", *(int *)(pid));
      pthread_cond_wait(&pne_cond,&mutex);
    }
    numPNE--;
    numOCUPADOS++;
    printf("Pessoa PNE %d: Sentado. Numº assentos ocupados = %d, Numº PNE na fila = %d, Numº PREF na fila = %d, Numº GERAL na fila = %d\n", *(int *)(pid),numOCUPADOS,numPNE,numPREF,numGERAL);
    pthread_mutex_unlock(&mutex);

    sleep(5); 
  
    pthread_mutex_lock(&mutex);
    numOCUPADOS--; 
    printf("Pessoa PNE %d: Saí da sala. Numº assentos ocupados = %d\n", *(int *)(pid),numOCUPADOS);

    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);
        

    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}


void * geral (void* pid){
  while(1){
    //sleep(2);
    pthread_mutex_lock(&mutex);
    printf("Pessoa Geral %d: Vou me sentar. \n", *(int *)(pid));
    numPREF++;
    while(numOCUPADOS == ASSENTOS || numPNE > 0) {
      //printf("Geral %d: Vou aguardar um assento \n", *(int *)(pid));
      pthread_cond_wait(&pref_cond,&mutex);
    }
    numPREF--;
    numOCUPADOS++;
    printf("Pessoa Geral %d: Sentado. Numº assentos ocupados = %d, Numº PNE na fila = %d, Numº PREF na fila = %d, Numº Geral na fila = %d\n", *(int *)(pid),numOCUPADOS,numPNE,numPREF,numGERAL);
    pthread_mutex_unlock(&mutex);

    sleep(4); 

    pthread_mutex_lock(&mutex);  
    numOCUPADOS--; 
    printf("Pessoa Geral %d: Saí da sala. Numº assentos ocupados = %d\n", *(int *)(pid),numOCUPADOS);

    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);

    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}


void * preferencial (void* pid){
  while(1){
    //sleep(2);
    pthread_mutex_lock(&mutex);
    printf("Pessoa Preferencial %d: Vou me sentar. \n", *(int *)(pid));
     
    numGERAL++;
    while(numOCUPADOS == ASSENTOS || numPNE > 0 || numPREF > 0) {
      //printf("Pessoa Preferencial %d: Vou aguardar um assento \n", *(int *)(pid));
      pthread_cond_wait(&geral_cond,&mutex);
    }
    numGERAL--;
    numOCUPADOS++;
    printf("Pessoa Preferencial %d: Sentado. Numº assentos ocupados = %d, Numº PNE na fila = %d, Numº PREF na fila = %d, Numº Geral na fila = %d\n", *(int *)(pid),numOCUPADOS,numPNE,numPREF,numGERAL);
    pthread_mutex_unlock(&mutex);

    sleep(5); 
  
    pthread_mutex_lock(&mutex);
    numOCUPADOS--; 
    printf("Pessoa Preferencial %d: Saí da sala. Numº assentos ocupados = %d\n", *(int *)(pid),numOCUPADOS);


    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);
    
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}
