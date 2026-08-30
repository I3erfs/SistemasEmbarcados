#include <Wire.h>
enum Estados { 
  Menu,
  Luminosidade,
  Temperatura,
  DesafioBotao,
  Perda,
  Vitoria
};

Estados Estado = Menu; // Começa no menu
byte resultado; 
unsigned long tempoAtual;
unsigned long tempoAnterior;

const int MEU_ENDERECO = 8;

const int buzzerPin = 9;

// --- Variáveis para botão ---
int leitura;
const int botaoPin = 7;
int ultimaLeituraBotao = LOW;
unsigned long ultimoTempoBotao = 0;
int estadoBotao = LOW;
unsigned long delayDebounce = 25;
//unsigned long tempoInicial=0;
int entrada=0;
byte statusBotao = 0;

//--- Variáveis LDR ---
int ldrPin = 0;
int ldrADC;
const long ldrIntervalo = 1000;

// --- Varivais LM35 ---
const int LM35_pin = 1; 
int lmADC = 0;
const long intervaloLm=500;

// --- Variaveis Perda/Vitoria ---
unsigned long tempoInicialBuzzer;
bool buzzerComecou = false;

void setup() {
  Wire.begin(MEU_ENDERECO);     // Inicia como Slave no endereço 8
  Wire.onRequest(enviarDados);  // Registra a função de resposta
  Wire.onReceive(receberComando);
  pinMode(botaoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  tempoAtual = millis();
  switch(Estado){
    case Menu:
      buzzerComecou = false;
      leitura = digitalRead(botaoPin);
      if(leitura!=ultimaLeituraBotao){
        ultimoTempoBotao=millis();
      }
      if(millis()-ultimoTempoBotao>delayDebounce){
        if(leitura!=estadoBotao){
          estadoBotao=leitura;
          if (estadoBotao == HIGH){
            statusBotao = 1;
          }
        }
      }
      ultimaLeituraBotao=leitura;
  	  break;
  
  	case Luminosidade: 
      if (tempoAtual - tempoAnterior >= ldrIntervalo) {
          tempoAnterior = tempoAtual;
          ldrADC = analogRead(ldrPin); // Apenas guarda o valor bruto
      }
  	  break;
    
     case Temperatura:
      if (tempoAtual - tempoAnterior >= intervaloLm) {
          tempoAnterior = tempoAtual;
          lmADC = analogRead(LM35_pin); 
      }
      break;
    
     case DesafioBotao:
      leitura = digitalRead(botaoPin);
      if(leitura!=ultimaLeituraBotao){
        ultimoTempoBotao=millis();
      }
      if(millis()-ultimoTempoBotao>delayDebounce){
        if(leitura!=estadoBotao){
          estadoBotao=leitura;
          if (estadoBotao == HIGH){
            statusBotao = 1;
          }
        }
      }
      ultimaLeituraBotao=leitura;
  	  break;
    
    case Perda:
      if(buzzerComecou == false){
        buzzerComecou = true;
        tempoInicialBuzzer = millis();
      }
          if(millis()- tempoInicialBuzzer<2000){
              digitalWrite(buzzerPin, HIGH);
          }
          else{
            digitalWrite(buzzerPin, LOW);
          }
      break;
    
    case Vitoria:
      if(buzzerComecou == false){
        buzzerComecou = true;
        tempoInicialBuzzer = millis();
      }
          if(millis()- tempoInicialBuzzer<2000){
              digitalWrite(buzzerPin, HIGH);
          }
          else{
            digitalWrite(buzzerPin, LOW);
          }
      break;

    
	}
}
  
  
  void receberComando(int howMany) {
  if (Wire.available()) {
    byte comando = Wire.read();
    Estado = (Estados)comando; // Muda o próprio estado interno
    //resultado = 0;             // Limpa qualquer dado residual do estado anterior
  }
}

// Essa função roda automaticamente quando o Master pede dados
void enviarDados() {
  switch(Estado){
  	case Menu:
    	Wire.write(statusBotao); 
    	statusBotao = 0;
    	break;
    
    case Luminosidade:
    	Wire.write(highByte(ldrADC)); // Envia a metade superior
        Wire.write(lowByte(ldrADC));  // Envia a metade inferior
        break;
    
    case Temperatura:
        Wire.write(highByte(lmADC)); 
        Wire.write(lowByte(lmADC));  
        break;
    case DesafioBotao:
        Wire.write(statusBotao); 
    	statusBotao = 0;
    	break;
    
     default:
        Wire.write(0);
        break;
    
  }
}