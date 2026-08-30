#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inicializa o display no endereço 0x27 (16 colunas e 2 linhas)
LiquidCrystal_I2C lcd(0x27, 16, 2);

enum Estados {
  Menu,
  Luminosidade,
  Temperatura,
  DesafioBotao,
  Perda,
  Vitoria
};

// --- Variaveis Gerais
const int ENDERECO_SLAVE = 8; // Endereço do Arduino Slave
Estados Estado = Menu;        // Usando o tipo do enum diretamente
unsigned long tempoAnterior = 0;
const unsigned long intervaloBusca = 200; // Consulta o Slave a cada 200 milissegundos
unsigned long tempoInicial;
bool contagemInicio = false;
unsigned long tempoInicio;

// --- Variaveis LDR ---
const long ldrIntervalo = 1000;
const float ldrLSB = 0.00488; //Vref:1023
int lrdADC;
int logInt;
int ldrPwm;
unsigned long tempoAtualLdr;
unsigned long tempoInicialLdr;

// --- Variaveis LM35 ---
const float lmLSB = 0.00107;
//const int LM35_pin = 0;
unsigned long tempoInicialLm = 0;
//unsigned lon tempoAtualLm;
const long intervaloLm = 500;
float temp_inicial;
int leitura = 0;
int pwm = 0;
float vet [10];
float total=0.00;
int i = 0;
int cont = 0;
float V;
float temp_atual;
float dif;
bool primeiraLeitura = false;

// --- Variaveis DesafioBotao
unsigned long tempoInicialB=0;
int contB=0;
const long intervaloB = 40;
unsigned long tempoComecaB = millis();
bool tempoMarcado = false;

// --- Variaveis Vitoria
unsigned long tempoInicialLeds = 0;
int contagem = 0;

void setup() {
  Wire.begin();           // Inicia o barramento I2C como Master
  lcd.init();             // Inicia o display
  lcd.setBacklight(HIGH); // Liga a luz de fundo
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  // Mensagem inicial padrão antes de receber o primeiro dado
  lcd.setCursor(0, 0);
  lcd.print("PRESSIONE O BO");
  lcd.setCursor(0, 1);
  lcd.print("TAO PARA INICIAR");
}

