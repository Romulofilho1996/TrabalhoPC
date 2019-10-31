#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include<time.h>

#define n 100 /*número maximo de caracteres no nome do filme*/
#define max_filmes 10 /*Aqui definimos o numero de filmes, caso aumente ou diminua no arquivo texto de entrada, eh necessario alterar aqui*/
#define periodo 3 /*Aqui definimos o numero de sessoes para cada produto, caso aumente ou diminua, eh necessario altera-lo tambem*/

#define max_lugares 2 /*numero de lugaresxlugares na sala, sempre sera quadrada*/
#define sala_cheia max_lugares*max_lugares /*numero quando sala esta cheia*/
#define max_clientes_comprando 5 /* número máximo de clientes usando terminal de compra juntos */
#define max_tentativas 3 /*número maximo de tentativas de escolhas de filmes (caso esteja cheio) ou lugares (caso esteja ocupado) */

#define numero_preferenciais 4 //numero de preferenciais
#define numero_legais 5 // numero de pessoas legais
#define numero_chatos 5 // numero de gente chata

pthread_mutex_t lock; //lock
pthread_cond_t prefs; //variável de condição de preferenciais
pthread_cond_t legais; //variável de condição de legais
pthread_cond_t chatos; //variável de condição de chatos
int tem_pref = 0; //contador de preferenciais que querem comprar
int tem_legal = 0; //contador de legais que querem comprar
int clientes_comprando = 0; //contador de clientes usando terminal de compra



typedef struct{
	int lugares[max_lugares][max_lugares];/*Lugares para cada sessão*/
	int capacidade; //começa com 0 e a cada lugar comprado aumenta
}nodo3;

typedef struct{
	char sessao[periodo+1][n];/*sessões para cada filme*/
	nodo3 lugar[periodo+1];
}nodo2;

/*struct em que salvaremos informacoes sobre os filmes*/
typedef struct{
	int cod[max_filmes+1]; /*codigo*/
	char nome[max_filmes+1][n]; /*nome do filme*/
	int sala[max_filmes+1]; /*sala de cinema*/
	nodo2 info[max_filmes+1]; /*struct com sessoes por horario e lugares*/
}nodo;

nodo filme; /*Criamos a struct dos filmes*/

void imprime_fileira(int fil, int pp, int i, int *y){
int t, j;

for(t=0;t<max_lugares;t++) y[t]=0;


for(j=0;j<max_lugares;j++){
	if(filme.info[i].lugar[pp].lugares[fil][j] != 0)
		y[j]=1;
}
printf("\n");
for(j=1;j<=max_lugares;j++){ 
	if(y[j-1]==1){
		printf("XX ");
	}else{
		printf("%c%d ",fil+65,j);
	}
}

			printf("   Fileira %d\n",fil+1);

}



/*Funcao que imprime os filmes na tela*/
void Imprime (){
  int i, j;
/*Percorremos por todos os produtos e imprimimos as informacoes na tela*/
    for(i=1;i<=max_filmes;i++){
		printf("%d	%s		%d	", filme.cod[i], filme.nome[i], filme.sala[i]);
      for(j=1;j<=periodo;j++)
	printf("%s		", filme.info[i].sessao[j]);
      printf("\n");
    }
  
}

