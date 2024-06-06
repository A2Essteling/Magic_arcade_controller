#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi and MQTT broker settings
const char* ssid = "Lasergame";
const char* password = "avans-01";
const char* mqtt_server = "broker.hivemq.com";

// Base string and device ID
const char* base_topic = "magic";
const char* device_id = "01";

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the address and size according to your LCD

// Pin definitions
const int button1Pin = 18;
const int button2Pin = 19;
const int joystickXPin = 34;
const int joystickYPin = 35;
const int joystickButtonPin = 5;

// Function prototypes
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  // Initialize Serial monitor
  Serial.begin(115200);

  // Initialize button pins
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(joystickButtonPin, INPUT_PULLUP);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // Connect to WiFi
  setup_wifi();

  // Setup MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read button and joystick data
  int button1State = digitalRead(button1Pin);
  int button2State = digitalRead(button2Pin);
  int joystickX = analogRead(joystickXPin);
  int joystickY = analogRead(joystickYPin);
  int joystickButtonState = digitalRead(joystickButtonPin);

  // Send button and joystick data to MQTT broker
  String button1Msg = String(button1State);
  String button2Msg = String(button2State);
  String joystickMsgX = String(joystickX);
  String joystickMsgY = String(joystickY);
  String joystickMsgButton = String(joystickButtonState);

  String button1Topic = String(base_topic) + "/" + String(device_id) + "/button1";
  String button2Topic = String(base_topic) + "/" + String(device_id) + "/button2";
  String joystickTopicX = String(base_topic) + "/" + String(device_id) + "/joystickX";
  String joystickTopicY = String(base_topic) + "/" + String(device_id) + "/joystickY";
  String joystickTopicButton = String(base_topic) + "/" + String(device_id) + "/joystickButton";

  client.publish(button1Topic.c_str(), button1Msg.c_str());
  client.publish(button2Topic.c_str(), button2Msg.c_str());
  client.publish(joystickTopicX.c_str(), joystickMsgX.c_str());
  client.publish(joystickTopicY.c_str(), joystickMsgY.c_str());
  client.publish(joystickTopicButton.c_str(), joystickMsgButton.c_str());

  delay(50); // Adjust delay as needed
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(device_id)) {
      Serial.println("connected");
      String lcdTopic = String(base_topic) + "/" + String(device_id) + "/lcd";
      client.subscribe(lcdTopic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  String lcdTopic = String(base_topic) + "/" + String(device_id) + "/lcd";
  if (String(topic) == lcdTopic) {
    lcd.clear();
    int line = 0;
    String lineContent = "";
    for (int i = 0; i < message.length(); i++) {
      if (message[i] == '\n' || lineContent.length() == 15) {
        lcd.setCursor(0, line);
        lcd.print(lineContent);
        line++;
        lineContent = "";
        if (line >= 2) {
          break; // only handle two lines
        }
      } else {
        lineContent += message[i];
      }
    }
    if (line < 2) {
      lcd.setCursor(0, line);
      lcd.print(lineContent);
    }
  }
}
