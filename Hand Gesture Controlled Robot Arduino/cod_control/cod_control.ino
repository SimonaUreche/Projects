#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 mpu; // Senzorul MPU6050
int16_t ax, ay, az;
int16_t gx, gy, gz;

// Definirea modulelor Bluetooth pe pinii 10 și 11
SoftwareSerial BTSerial(10, 11); // RX, TX (Modulul HC-05)

// Variabile pentru datele transmise
int data[2];

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600); //Conexiune Bluetooth
  
  Wire.begin();
  mpu.initialize(); //Initializează senzorul MPU6050
  
  // verific conexiunea cu MPU6050
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected!");
  } else {
    Serial.println("MPU6050 connection failed!");
  }
}

void loop() {

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //valorile de pe mpu
  
  //folosesc valorile de pe axa x si y
  data[0] = map(ax, -17000, 17000, 300, 400); 
  data[1] = map(ay, -17000, 17000, 100, 200); 
  
  BTSerial.write(data[0]);  // Trimite valoarea de pe axa X
  BTSerial.write(data[1]);  // Trimite valoarea de pe axa Y

  Serial.print("X: ");
  Serial.print(data[0]);
  Serial.print(" Y: ");
  Serial.println(data[1]);
  
  delay(100);
}