/*Função que faz a compra do ingresso*/
void Compra (){
int i, cod, pp, flag = 0, j, k, fileira, assento, pag, imp;
int y[max_lugares];
/*iremos localizar o filme desejado pelo seu codigo (que deve ser unico)*/
	printf("\nDigite o codigo do filme que deseja comprar: ");
	scanf("%d",&cod);
/*percorremos todos filmes em busca do codigo, ao acha-lo, pedimos pela sessão que deseja assistir*/
	for (i=1;i<=max_filmes;i++){
		if (filme.cod[i]==cod){
			flag = 1;
			printf("\nFilme escolhido: %s\n\nDigite a sessao que deseja assistir: ", filme.nome[i]);
			scanf("%d",&pp);
			while((pp<1)||(pp>periodo)) {
				printf("Por favor digite uma sessao valida (1 - %d): ", periodo);
				scanf("%d",&pp);
				getchar();
			}
			printf("\nSessao escolhida: %s\n\n", filme.info[i].sessao[pp]);
			printf("Aqui estão os lugares disponiveis: \n\n------------------------------\n            TELA\n");   

/*Aqui imprimimos os lugares de cada sessão*/

			for (int k = 0; k <max_lugares; k++){
				imprime_fileira(k, pp, i, y);
			}

			printf("\n\nDigite a fileira que deseja comprar: ");
			scanf("%d",&fileira);
			getchar();
			while((fileira<1)||(fileira>max_lugares)) {
				printf("Por favor digite uma opção valida! (entre 1 e %d):\n",max_lugares);
				scanf("%d",&fileira);
				getchar();
			}
			printf("\n\nDigite o assento que deseja comprar: ");
			scanf("%d",&assento);
			getchar();
			while((assento<1)||(assento>max_lugares)) {
				printf("Por favor digite uma opção valida! (entre 1 e %d):\n", max_lugares);
				scanf("%d",&assento);
				getchar();
			}

			while(filme.info[i].lugar[pp].lugares[fileira-1][assento-1] != 0){
				printf("\nLugar indisponivel, favor escolher outro lugar!\n\nDigite a fileira que deseja comprar: ");
				scanf("%d",&fileira);
				getchar();
				while((fileira<1)||(fileira>max_lugares)) {
					printf("Por favor digite uma opção valida! (entre 1 e %d):\n",max_lugares);
					scanf("%d",&fileira);
					getchar();
				}
				printf("\n\nDigite o assento que deseja comprar: ");
				scanf("%d",&assento);
				getchar();
				while((assento<1)||(assento>max_lugares)) {
					printf("Por favor digite uma opção valida! (entre 1 e %d):\n", max_lugares);
					scanf("%d",&assento);
					getchar();
				}
			}

				filme.info[i].lugar[pp].lugares[fileira-1][assento-1] = 1;
				printf("\nCompra realizada com sucesso.");
		}
	}


/*Caso o codigo digitado nao seja encontrado, imprime mensagem de erro*/
	if (flag == 0){
		printf("\nCODIGO NAO ENCONTRADO!\n");
		getchar();
	}
	
}

void * cliente_preferencial (void *arg){
	int i = *((int *) arg);
	int cod, pp, fileira, assento, tentativas = 0;
	int y[max_lugares];
	while (1){
		pthread_mutex_lock(&lock);
		printf("\nCliente Preferencial [%d]: Vou comprar ingresso! \n", i);
		tem_pref ++;
		while (clientes_comprando >= max_clientes_comprando){
			printf("Tem %d clientes preferenciais e %d clientes legais na fila! \n", tem_pref, tem_legal);
			pthread_cond_wait(&prefs, &lock);
		}
		tem_pref--;
		clientes_comprando++;
		cod = 1+(rand() % (max_filmes));
		//cod = 1;
		pp = 1+(rand() % (periodo));
		//pp =1;
		printf("Cliente Preferencial [%d]: Escolhi filme %s, sessão %d \n", i, filme.nome[cod], pp);
		while (filme.info[cod].lugar[pp].capacidade == sala_cheia && tentativas <= max_tentativas){
			printf("Cliente Preferencial [%d]: Sala que eu escolhi ta cheia, vou escolher outro! \n", i);
			cod = 1+(rand() % (max_filmes));
			pp = 1+(rand() % (periodo));
			printf("Cliente Preferencial [%d]: Escolhi filme %s, sessão %d \n", i, filme.nome[cod], pp);
			tentativas++;
		}
		if (tentativas > max_tentativas){
			printf("Cliente Preferencial [%d]: Tentei muito, mas ta tudo cheio, bjs tchau! \n", i);
			clientes_comprando --;
			pthread_cond_broadcast(&prefs);
			pthread_cond_broadcast(&legais);
			pthread_cond_broadcast(&chatos);
			pthread_mutex_unlock(&lock);
			break;
		}
			tentativas = 0;
			fileira = 1+(rand() % (max_lugares));
			assento = 1+(rand() % (max_lugares));
			printf("Cliente Preferencial [%d]: Escolhi assento %c%d \n", i, fileira+64, assento);
			while(filme.info[cod].lugar[pp].lugares[fileira-1][assento-1] != 0 && tentativas <= max_tentativas){
				printf("Cliente Preferencial [%d]: Lugar que escolhi ta ocupado, vou escolher outro! \n", i);
				fileira = 1+(rand() % (max_lugares));
				sleep(1);
				assento = 1+(rand() % (max_lugares));	
				printf("Cliente Preferencial [%d]: Escolhi assento %c%d \n", i, fileira+64, assento);	
				tentativas++;
			}
			if (tentativas > max_tentativas){
				printf("Cliente Preferencial [%d]: Tentei muito, mas ta tudo ocupado, bjs tchau! \n", i);
				clientes_comprando --;
				pthread_cond_broadcast(&prefs);
				pthread_cond_broadcast(&legais);
				pthread_cond_broadcast(&chatos);
				pthread_mutex_unlock(&lock);
				break;
			}
				tentativas = 0;
				filme.info[cod].lugar[pp].lugares[fileira-1][assento-1] = 1;
				filme.info[cod].lugar[pp].capacidade++;
				printf("\nCompra realizada com sucesso.\n");		
				printf("\n-----LUGARES FILME %s SESSÃO %d -------\n", filme.nome[cod], pp);
				for (int k = 0; k <max_lugares; k++){
						imprime_fileira(k, pp, cod, y);
					}
				printf("\n--------------------------------------------------\n");
				sleep(1);
				pthread_mutex_unlock(&lock);
				sleep(3); //tempo finalizando compra e saindo terminal
				pthread_mutex_lock(&lock);
				printf("\nCliente Preferencial [%d]: Vou sair do terminal de compra! \n", i);
				clientes_comprando --;
				pthread_cond_broadcast(&prefs);
				pthread_cond_broadcast(&legais);
				pthread_cond_broadcast(&chatos);
				pthread_mutex_unlock(&lock);
				sleep(2);
	}
 return 0;
}



