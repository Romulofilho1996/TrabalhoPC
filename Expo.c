#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define PNE 4 //numero de pessoas com necessidades especiais
#define PREF 6  //numero de pessoas preferenciais 
#define NG 7 //numero de pessoas que não se encaixam nos requisitos acima (geral)
#define CAPACIDADESALA 2 //numero de capacidade sala disponíveis na sala
#define NUMEROSALA 3 //numero de salas na exposição
#define COPOSAGUA 4 //numero de copos d'agua que o garçom tem 

void * especiais(void * meuid); //declaração de protipagem da função
void * preferencial (void * meuid); //declaração de protipagem da função
void * geral (void * meuid); //declaração de protipagem da função

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t pne_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pref_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t geral_cond = PTHREAD_COND_INITIALIZER;

// numero de capacidade sala ocupados e numero de pessoas querendo entrar na sala
int aguaGarcom[NUMEROSALA];
int numOCUPADOS[NUMEROSALA]; 
int numPNE[NUMEROSALA];
int numPREF[NUMEROSALA];
int numGERAL[NUMEROSALA];


int main(int argc, char *argv[]){

  int j = 0;

  for(j = 0; j < NUMEROSALA; j++)
    numOCUPADOS[j] = 0;

  for(j = 0; j < NUMEROSALA; j++)
    numPNE[j] = 0;

  for(j = 0; j < NUMEROSALA; j++)
    numPREF[j] = 0;

  for(j = 0; j < NUMEROSALA; j++)
    numGERAL[j] = 0;

  for(j = 0; j < NUMEROSALA; j++)
    aguaGarcom[j] = COPOSAGUA;

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

  for(j = 0; j < PNE; j++){
    pthread_join(tPNE[j],NULL);
  }

  for(j = 0; j < PREF; j++){
    pthread_join(tPREF[j],NULL);
  }

  for(j = 0; j < NG; j++){
    pthread_join(tNG[j],NULL);
  }

  return 0;
} 

