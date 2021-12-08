#include <SPI.h>
#include <Ethernet.h>

#include "Mudbus.h"

Modbus<50,16> Mb;
//Function codes 1(read coils), 3(read registers), 5(write coil), 6(write register)
//signed int Mb.R[0 to 50] or Mb.MW[0 to 50]  (max MB_N_R = 125) and bool Mb.C[0 to 16] or Mb.MB[0 to 16] (max MB_N_C = 128)  
//Port 502 (defined in Mudbus.h) MB_PORT

void setup()
{
  uint8_t mac[]     = { 0x90, 0xA2, 0xDA, 0x00, 0x51, 0x06 };
  uint8_t ip[]      = { 192, 168, 2, 100 };
  uint8_t gateway[] = { 192, 168, 2, 200 };
  uint8_t subnet[]  = { 255, 255, 255, 0 };
  Ethernet.begin(mac, ip, gateway, subnet);
  //Avoid pins 4,10,11,12,13 when using ethernet shield

  delay(5000);
  Serial.begin(9600);
  
  pinMode(7, INPUT);
  pinMode(8, OUTPUT); 
}

void loop()
{
  Mb.Run();

  //Program counter
  Mb.R[7] = millis();
  
  //Analog inputs 0-1023
  Mb.R[0] = analogRead(A0); //pin A0 to Mb.R[0]
  Mb.R[1] = analogRead(A1);
  Mb.R[2] = analogRead(A2);
  Mb.R[3] = analogRead(A3);
  Mb.R[4] = analogRead(A4);
  Mb.R[5] = analogRead(A5);

  //Digital inputs
  Mb.C[7] = digitalRead(7); //pin 7 to Mb.C[7]

  //Digital outputs
  digitalWrite(8, Mb.C[8]); //pin 8 from Mb.C[8]
}
