#include <LoRa.h>
#include <SSD1306.h>
#include "DHTesp.h"

DHTesp dht;
#define SCK 5 // GPIO5 - SX1278's SCK
#define MISO 19 // GPIO19 - SX1278's MISO
#define MOSI 27 // GPIO27 - SX1278's MOSI
#define SS 18 // GPIO18 - SX1278's CS
#define RST 14 // GPIO14 - SX1278's RESET
#define DI0 26 // GPIO26 - SX1278's IRQ (interrupt request)
#define BAND 868E6 // 915E6

unsigned int counter = 0;

SSD1306 display (0x3c, 4, 15);
String rssi = "RSSI -";
String packSize = "-";
String packet;


void setup () {
  pinMode (16, OUTPUT);
  pinMode (2, OUTPUT);
  
  digitalWrite (16, LOW); // set GPIO16 low to reset OLED
  delay (50);
  digitalWrite (16, HIGH); // while OLED is running, GPIO16 must go high
  
  Serial.begin (115200);
  while (! Serial);
  Serial.println ();
  Serial.println ("LoRa Sender Test");
  
  SPI.begin (SCK, MISO, MOSI, SS);
  LoRa.setPins (SS, RST, DI0);
  if (! LoRa.begin (868E6)) {
    Serial.println ("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
// LoRa.receive ();
  Serial.println ("init ok");
  display.init ();
  display.flipScreenVertically ();
  display.setFont (ArialMT_Plain_10);
  delay (1500);
  display.clear ();
  display.setTextAlignment (TEXT_ALIGN_LEFT);
  display.setFont (ArialMT_Plain_10);
  
  display.drawString (0, 0, "Do you see me ");
  delay (1500);
// Show Temp & Humidity
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  dht.setup(13); // data pin 13
}
  
}

void loop () {
  display.clear ();
  display.setTextAlignment (TEXT_ALIGN_LEFT);
  display.setFont (ArialMT_Plain_10);
  
  display.drawString (0, 0, "Do you see meSending packet:");
  display.drawString (90, 0, String (counter));
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  // Show Temp & Humidity
  display.drawString (0, 20, "Temp ");
  display.drawString (30, 20, String (temperature, 1));
  display.drawString (50, 20, " ºC ");
  display.drawString (0, 40, "Humidity ");
  display.drawString (45, 40, String (humidity, 1));
  display.drawString (67, 40, "%");
  display.display ();

  counter ++;
  delay (500); // wait for 500mS
  // Show Temp & Humidity
{
  delay(dht.getMinimumSamplingPeriod());
  
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temperature), 1);
  Serial.print("\t\t");
  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("\t\t");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);

  // Send Temp & Humidity packet
  LoRa.beginPacket ();
  LoRa.print ("Temp ");
  LoRa.print (temperature, 1);
  LoRa.println (" ºC ");
  LoRa.print ("Humidity ");
  LoRa.print (humidity, 1);
  LoRa.print ("%");
  LoRa.endPacket ();
    
}

}


