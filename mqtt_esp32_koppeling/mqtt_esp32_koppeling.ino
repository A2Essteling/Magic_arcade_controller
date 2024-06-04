#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

// Wifi gegevens
const char* ssid     = "deproject";
const char* password = "projectP4";

char *server = "broker.emqx.io";
// char *server = "broker.hivemq.com";
int port = 1883;

int pinLED = 26;
int pinTemperatuurSensor = 18;

int temperatuur_room;
int inputMQTT = 0;
char data[40];
String resOutput = "";

void wifiInit() {
    Serial.print("Verbinding met ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
        delay(500);  
    }
    Serial.println("");
    Serial.println("Verbonden met WiFi");
    Serial.println("Addresse IP: ");
    Serial.println(WiFi.localIP());
    delay(10000);
  }

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Bericht ontvangen [");
  Serial.print(topic);
  Serial.print("] ");

  char payload_string[length + 1];
  
  int res;

  memcpy(payload_string, payload, length);
  payload_string[length] = '\0';
  res = atoi(payload_string);
  
  inputMQTT = res;

  resOutput = "";
  
  for (int i=0; i<length; i++) {
    resOutput = resOutput + (char)payload[i];
  }
  Serial.println();
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Verbinding met MQTT...");

    if (mqttClient.connect("arduinoClient")) {
      Serial.println("CONNECTED");

      mqttClient.subscribe("Input/01");
      
    } else {
      Serial.print("NIET GELUKT, ERROR :");
      Serial.print(mqttClient.state());
      Serial.println(" Opnieuw proberen...");
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(pinLED,OUTPUT);
  Serial.begin(115200);
  delay(10);
  wifiInit();
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop()
{
   if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  Serial.print("String: ");
  Serial.println(resOutput);

  if(inputMQTT == 0)
  {
  digitalWrite(pinLED,LOW);
  }
  else if (inputMQTT == 1)
  {
  digitalWrite(pinLED,HIGH);
  }

  temperatuur_room = analogRead(pinTemperatuurSensor);
  Serial.print("Temperatuur in de kamer: ");
  Serial.println(temperatuur_room);

  sprintf(data, "Waarde Temperatuur in de kamer: %d ", temperatuur_room);
  mqttClient.publish("Output/01",data);
  delay(5000);
}
