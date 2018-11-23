#include "ThingSpeak.h"//Libreria de la nube
#include <ESP8266WiFi.h> // Librería de la tarjeta NODEMCU
#include <FirebaseArduino.h> // Librería para enviar datos a Firebase
#include <DHT.h> // Librería del sensor de humedad y temperatura
#include <SDS011.h> //Libreria del sensor de material particulado
#include "MQ135.h" // Libreria Sensor MQ135


// Set these to run example.
#define FIREBASE_HOST "calidad-del-aire-7ec9b.firebaseio.com"
#define FIREBASE_AUTH "k2MwdvGS7KVwojlNBRx1uWmMmg6fXf3BmfuN6tWd"
//#define WIFI_SSID "FUNCIONARIO"
//#define WIFI_PASSWORD "Nduthr29*Ecali20%Mbog16="
#define WIFI_SSID "APRENDIZ"
#define WIFI_PASSWORD "Dvbew89*UBuc70%RCuc95="



//Definicion de conexiones que traen información
#define DHTPIN 13     // Pin al cual conectamos el pin 2 del sensor de humedad (de izquierda a derecha)
#define D1 5          // Pin al cual conectamos el pin TXD del sensor de material particulado
#define D2 4          // Pin al cual conectamos el pin RXD del sensor de material particulado
#define DHTTYPE DHT22   // Define el tipo de sensor de temperatura y humedad DHT 22  (AM2302), AM2321
#define ANALOGPIN A0    //  Define Analog PIN on Arduino Board
#define RZERO 197.90    //  Define RZERO Calibration Value 206.85 
MQ135 gasSensor = MQ135(ANALOGPIN);

//Variables
float p10,p25; //Defiicion de las variables que guardaran el valor del PM10 Y PM2.5
float h, t;  //Defiicion de las variables que guardaran el valor de la humedad y la temperatura
int error; //Error del sensor de mateial particulado
int sent = 0;
int lp=0;
int lp_time=30;
int disconnected = 1;

// ThingSpeak Settings
const int channelID = 536198;
String apiKey = "3WERWBY1KPH7QZK1"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";

SDS011 my_sds;//Define el objeto SDS
DHT dht(DHTPIN, DHTTYPE); //Inicialización del sensor de temperatura y humedad

void setup() {
  Serial.begin(115200);
  dht.begin(); //Empieza la medición de la temperatura y humedad
  my_sds.begin(D1,D2); //Empieza la medición de material particulad
  
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
}
void reconnectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}
void onDisconnected(const WiFiEventStationModeDisconnected& event)
    {
        disconnected = 1;
        reconnectWifi();
    }
void loop() {

  if (disconnected == 1 && WiFi.status() == WL_CONNECTED){
      Serial.print(String("[WIFI] IP: "));
      Serial.println(WiFi.localIP());
      disconnected = 0;
  }
   
  delay(1000*20); // Leer la temperatura o humedad toma cerca de 250 milisegundos
  float ppm = gasSensor.getPPM(); // Variable de medición CO2      
  h = dht.readHumidity();   //Lectura de la humedad
  t = dht.readTemperature();   // Lectura de la temperatura en °C
  error = my_sds.read(&p25,&p10);// Lee los valores de pm 2.5y pm10, devuelve el valor de 0 si se leyeron nuevos valores y 1 si no hay nuevos valores
  sendPM(p10,p25,t,h, ppm);
 
 
        StaticJsonBuffer<200> jsonBuffer1;
        JsonObject& root1 = jsonBuffer1.createObject();
        JsonObject& time1 = root1.createNestedObject("timestamp");
        root1["temperatura"] = t;
        time1[".sv"] = "timestamp";
        // append a new value to /logDHT
        //String name1 = Firebase.push("/CIDE/sensores/temperatura", root1);
        String name1 = Firebase.push("/nodocazuca/sensores/temperatura", root1);
        
        StaticJsonBuffer<200> jsonBuffer2;
        JsonObject& root2 = jsonBuffer2.createObject();
        JsonObject& time2 = root2.createNestedObject("timestamp");
        root2["humedad"] = h;
        time2[".sv"] = "timestamp";
        // append a new value to /logDHT
        //String name2 = Firebase.push("/CIDE/sensores/humedad", root2);
        String name2 = Firebase.push("/nodocazuca/sensores/humedad", root2);
        
        StaticJsonBuffer<200> jsonBuffer3;
        JsonObject& root3 = jsonBuffer3.createObject();
        JsonObject& time3 = root3.createNestedObject("timestamp");
        root3["p10"] = p10;
        time3[".sv"] = "timestamp";
        // append a new value to /logDHT
        //String name3 = Firebase.push("/CIDE/sensores/p10", root3);
        String name3 = Firebase.push("/nodocazuca/sensores/p10", root3);
        
        StaticJsonBuffer<200> jsonBuffer4;
        JsonObject& root4 = jsonBuffer4.createObject();
        JsonObject& time4 = root4.createNestedObject("timestamp");
        root4["p25"] = p25;
        time4[".sv"] = "timestamp";
        // append a new value to /logDHT
        //String name4 = Firebase.push("/CIDE/sensores/p25", root4);
        String name4 = Firebase.push("/nodocazuca/sensores/p25", root4);
        
        StaticJsonBuffer<200> jsonBuffer5;
        JsonObject& root5 = jsonBuffer5.createObject();
        JsonObject& time5 = root5.createNestedObject("timestamp");
        root5["co2"] = ppm;
        time5[".sv"] = "timestamp";
        // append a new value to
        //String name5 = Firebase.push("/CIDE/sensores/co2", root5);        
        String name5 = Firebase.push("/nodocazuca/sensores/co2", root5);

        /////Envia 0 si no hay errores y se actualiza a 1 si hay uno
        //Firebase.setInt("/CIDE/error", 0); 
        Firebase.setInt("/nodocazuca/error", 0);  

///////Error folder for error history //////////
        //StaticJsonBuffer<200> jsonBufferE;
        //JsonObject& rootE = jsonBufferE.createObject();
        //JsonObject& timeE = rootE.createNestedObject("timestamp");
       // rootE["co2"] = "error";
       // timeE[".sv"] = "timestamp";
        
            Serial.print(h);
            Serial.print(" , ");
            Serial.print(t);
            Serial.print(" , ");
            Serial.print(p10);
            Serial.print(" , ");
            Serial.print(p25);
            Serial.print(" , ");
            Serial.println(ppm);
          
            delay(1000*4); 
   
// handle error
            if (Firebase.failed()) {
              /////Envia 0 si no hay errores y se actualiza a 1 si hay un error
              //Firebase.setInt("/CIDE/error", 1);
              Firebase.setInt("/nodocazuca/error", 1);  
              Serial.print("Error enviando datos:");
              Serial.println(Firebase.error());
              delay(1000*4);
              return;
}
delay(1000*4);                         
  }
void sendPM(float pm10, float pm25, float t, float h, float ppm)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(pm10);
   postStr += "\r\n\r\n";
   postStr += "&field2=";
   postStr += String(pm25);
   postStr += "\r\n\r\n";
   postStr += "&field3=";
   postStr += String(t);
   postStr += "\r\n\r\n";
   postStr += "&field4=";
   postStr += String(h);
   postStr += "\r\n\r\n";
   postStr += "&field5=";
   postStr += String(ppm);
   postStr += "\r\n\r\n";   
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000*28);
   }
   sent++;
 client.stop();
}
