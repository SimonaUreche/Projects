#include <SoftwareSerial.h>  

int motorInput1 = 5;  // Motor 1 (direcție)
int motorInput2 = 6;  // Motor 1 (direcție)
int motorInput3 = 3;  // Motor 2 (direcție)
int motorInput4 = 9;  // Motor 2 (direcție)

// Definirea pinii pentru comunicarea Bluetooth
SoftwareSerial BTSerial(10, 11);  // RX, TX (Conectare la modulul HC-05)

int data[2];  // Array pentru datele primite

void setup() {
  // Configurare pinii pentru motoare
  pinMode(motorInput1, OUTPUT);
  pinMode(motorInput2, OUTPUT);
  pinMode(motorInput3, OUTPUT);
  pinMode(motorInput4, OUTPUT);
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, LOW);
  
  Serial.begin(9600);  //Serial Monitor
  BTSerial.begin(9600);  //Conexiune Bluetooth
}

void loop() {
  // Dacă există date disponibile pe portul serial Bluetooth
  if (BTSerial.available() >= 2) {
    data[0] = BTSerial.read();  // Citește valoarea de pe axa X
    data[1] = BTSerial.read();  // Citește valoarea de pe axa Y

    Serial.print("X: ");
    Serial.print(data[0]);
    Serial.print(" Y: ");
    Serial.println(data[1]);

    // Mișcare înapoi (Spate)
    if (data[1] > 160) {  
      digitalWrite(motorInput1, LOW);
      digitalWrite(motorInput2, HIGH);
      digitalWrite(motorInput3, LOW);
      digitalWrite(motorInput4, HIGH);
      Serial.println("SPATE");
    } 
    // Mișcare înainte (Fata)
    else if (data[1] < 140) {  
      digitalWrite(motorInput1, HIGH);
      digitalWrite(motorInput2, LOW);
      digitalWrite(motorInput3, HIGH);
      digitalWrite(motorInput4, LOW);
      Serial.println("FATA");
    } 
    // Mișcare dreapta
    else if (data[0] < 50) {  
      digitalWrite(motorInput1, LOW);  
      digitalWrite(motorInput2, 150);
      digitalWrite(motorInput3, 150);  
      digitalWrite(motorInput4, LOW);
      Serial.println("DREAPTA");
    }    
     // Mișcare stânga
    else if (data[0] > 135) {  
      digitalWrite(motorInput1, 150);  
      digitalWrite(motorInput2, LOW);
      digitalWrite(motorInput3, LOW);  
      digitalWrite(motorInput4, 150);
      Serial.println("STANGA");
    }
    // Oprire
    else {  
      digitalWrite(motorInput1, LOW);
      digitalWrite(motorInput2, LOW);
      digitalWrite(motorInput3, LOW);
      digitalWrite(motorInput4, LOW);
      Serial.println("Stop");
    }
  }
}