void * cliente_legal (void *arg){
	int i = *((int *) arg);
	int cod, pp, fileira, assento, tentativas = 0;
	int y[max_lugares];
	while (1){
		pthread_mutex_lock(&lock);
		printf("\nCliente Legal [%d]: Vou comprar ingresso! \n", i);
		tem_legal ++;
		while (clientes_comprando >= max_clientes_comprando || tem_pref > 0){
			printf("Tem %d clientes preferenciais e %d clientes legais na fila! \n", tem_pref, tem_legal);
			pthread_cond_wait(&legais, &lock);
		}
		tem_legal--;
		clientes_comprando++;
		cod = 1+(rand() % (max_filmes));
		pp = 1+(rand() % (periodo));
		printf("Cliente Legal [%d]: Escolhi filme %s, sessão %d \n", i, filme.nome[cod], pp);
		while (filme.info[cod].lugar[pp].capacidade == sala_cheia && tentativas <= max_tentativas){
			printf("Cliente Legal [%d]: Sala que eu escolhi ta cheia, vou escolher outro! \n", i);
			cod = 1+(rand() % (max_filmes));
			pp = 1+(rand() % (periodo));
			printf("Cliente Legal [%d]: Escolhi filme %s, sessão %d \n", i, filme.nome[cod], pp);
			tentativas++;
		}
		if (tentativas > max_tentativas){
			printf("Cliente Legal [%d]: Tentei muito, mas ta tudo cheio, bjs tchau! \n", i);
			clientes_comprando --;
			pthread_cond_broadcast(&prefs);
			pthread_cond_broadcast(&legais);
			pthread_cond_broadcast(&chatos);
			pthread_mutex_unlock(&lock);
			break;
		}
		tentativas = 0;
		fileira = 1+(rand() % (max_lugares));
		assento = 1+(rand() % (max_lugares));
		printf("Cliente Legal [%d]: Escolhi assento %c%d \n", i, fileira+64, assento);
		while(filme.info[cod].lugar[pp].lugares[fileira-1][assento-1] != 0 && tentativas <= max_tentativas){
			printf("Cliente Legal [%d]: Lugar que escolhi ta ocupado, vou escolher outro! \n", i);
			fileira = 1+(rand() % (max_lugares));
			sleep(1);
			assento = 1+(rand() % (max_lugares));	
			printf("Cliente Legal [%d]: Escolhi assento %c%d \n", i, fileira+64, assento);			
			tentativas++;
		}
		if (tentativas > max_tentativas){
			printf("Cliente Legal [%d]: Tentei muito, mas ta tudo ocupado, bjs tchau! \n", i);
			clientes_comprando --;
			pthread_cond_broadcast(&prefs);
			pthread_cond_broadcast(&legais);
			pthread_cond_broadcast(&chatos);
			pthread_mutex_unlock(&lock);
			break;
		}
		tentativas = 0;
		filme.info[cod].lugar[pp].lugares[fileira-1][assento-1] = 1;
		filme.info[cod].lugar[pp].capacidade++;
		printf("\nCompra realizada com sucesso.\n");
		printf("\n----- LUGARES FILME %s SESSÃO %d -------\n", filme.nome[cod], pp);
		for (int k = 0; k <max_lugares; k++){
				imprime_fileira(k, pp, cod, y);
			}
		printf("\n--------------------------------------------------\n");
		sleep(1);
		pthread_mutex_unlock(&lock);
		sleep(3); //tempo finalizando compra e saindo terminal
		pthread_mutex_lock(&lock);
		printf("\nCliente Legal [%d]: Vou sair do terminal de compra! \n", i);
		clientes_comprando --;
		pthread_cond_broadcast(&prefs);
		pthread_cond_broadcast(&legais);
		pthread_cond_broadcast(&chatos);
		pthread_mutex_unlock(&lock);
		sleep(2);
	}
 return 0;
}

