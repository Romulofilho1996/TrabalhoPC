#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define PNE 3 //numero de pessoas com necessidades especiais
#define PREF 5  //numero de pessoas preferenciais 
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
  for (i = 0; i < PNE; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tPNE[i], NULL, especiais, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }


  pthread_t tPREF[PREF];
  for (i = 0; i < PREF; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tPREF[i], NULL, preferencial, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
  pthread_t tNG[NG];
  for (i = 0; i < NG; i++){
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

void * especiais (void* pi){
  while(1){
    //sleep(2);
    pthread_mutex_lock(&mutex);
    printf("Especiais %d: vou estacionar \n", *(int *)(pi));
        
    numPNE++;
    while(numOCUPADOS == ASSENTOS) {
      //printf("especiais %d: vou aguardar uma vaga \n", *(int *)(pi));
      pthread_cond_wait(&pne_cond,&mutex);
    }
    numPNE--;
    numOCUPADOS++;
    printf("Especiais %d: estacionei, num assentos ocupados = %d, numPNE = %d, numPREF = %d, numGERAL = %d\n", *(int *)(pi),numOCUPADOS,numPNE,numPREF,numGERAL);
    pthread_mutex_unlock(&mutex);

    sleep(5); 
  
    pthread_mutex_lock(&mutex);
    numOCUPADOS--; 
    printf("Especiais %d: sai, num assentos ocupados = %d\n", *(int *)(pi),numOCUPADOS);

    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);
        

    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}


void * geral (void* pi){
  while(1){
    //sleep(2);
    pthread_mutex_lock(&mutex);
    printf("geral %d: vou estacionar \n", *(int *)(pi));
    numPREF++;
    while(numOCUPADOS == ASSENTOS || numPNE > 0) {
      //printf("geral %d: vou aguardar uma vaga \n", *(int *)(pi));
      pthread_cond_wait(&pref_cond,&mutex);
    }
    numPREF--;
    numOCUPADOS++;
    printf("geral %d: estacionei, num assentos ocupados = %d, numPNE = %d, numPREF = %d, numGERAL = %d\n", *(int *)(pi),numOCUPADOS,numPNE,numPREF,numGERAL);
    pthread_mutex_unlock(&mutex);

    sleep(4); 

    pthread_mutex_lock(&mutex);  
    numOCUPADOS--; 
    printf("geral %d: sai, num assentos ocupados = %d\n", *(int *)(pi),numOCUPADOS);

    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);

    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}


void * preferencial (void* pi){
  while(1){
    //sleep(2);
    pthread_mutex_lock(&mutex);
    printf("preferencial %d: vou estacionar \n", *(int *)(pi));
     
    numGERAL++;
    while(numOCUPADOS == ASSENTOS || numPNE > 0 || numPREF > 0) {
      //printf("preferencial %d: vou aguardar uma vaga \n", *(int *)(pi));
      pthread_cond_wait(&geral_cond,&mutex);
    }
    numGERAL--;
    numOCUPADOS++;
    printf("preferencial %d: estacionei, num assentos ocupados = %d, numPNE = %d, numPREF = %d, numGERAL = %d\n", *(int *)(pi),numOCUPADOS,numPNE,numPREF,numGERAL);
    pthread_mutex_unlock(&mutex);

    sleep(5); 
  
    pthread_mutex_lock(&mutex);
    numOCUPADOS--; 
    printf("preferencial %d: sai, num assentos ocupados = %d\n", *(int *)(pi),numOCUPADOS);


    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);
    
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}