void * especiais (void* pid){
  int salaAtual = 0;
  while(1){
    pthread_mutex_lock(&mutex);
    printf("Pessoa PNE %d: Estou na fila da sala %d da exposição.  \n", *(int *)(pid), salaAtual);
    numPNE[salaAtual]++;
    while(numOCUPADOS[salaAtual] == CAPACIDADESALA) {
      if(aguaGarcom[salaAtual] > 0){
        aguaGarcom[salaAtual]--;
        printf("Garçom da fila da sala %d: Dando agua para Pessoa PNE %d, existem %d águas restantes \n", salaAtual, *(int *)(pid), aguaGarcom[salaAtual]);
      }else{
        printf("O garçom da fila %d não tem mais água para as pessoas da fila \n", salaAtual);
      }
      pthread_cond_wait(&pne_cond,&mutex);
    }
    numPNE[salaAtual]--;
    numOCUPADOS[salaAtual]++;
    printf("Pessoa PNE %d: Entrei na sala %d. Numº pessoas na sala de exposição = %d, Numº PNE na fila = %d, Numº PREF na fila = %d, Numº Geral na fila = %d\n", *(int *)(pid), salaAtual, numOCUPADOS[salaAtual],numPNE[salaAtual],numPREF[salaAtual],numGERAL[salaAtual]);
    pthread_mutex_unlock(&mutex);

    sleep(5); 
  
    pthread_mutex_lock(&mutex);
    numOCUPADOS[salaAtual]--; 
    printf("Pessoa PNE %d: Saí da sala %d. Numº pessoas na sala da exposição %d = %d\n", *(int *)(pid), salaAtual, salaAtual, numOCUPADOS[salaAtual]);
    salaAtual++;

    if(salaAtual == NUMEROSALA){
      pthread_mutex_unlock(&mutex);
      pthread_exit(NULL);
    }
    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}


void * preferencial (void* pid){
  int salaAtual = 0;
  while(1){
    pthread_mutex_lock(&mutex);
    printf("Pessoa Preferencial %d: Estou na fila da sala %d da exposição. \n", *(int *)(pid), salaAtual);
    numPREF[salaAtual]++;
    while(numOCUPADOS[salaAtual] == CAPACIDADESALA || numPNE[salaAtual] > 0) {
      if(aguaGarcom[salaAtual] > 0){
        aguaGarcom[salaAtual]--;
        printf("Garçom da fila da sala %d: Dando agua para Pessoa Preferencial %d, existem %d águas restantes \n", salaAtual, *(int *)(pid), aguaGarcom[salaAtual]);
      }else{
        printf("O garçom da fila %d não tem mais água para as pessoas da fila \n", salaAtual);
      }
      pthread_cond_wait(&pref_cond,&mutex);
    }
    numPREF[salaAtual]--;
    numOCUPADOS[salaAtual]++;
    printf("Pessoa Preferencial %d: Entrei na sala %d. Numº pessoas na sala de exposição = %d, Numº PNE na fila = %d, Numº PREF na fila = %d, Numº Geral na fila = %d\n", *(int *)(pid), salaAtual,numOCUPADOS[salaAtual],numPNE[salaAtual],numPREF[salaAtual],numGERAL[salaAtual]);
    pthread_mutex_unlock(&mutex);

    sleep(5); 

    pthread_mutex_lock(&mutex);  
    numOCUPADOS[salaAtual]--; 
    printf("Pessoa Preferencial %d: Saí da sala %d. Numº pessoas na sala da exposição %d = %d\n", *(int *)(pid), salaAtual, salaAtual, numOCUPADOS[salaAtual]);
    salaAtual++;

    if(salaAtual == NUMEROSALA){
      pthread_mutex_unlock(&mutex);
      pthread_exit(NULL);
    }
    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}


void * geral (void* pid){
  int salaAtual = 0;
  while(1){
    pthread_mutex_lock(&mutex);
    printf("Pessoa Geral %d: Estou na fila da sala %d da exposição.  \n", *(int *)(pid), salaAtual);
     
    numGERAL[salaAtual]++;
    while(numOCUPADOS[salaAtual] == CAPACIDADESALA || numPNE[salaAtual] > 0 || numPREF[salaAtual] > 0) {
      if(aguaGarcom[salaAtual] > 0){
        aguaGarcom[salaAtual]--;
        printf("Garçom da fila da sala %d: Dando agua para Pessoa Geral %d, existem %d águas restantes \n", salaAtual, *(int *)(pid), aguaGarcom[salaAtual]);
      }else{
        printf("O garçom da fila %d não tem mais água para as pessoas da fila \n", salaAtual);
      }
      pthread_cond_wait(&geral_cond,&mutex);
    }
    numGERAL[salaAtual]--;
    numOCUPADOS[salaAtual]++;
    printf("Pessoa Geral %d: Entrei na sala %d. Numº pessoas na sala de exposição = %d, Numº PNE na fila = %d, Numº PREF na fila = %d, Numº Geral na fila = %d\n", *(int *)(pid), salaAtual, numOCUPADOS[salaAtual],numPNE[salaAtual],numPREF[salaAtual],numGERAL[salaAtual]);
    pthread_mutex_unlock(&mutex);

    sleep(5); 
  
    pthread_mutex_lock(&mutex);
    numOCUPADOS[salaAtual]--; 
    printf("Pessoa Geral %d: Saí da sala %d. Numº pessoas na sala de exposição %d = %d\n", *(int *)(pid), salaAtual, salaAtual, numOCUPADOS[salaAtual]);
    salaAtual++;

    if(salaAtual == NUMEROSALA){
      pthread_mutex_unlock(&mutex);
      pthread_exit(NULL);
    }
    pthread_cond_signal(&pne_cond);
    pthread_cond_signal(&pref_cond);
    pthread_cond_signal(&geral_cond);
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  
}
