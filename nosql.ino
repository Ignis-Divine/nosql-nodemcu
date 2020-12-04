#include <ESP8266WiFi.h>  
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
//#include "RestClient.h"
#include "DHT.h"
#define DHTPIN 14   //pin d5= sensor temperatura/humedad
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE,15);
#define ARDUINOJSON_USE_DOUBLE 0
int ledPIN = D1; //= Pin LED y Buzzer mismo pin

//para conexion a red
const char* const ssid = "INFINITUM7580_2.4";
const char* const password = "7Mn5NnrjDC";

//variables de programa
const int btn =16; //D0
float h=0;
float t=0;
String a="";
String m="";
String p="";
int est1=0;
int val1=0;
int ant1=0;
int led =02;
byte mac[6];
char buf[3];
char buff[100];
String txt="";
void setup(){
  //modo de pin

  Serial.begin(115200);
  //establecer parametro wifi
  pinMode(ledPIN , OUTPUT);
  WiFi.begin(ssid, password);
  //iniciar conexion
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Espere conectando...");
  }//Condicion Para Vincular Conexion
  Serial.println("** Conexion Establecida **");
  dht.begin();
}//Setup Parametros Consola y conexion.

//Post's Para Bases de Datos.

void post_temp(double h, double t){
    //creacion de cliente http
    HTTPClient http; 
    //ruta "de api
    http.begin("http://189.141.97.39:5050/nodemcu");
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(macAdd());
    //creacion de json
    StaticJsonBuffer<200> jsonBuffer;
    char json[256];
    JsonObject& root = jsonBuffer.createObject();
    root["humedad"] = h;
    root["temperatura"] = t;
    root.printTo(json, sizeof(json));
    Serial.println(json);
    //envio de solicitud
    int httpCode = http.POST(json);
    http.end(); //Close connection
}//Post Temperatura

void post_alarma(String a, String m,String p){
    //creacion de cliente http
    HTTPClient http; 
    //ruta "de api
    http.begin("http://189.141.97.39:5050/alarma");
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(macAdd());
    //Creacion de json
    StaticJsonBuffer<200> jsonBuffer;
    char json[256];
    JsonObject& root = jsonBuffer.createObject();
    root["Alarma"] = a;
    root["Mensaje"] = m;
    root["tipo"] = p;
    root.printTo(json, sizeof(json));
    Serial.println(json);
    //envio de solicitud
    int httpCode = http.POST(json);
    http.end(); //Close connection
}//Post Alarma

char * macAdd(){
  WiFi.macAddress(mac);
  txt="";
  for (byte i = 0; i < 6; ++i){
    sprintf(buf, "%2X", mac[i]);
    txt += buf;
    if (i < 5) txt += ':';
  }//Formula para conversion de Direccion MAC Address
  snprintf( buff, sizeof(buff)-1, "%s", txt.c_str() );
  buff[sizeof(buff)] = '\0';
  //Serial.println(buff);
  return buff;
}//Conversion de MAC address de NodemCU

void alarma_sen(){
   if(t>30){
      for (int i=1; i <= 5; i++){
      digitalWrite(ledPIN,HIGH);
      delay(700);
      digitalWrite(ledPIN,LOW);
      delay(250);
      }//Termina if temperatura automatica
    a="Alarma Activa";
    m="Temperatura Alta ¡Peligro!";
    p="Automatica";
      post_alarma(a,m,p);  
   } //Datos Subir a Base de Datos
} //Post Alarma Automatica

void alarma_man(){
  if (est1==1){
    for (int i=1; i <= 5; i++){
      digitalWrite(ledPIN,HIGH);
      delay(700);
      digitalWrite(ledPIN,LOW);
      delay(250);
      }//Termina if Boton Alarma Manual
    a="Alarma Activa";
    m="Temperatura Alta ¡Peligro!";
    p="Manual";
      post_alarma(a,m,p);
    } //Datos Subir a Base de Datos
}//Post Alarma Manual

void loop(){
  //enviar datos mientras este conectado
  if (WiFi.status() == WL_CONNECTED){
    h = dht.readHumidity();
    val1= digitalRead(btn);
    t = dht.readTemperature(true);
    t=(t-32);
    t=t/(1.800);
    alarma_sen();
    alarma_man();
    post_temp(h,t);
    digitalWrite(led,HIGH);
    delay(2500);
    digitalWrite(led,LOW);
    delay(2500);  
    if ((val1 == HIGH) && (ant1 == LOW)){
      est1=1;
      delay(10);
    }
    else{
      est1=0;
      delay(10);
    } 
    ant1 = val1;
  }
}
