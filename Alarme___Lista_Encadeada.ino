#include <LiquidCrystal.h>              // Adiciona a biblioteca para display de LCD
#include <Ultrasonic.h>
#include<stdlib.h>
#define trigger 9                       // Declara que o pino de trigger estará na porta 9
#define echo 8                          // Declara que o pino de echo estará na porta 8
float distancia = 0;                    // Cria a variável para receber o valor da distância
float tempo = 0;                        // Cria a variável para recer o valor do tempo lido do pino 5
const int buzzer = 2;       //PINO DIGITAL EM QUE O BUZZER ESTÁ CONECTADO

LiquidCrystal lcd(12,11, 5, 4, 3, 2);   // Define os pinos que são utilizados para conexão do display
Ultrasonic ultrasonic(trigger,echo);  //INICIALIZANDO OS PINOS

typedef struct fila{       		 // Estrutura do tipo fila
	int tipo_beep;                 //Armazena um novo alarme
	struct fila *prox;   	         //Aponta pro proximo elemento
}TF; 

TF * inicio = NULL;
int flag = 0;
int cont = 0;
int cont_beep = 0;
int cont_espera = 0;
int posicao = 0;

TF * insere(TF * inicio, int elem){
	
	TF * novo = (TF *) malloc(sizeof(TF));	//Guarda na memória o tamanho da minha estrutura(8 bytes);
	novo->tipo_beep = elem;   				// O inteiro da minha estrutura recebe o elemento para inserir

	if(!inicio){				 //1º Caso - Lista vazia
		inicio = novo;
		inicio->prox = NULL;
		return inicio;
	}
	
	if(inicio && !inicio->prox){	//2º Caso - Lista com um unico elemento     1->
		inicio->prox = novo;
		novo->prox = NULL;
		return inicio;
	}
	
	//3º Caso - Lista com mais de um elemento     1 -> 2 -> 3 ->4->5->

	TF * aux = inicio;
	
	while(aux->prox)
		aux= aux->prox;
	
	aux->prox = novo;
	novo->prox = NULL;
	return inicio;
}

TF * retira(TF * inicio){       // Remove o primeiro elemento
	if(!inicio){
		return inicio;
	}
	
	if(inicio && !inicio->prox){
		free(inicio);
		inicio = NULL;
		return inicio;
	}

	TF * aux = inicio;     
	inicio = inicio->prox;
	free(aux);
	return inicio;
}

void imprime(TF* inicio, int posicao){
	if(!inicio)
		return;
 	lcd.setCursor(posicao,1);                   // Posiciona o cursor
	lcd.print("%d->", inicio->tipo_beep);
	return imprime(inicio->prox, posicao+3);
}

void setup() {

  pinMode(trigger, OUTPUT);             // Configura o trigger como saída
  digitalWrite(trigger, LOW);                //Configura o trigger em nível baixo inicialmente
  pinMode(echo, INPUT);                   // Configura o echo como entrada de sinal digital
  lcd.begin(16,2);                                 // Configura e inicia o display como sendo de 16 colunas e 2 linhas
  lcd.clear();                                          // Limpa qualquer informação armazenada em cache
}

void loop() {
  distancia = 0;
  digitalWrite(trigger, HIGH);          // Aciona o trigger por 10 microsegundos
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  tempo = pulseIn(echo, HIGH);          // Lê o tempo em que echo permanece em nivel alto, tempo dado em microsegundos
  tempo = tempo/1000000;                // Converte o tempo para segundos
  distancia = (tempo*340)*50;           // Calcula a distancia com base na equação fornecida no datasheet


if(distancia <= 30 && cont >= 5){
	cont = 0;
	inicio = insere(inicio, cont_beep);
	cont_beep++; 
}
else
	cont += 1;

if(inicio && cont_espera < 15){
	tone(buzzer, 440, (inicio->tipo_beep) * 500);
	noTone(buzzer);
	cont_espera++;
	lcd.clear();                          		// Limpa o display
  	lcd.setCursor(0,0);                   // Posiciona o cursor
  	lcd.print("%d Pessoa", inicio->tipo_beep);        // Escreve no display  a pessoa atual
}
else{
	cont_espera = 0;
	inicio = retira(inicio);
	//cont_beep--;
}
 
  imprime(inicio, posicao);

  delay(1000);                           // Intervalo até a próxima medida
}