void * cliente_chato (void *arg){
	int i = *((int *) arg);
	int cod, pp, fileira, assento, tentativas = 0;
	int y[max_lugares];
	while (1){
		pthread_mutex_lock(&lock);
		printf("\nCliente Chato [%d]: Vou comprar ingresso! \n", i);
		while (clientes_comprando >= max_clientes_comprando || tem_pref > 0 || tem_legal > 0){
			printf("Tem %d clientes preferenciais e %d clientes legais na fila e alguns chatos insignificantes! \n", tem_pref, tem_legal);
			pthread_cond_wait(&chatos, &lock);
		}
		clientes_comprando++;
		cod = 1+(rand() % (max_filmes));
		pp = 1+(rand() % (periodo));
		printf("Cliente chato [%d]: Escolhi filme %s, sessão %d \n", i, filme.nome[cod], pp);
		while (filme.info[cod].lugar[pp].capacidade == sala_cheia && tentativas <= max_tentativas){
			printf("Cliente chato [%d]: Sala que eu escolhi ta cheia, vou escolher outro! \n", i);
			cod = 1+(rand() % (max_filmes));
			pp = 1+(rand() % (periodo));
			printf("Cliente chato [%d]: Sala que eu escolhi ta cheia, vou escolher outro! \n", i);
			tentativas++;
		}
		if (tentativas > max_tentativas){
			printf("Cliente chato [%d]: Tentei muito, mas ta tudo cheio, bjs tchau! \n", i);
			clientes_comprando --;
			pthread_cond_broadcast(&prefs);
			pthread_cond_broadcast(&legais);
			pthread_cond_broadcast(&chatos);
			pthread_mutex_unlock(&lock);
			break;
		}
		tentativas = 0;
		fileira = 1+(rand() % (max_lugares));
		assento = 1+(rand() % (max_lugares));
		printf("Cliente chato %d: Escolhi assento %c%d \n", i, fileira+64, assento);
		while(filme.info[cod].lugar[pp].lugares[fileira-1][assento-1] != 0 && tentativas <= max_tentativas){
			printf("Cliente chato %d: Lugar que escolhi ta ocupado, vou escolher outro! \n", i);
			fileira = 1+(rand() % (max_lugares));
			assento = 1+(rand() % (max_lugares));	
			printf("Cliente chato %d: Escolhi assento %c%d \n", i, fileira+64, assento);		
			tentativas++;
		}
		if (tentativas > max_tentativas){
			printf("Cliente chato %d: Tentei muito, mas ta tudo ocupado, bjs tchau! \n", i);
			clientes_comprando --;
			pthread_cond_broadcast(&prefs);
			pthread_cond_broadcast(&legais);
			pthread_cond_broadcast(&chatos);
			pthread_mutex_unlock(&lock);
			break;
		}
		tentativas = 0;
		filme.info[cod].lugar[pp].lugares[fileira-1][assento-1] = 1;
		filme.info[cod].lugar[pp].capacidade++;
		printf("\nCompra realizada com sucesso.\n");
		printf("\n-----LUGARES FILME %s SESSÃO %d -------\n", filme.nome[cod], pp);
		for (int k = 0; k <max_lugares; k++){
				imprime_fileira(k, pp, cod, y);
			}
		printf("--------------------------------------------------\n");
		sleep(1);
		pthread_mutex_unlock(&lock);
		sleep(4); //tempo finalizando compra e saindo do terminal
		pthread_mutex_lock(&lock);
		printf("\nCliente chato %d: Vou sair do terminal de compra! ALELUIA \n", i);
		clientes_comprando --;
		pthread_cond_broadcast(&prefs);
		pthread_cond_broadcast(&legais);
		pthread_cond_broadcast(&chatos);
		pthread_mutex_unlock(&lock);
		sleep(2);
	}
 return 0;
}



