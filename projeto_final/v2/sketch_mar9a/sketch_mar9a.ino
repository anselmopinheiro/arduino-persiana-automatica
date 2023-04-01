
#include <Ultrasonic.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//o nome das variáveis está relacionado
// com o shield  L298N
//portas sensor de proximidade
const int triggerPin = 11; 
const int echoPin = 12;
Ultrasonic ultrasonic (triggerPin, echoPin);

LiquidCrystal_I2C lcd(0x3F, 16, 2);
//pushbuttons
const int btnMenuPin = 2;
const int btnUpPin = 3;
const int btnDownPin = 4;

//pinout do motor driver
const int in3 =7;
const int in4 =6;
const int enB =5;

const int ldrPin = A0;

int speed; //velocidade do motor
int startApp=1; 
int ldrValue ;
int menuOption, manualUp, manualDown;

bool openedBlind; //true indica cortina aberta 
bool closedBlind; //true indica cortina fechada
bool autoBlind; //true indica modo automatico;

float distance; //sensor- distancia lida
float minDistance; //distancia mínima para desligar motor
float maxDistance; //distancia maxima para desligar motor
long sTime; //tempo que o sinal demora a chegar desde que foi emitido


void setup()
{
  //distancia da persiana ao sensor
  //ajustar estes valores às dimensoes da nossa maquete
  minDistance=8;
  maxDistance=28;

  // pinMode(in1, OUTPUT);
  // pinMode(in2, OUTPUT); 
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT); 
  pinMode(enB, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(btnMenuPin, INPUT);
  pinMode(btnUpPin, INPUT);
  pinMode(btnDownPin, INPUT);
  lcd.init();
  Serial.begin(9600);
}

void loop()
{
  sTime= ultrasonic.timing();
  distance = ultrasonic.convert(sTime, Ultrasonic::CM);
 

  if (startApp==1) {
    opcaoLcd=0;
      lcd.off();
      autoBlind = false; //ao arrancar a persiana fica em modo manual
      speed=200;
      setSpeed (speed);
      startApp=0;

      stopBlind();
  }

  //ler botao modo (alternar entre auto e manual)
  menuOption = digitalRead (btnMenuPin);

  if (menuOption) {
    opcaoLcd++;
    if (opcaoLcd==1) { //ligar lcd
      lcd.backlight();
      lcd.display();
      autoBlind=false; //colocar persianas em modo manual
    }
     else if (opcaoLcd==2) {
      autoBlind=true;//colocar persianas em modo auto
     
    }
    else { //desligar lcd
     opcaoLcd=0;
      lcd.noBacklight();
      lcd.noDisplay();
    }
  }
   

    //obter o valor do sensor de luz
    ldrValue = analogRead(ldrPin);
  if (autoBlind) { //modo automatico
      stopBlind();
      speed=200;
      setSpeed (speed);
      ldrValue = analogRead(ldrPin); 
        if (ldrValue<100 and closedBlind==0 ) {//noite fecha
          checkDistance (0, minDistance, maxDistance);
          Serial.println("Noite");
        }
        else if (ldrValue>100 and openedBlind==0 ) { //dia abre
         checkDistance (1, minDistance, maxDistance);
          Serial.println("Dia");
        }
        else { //só para debug
            // Serial.println("aqui");
             checkDistance (100, minDistance, maxDistance);
        }      
    
  }
  else //modo manual
  { 
    // Serial.println("manual");
    
    //botao para cima
    if (digitalRead(btnUpPin)) {
      // Serial.println("up:");
      checkDistance (1, minDistance, maxDistance);
      delay(100);
      stopBlind();
    }
    //botao para baixo
    if (digitalRead(btnDownPin)) {
      // Serial.println("down:");
      checkDistance (0,minDistance, maxDistance);
      delay(100);
      stopBlind();
    }
//  stopBlind();

  }
  
  //output para o display
  lcd.setCursor(0,0);
  if (autoBlind) {
    lcd.clear();
    lcd.print("auto");
  }
  else {
    lcd.print("manual");
  }
    lcd.setCursor(0,1);
    lcd.print("d:");
    lcd.print(distance);

    lcd.print(" l:");
    lcd.print(ldrValue);
    lcd.print("   ");
    lcd.print(opcaoLcd);
    delay(100);
}


//mover a persiana no sentido pretendido enquando estiver no intervalo
//de distâncias pretendidos (min - max) 
//para (de parar) a persiana se estiver fora do intervalo definido
//direction=sentido da persian (0 baixo 1 cima 100 só para verificar)
void checkDistance (int direction, int min, int max ) {
    //sensor da persiana perto do fecho?
      if(distance<=min) {
        openedBlind=0;
        closedBlind=1;
        semiOpenedBlind=0;
      }
      //sensor da persiana perto da abertura maxima?
      //damos mais 1cm de tolerancia
      else if(distance>=max-1){ {
        openedBlind=1;
        semiOpenedBlind=0;
        closedBlind=0;
      }
      //se a distancia for muito grande
      //há a possibilidade do pano estar em cima do sensor de distancia
      else if (distance>=max+10) { 
        openedBlind=0;
        semiOpenedBlind=0;
        closedBlind=1;
      }
      else {
        openedBlind=0;
        semiOpenedBlind=0;
        closedBlind=0;
      }  

    
      //parar motor se estiver fora do intervalo de distâncias
       if ((openedBlind && direction==1) || (closedBlind && direction==0))   {
        stopBlind();
      }
      else 
      if (direction==1) {
        moveUp();
      }
      else if (direction==0) {
        moveDown();
      }
    Serial.print("direction:");
    Serial.print(direction);
    Serial.print("          distance:");
    Serial.print(distance);
    Serial.print("          openedBlind:");
    Serial.print(openedBlind);
    Serial.print("          closedBlind:");
    Serial.println(closedBlind);
}

//deslocar persiana para cima
void moveUp() {
  //motor B
  delay(200);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

//deslocar persiana para baixo
void moveDown() {
  delay(200);
  //motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

//parar persiana
void stopBlind() {

  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

}

//velocidade da persiana
void setSpeed(int speed) { 
  analogWrite(enB, speed); //velocidade do motor B
} 