void loop() {
  // Movemos o millis() para fora do switch para evitar o erro de declaração de C++
  // e permitir que todos os estados possam acessar o tempo atual facilmente.
  unsigned long tempoAtual = millis();
  if (contagemInicio == false){
  	contagemInicio = true;
    tempoInicio = millis();
  }
  else{
    if(tempoAtual - tempoInicio >= 60000){
     Estado = Vitoria; 
     lcd.clear(); 
     lcd.setCursor(0, 0);
     lcd.print("   FALHA"); 
     lcd.setCursor(0, 1);
     lcd.print("TENTE NOVAMENTE");
     Wire.beginTransmission(ENDERECO_SLAVE);
     Wire.write(Vitoria); // Envia o valor do estado (1)
     Wire.endTransmission();
    }
  }
  
  switch (Estado) {
    
    case Menu:
      // Solicita dados ao Slave a cada 200ms (sem bloquear o código)
      if (tempoAtual - tempoAnterior >= intervaloBusca) {
        tempoAnterior = tempoAtual;

        Wire.requestFrom(ENDERECO_SLAVE, 1); // Pede 1 byte ao Slave

        if (Wire.available()==1) {
          byte botao = Wire.read(); // Lê o 0 ou 1 enviado pelo Slave
          
          // A checagem do botão agora está dentro do escopo correto
          if (botao == 1) {
            tempoInicial = millis();
            Estado = Luminosidade; // Atualiza a máquina de estados
            
            // Fazemos a atualização da tela exatamente no momento da transição
            // Assim ele apaga e escreve apenas UMA vez, evitando o Flicker (tela piscando).
            lcd.clear(); 
            lcd.setCursor(0, 0);
            lcd.print("LUMINOSIDADE"); 
            Wire.beginTransmission(ENDERECO_SLAVE);
            Wire.write(Luminosidade); // Envia o valor do estado (1)
            Wire.endTransmission();
          }
        }
      }
      break; // Fim do case Menu
      
    case Luminosidade:
      if (tempoAtual - tempoAnterior >= intervaloBusca) {
        tempoAnterior = tempoAtual;

        // Na Luminosidade, o Mestre sabe que precisa pedir 2 bytes (O ADC fatiado)
        Wire.requestFrom(ENDERECO_SLAVE, 2); 

        if (Wire.available() == 2) {
          byte alta = Wire.read();
          byte baixa = Wire.read();

          // Reconstrói o valor de 0 a 1023 combinando os dois bytes
          int ldrADC = word(alta, baixa);
          Serial.println(ldrADC);

          // Agora o Mestre faz todo o processamento matemático pesado:
          float Vin = ldrADC * ldrLSB;
          if (Vin > 0) {
            float Rldr = ((5.0 * 10000.0) / Vin) - 10000.0;
            Serial.println(Rldr);
            if (Rldr > 0) {
              float logaritmo = log10(Rldr);
              Serial.println(logaritmo);
              logInt = logaritmo * 100; 
			  if (logInt < 330) {
  				ldrPwm = map(logInt, 300, 330, 0, 255);
  				ldrPwm = constrain(ldrPwm, 0, 255); 
  				analogWrite(10, ldrPwm);
  				analogWrite(11, 0);   // Garante que o 2º LED apagou se a luz voltar a subir
  				analogWrite(12, 0);   // Garante que o 3º LED apagou
			  } 
			  else if (logInt < 360) {
  				ldrPwm = map(logInt, 330, 360, 0, 255);
  				ldrPwm = constrain(ldrPwm, 0, 255);
  				analogWrite(10, 255); // Trava o LED 1 no máximo
  				analogWrite(11, ldrPwm);
  				analogWrite(12, 0);   // Garante que o 3º LED está apagado
			  } 
			  else if (logInt < 380) {
  				ldrPwm = map(logInt, 360, 380, 0, 255);
  				ldrPwm = constrain(ldrPwm, 0, 255);
  				analogWrite(10, 255); // Trava o LED 1
  				analogWrite(11, 255); // Trava o LED 2
  				analogWrite(12, ldrPwm);
			  } 
			  else {
  				analogWrite(10, 255);
  				analogWrite(11, 255);
  				analogWrite(12, 255);
  				tempoAtualLdr = millis();
  				if (tempoInicialLdr == 0) { // Se acabou de escurecer
     				tempoInicialLdr = tempoAtualLdr;
  				} 
  				else if (tempoAtualLdr - tempoInicialLdr >= 3000) {
                  Estado = Temperatura;
                  lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print("TEMPERATURA"); 
                  Wire.beginTransmission(ENDERECO_SLAVE);
                  Wire.write(Temperatura); // Envia o valor do estado (1)
                  Wire.endTransmission();
  				}
			}
			if (logInt < 380) {
   				tempoInicialLdr = 0; 
			}

            }
          }
        }
      }
      break;
    
    case Temperatura:
      if(tempoAtual-tempoInicialLm>=intervaloLm){
          tempoInicialLm=tempoAtual;

          Wire.requestFrom(ENDERECO_SLAVE, 2); 

          if (Wire.available() == 2) {
            byte alta = Wire.read();
            byte baixa = Wire.read();
            int adcLm = word(alta, baixa); 

            V = adcLm * lmLSB;
            temp_atual = V * 100.0;
            
            if(temp_atual>0){

              if(i<10){
                total=total-vet[i];
                vet[i]=temp_atual;
                total=total+temp_atual;
                i++;
              }
              else{
                i=0;
                dif=temp_atual-vet[i];
                vet[i]=temp_atual;
                total=total+dif;
                i++;
             }
             if(cont<10){
                cont++;
             }
             float media=total/cont;
             Serial.println(media);
             if (!primeiraLeitura && cont > 0) {
                temp_inicial = media;
                primeiraLeitura = true;
              }
             if (primeiraLeitura && media > temp_inicial) {
                float diferenca = media - temp_inicial;
               if (diferenca>=3.00){
                 Estado = DesafioBotao;
                 lcd.clear(); 
                 lcd.setCursor(0, 0);
                 lcd.print("DESAFIO"); 
                 lcd.setCursor(0, 1);
                 lcd.print("BOTAO");
                 Wire.beginTransmission(ENDERECO_SLAVE);
                 Wire.write(DesafioBotao); // Envia o valor do estado (1)
                 Wire.endTransmission();
               }
             }
           }
          }
      	}
        break;
    	
    	case DesafioBotao:
    		if(tempoMarcado==false){
            	tempoComecaB = millis();
              	tempoMarcado = true;
              	contB = 0;
            }
    		else{
              unsigned long tempo=millis();
              if(tempo-tempoComecaB>5000){
                Estado = Vitoria; 
                 lcd.clear(); 
                 lcd.setCursor(0, 0);
                 lcd.print("   FALHA"); 
                 lcd.setCursor(0, 1);
                 lcd.print("TENTE NOVAMENTE");
                 Wire.beginTransmission(ENDERECO_SLAVE);
                 Wire.write(Vitoria); // Envia o valor do estado (1)
                 Wire.endTransmission();
              }
          	}
    		if (tempoAtual - tempoInicialB >= intervaloB) {
        		tempoInicialB = tempoAtual;

        		Wire.requestFrom(ENDERECO_SLAVE, 1); // Pede 1 byte ao Slave

        		if (Wire.available()==1) {
          			byte botao = Wire.read(); // Lê o 0 ou 1 enviado pelo Slave
          
          		if (botao == 1) {
            		contB++;
                  Serial.println(contB);
                }
                  if(contB>2){
                   Estado = Vitoria;
                   lcd.clear(); 
                   lcd.setCursor(0, 0);
                   lcd.print("ESCAPE REALIZADO"); 
                   lcd.setCursor(0, 1);
                   lcd.print("PARABENS");
                   Wire.beginTransmission(ENDERECO_SLAVE);
                   Wire.write(Vitoria); // Envia o valor do estado (1)
                   Wire.endTransmission();
           }
          }
        }
      break;
    
    	case Vitoria:
    	  if(millis()- tempoInicialLeds>1000 && contagem<6){
          if(digitalRead(10)==LOW){
            digitalWrite(10, HIGH);
            digitalWrite(11, HIGH);
            digitalWrite(12, HIGH);
          }
          else{
            digitalWrite(10, LOW);
            digitalWrite(11, LOW);
            digitalWrite(12, LOW);
          }
          contagem++;
          tempoInicialLeds=millis();
        }
    	else{
          if(contagem==6){
          Estado=Menu;
          contagem=0;
          tempoMarcado = false;
          Wire.beginTransmission(ENDERECO_SLAVE);
    	  Wire.write(Menu); 
          Wire.endTransmission();
          contB = 0;
          tempoInicialLdr = 0;
          primeiraLeitura = false; // Permite recalcular a nova temperatura ambiente base
          total = 0.00;            // Zera o somatório da média móvel
          i = 0;                   // Volta o índice do vetor para a posição zero
          cont = 0;
           contagemInicio=0;
          for(int j = 0; j < 10; j++) {
          vet[j] = 0.00;
          }
         }
        }
    	break;
    		

  }
}
