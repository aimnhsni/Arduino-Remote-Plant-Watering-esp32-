#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include "ThingSpeak.h"
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <Servo.h>
#define DHTPIN 21 //The digital pin that is connected to the DHT sensor
#define DHTTYPE DHT11// the dht type
unsigned long myChannelId = 1410033;
const char* myWriteAPIKey = "PA7YQAI4HRH0OV28";

DHT dht(DHTPIN, DHTTYPE);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "euQSuFwuWyYv0_4UGNoMlIumAyTXPf18";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "hasni_wifi@unifi";
char pass[] = "pt8478h@sni";

int sensorPin = 2;   // Input pin for the Flame Sensor
int sensorValue = 0;  // Variable to store the value coming from the flame sensor
int moistureLevel;
int moisturePercentage;
const int moistureLevelSensor = A0;

Servo servo;
WiFiClient client;
BLYNK_WRITE(V3) {

servo.write(param.asInt());
}
BlynkTimer timer;


void sendSensor1()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
    
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

}

void sendSensor2()
{
  moistureLevel = analogRead(moistureLevelSensor);
  moistureLevel = map(moistureLevel,0,4000,0,20);
  


  Blynk.virtualWrite(V8, moistureLevel);

}

void setup()
{
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  servo.attach(2);
  pinMode(2, INPUT);
  dht.begin();

    // Setup a function to be called every second
  timer.setInterval(2000L, sendSensor1);
  timer.setInterval(5000L, sendSensor2);
  
 ThingSpeak.begin(client);

}

void loop()
{
  Blynk.run();


  delay(2000);
  // read sensor readings

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check

  if (isnan(h) || isnan(t) ){

    Serial.println("Failed to read from DHT sensor!");

    
  }

  //compute heat index (in Celcius)

  float hic = dht.computeHeatIndex(t, h, false);

  //print out to serial

  Serial.println();

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% Temperature: "));
  Serial.print(t);
  Serial.print(F("C "));



  moisturePercentage = (100-(analogRead(moistureLevelSensor)/4095)*100  );
  Serial.print("Soil Moisture is  = ");
  Serial.print(moisturePercentage);
  Serial.println("%");
  Blynk.virtualWrite(V8, moisturePercentage);
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

ThingSpeak.writeField(myChannelId, 1,
moisturePercentage, myWriteAPIKey);

 delay(100);


  

}