int main (){
int i, j, k, s;
char titulo[n]; /*variavel que guarda a primeira linha do arquivo texto de entrada (filmes.txt)*/

/*Primeiro irei preencher a matriz dos lugares disponiveis com 0, que siginifica que todos estao disponiveis a principio*/
for(k=1;k<=max_filmes;k++)
for(s=1;s<=periodo;s++){
	filme.info[k].lugar[s].capacidade = 0;
for(i=0;i<max_lugares;i++)
	for(j=0;j<max_lugares;j++)
		filme.info[k].lugar[s].lugares[i][j] = 0;
}

FILE* arq;
arq = fopen("filmes.txt", "r");
	
if(arq==NULL){
	printf("arquivo invalido\n");
	return 0;	
}

else{
 /*Aqui lemos todas informacoes do arquivo de entrada e salvamos na struct*/ 
fscanf(arq, "%[^\n]s", titulo);

	for(i=1;i<=max_filmes;i++){	
		fscanf(arq, "%d		%[^\t]		%d		", &filme.cod[i], filme.nome[i], &filme.sala[i]);
		for(j=1;j<=periodo;j++){
			fscanf(arq, "%s		", filme.info[i].sessao[j]);
		}			
	}
}


fclose(arq);

/*Primeiro limpamos a tela*/
system("clear||cls");
/*Entao imprimimos o titulo do programa  e os filmes disponiveis*/ 
printf("\n|------------------ CINEMINHA -------------------|\n|    ");

printf("\nAqui estao os filmes disponiveis:\n");
printf("\n%s\n", titulo);
Imprime(); //mudar codigos!
//Compra(&filme);
//printf("\nPressione <enter> para continuar\n");
//getchar();

pthread_mutex_init(&lock, NULL);
pthread_cond_init(&prefs, NULL);
pthread_cond_init(&legais, NULL);
pthread_cond_init(&chatos, NULL);

pthread_t pre[numero_preferenciais], leg[numero_legais], cha[numero_chatos];
srand(time(NULL));
int *id;
for (i = 0; i < numero_preferenciais ; i++) {
	id = (int *) malloc(sizeof(int));
	*id = i;
	pthread_create(&pre[i], NULL, cliente_preferencial, (void *) (id)); //cria thread de professores
}   
for (i = 0; i < numero_legais ; i++) {
	id = (int *) malloc(sizeof(int));
	*id = i;
	pthread_create(&leg[i], NULL, cliente_legal, (void *) (id)); //cria threads de funcionarios
}
for (i = 0; i < numero_chatos ; i++) {
	id = (int *) malloc(sizeof(int));
	*id = i;
	pthread_create(&cha[i], NULL, cliente_chato, (void *) (id)); //cria threads de alunos
}


pthread_join(pre[0],NULL);
pthread_join(leg[0],NULL);
pthread_join(cha[0],NULL);
/*FIM*/
return 0;

}	
