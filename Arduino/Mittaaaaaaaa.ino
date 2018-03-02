#include <WiFi101.h>
#include <SPI.h>
#include <SimpleDHT.h>

//=====================================================================================================wifi================
const char* ssid = "BC-PUBLIC";
const char* password = "freenetBC";
WiFiClient client;
char server[] = "api.ruonavaara.fi";

//=========================================================================================================DHT===================
int pinDHT22 = 5;
SimpleDHT22 dht22;
//==================================================================================================MH-Z19======================
byte com[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte return1[9];
int concentration1;

//=================================================================================================funktio=================
String apiFormat (String sensorId, float temp, float hum, int co2) {
  String json = "{  \"sensorId\": \"";
  json += sensorId;
  json += "\",  \"data\": {    \"temperature\": ";
  json += temp;
  json += ",  \"humidity\": ";
  json += hum;
  json += ",  \"CO2\": ";
  json += co2;
  json += " }}";
  return json;
}
//====================================================================================================funktio=================
void httpRequest( String area, String data){
  if (client.connect(server, 80 )) {
    Serial.println("Yhteys muodostettu palvelimeen");
    client.println(String("POST /iot/area/") + area + String(" HTTP/1.1"));
    client.println("Host: api.ruonavaara.fi");
    client.println("Content-Type: application/json");
    client.println(String("Content-Length: ") + data.length());
    client.println("Cache-Control: no-cache");
    client.println();
    client.println(data);
    client.flush(); // varmuuden vuoksi
  }
  delay(5000);
  while (client.available()) {
    char c = client.read();
   
  }
}
//===================================================================================================setup====================
void setup() {
  Serial1.begin(9600);
  Serial.begin(115200);

 Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin (ssid, password);

  while(WiFi.status() != WL_CONNECTED){
  delay(50);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println ("WiFi connected");

}
//======================================================================================================loop==================
void loop() {
   Serial.println("====================================================================");
  //================================================================================================DHT22:=================
  float temperature = 0;
  float humidity = 0;
  byte data[40] = {0};
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(pinDHT22, &temperature, &humidity, data)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err); delay(2000);
    return;
  }

  //=====================================================================================================MH-Z19:================
  while (Serial1.available() > 0) {
    Serial1.read();
  }
  Serial1.write(com, 9);
  Serial1.readBytes(return1, 9);
  concentration1 = return1[2] * 256 + return1[3];

  // muoostetaan API:n vaatima JSON-data
  String data2 = apiFormat("test", temperature, humidity, concentration1);

  // lähetetään  request APIin
  httpRequest("462", data2);
  //============================================================================================================print==============
  Serial.print("Lämpötila: ");
  Serial.print((float)temperature); Serial.print(" *C; ");
  Serial.print("kosteus: ");
  Serial.print((float)humidity); Serial.print(" RH%");
  Serial.print(";  Hiilidioksidi: CO2 = ");
  Serial.print(concentration1);
  Serial.println();
  delay(5000);
}
