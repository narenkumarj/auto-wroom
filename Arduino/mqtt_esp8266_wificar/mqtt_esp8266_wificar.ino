/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
// Update these with values suitable for your network.
Servo servo1, servo2;

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";//"test.mosquitto.org";

int PWMA=5;//Right side 
int PWMB=4;//Left side 
int DA=0;//Right reverse 
int DB=2;//Left reverse
int PWMS=6;//Servo



WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


String byteArrayToString(byte byteArray[], int length) {
  String result = "";
  for (int i = 0; i < length; i++) {
    result += char(byteArray[i]);
  }
  return result;
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String str = "";

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    str += char(payload[i]);
  }
  Serial.println();


  Serial.println(str);


  String inputString = str; //"A1FA255B0FB023SA120SB120";

  // Variables to store parsed values
  int valueA, valueFA, valueB, valueFB, valueSA, valueSB;

  // Parse the string and extract values
  parseString(inputString, valueA, valueFA, valueB, valueFB, valueSA, valueSB);

  // Print the parsed values
  Serial.print("A: "); Serial.println(valueA);
  Serial.print("FA: "); Serial.println(valueFA);
  Serial.print("B: "); Serial.println(valueB);
  Serial.print("FB: "); Serial.println(valueFB);
  Serial.print("SA: "); Serial.println(valueSA);
  Serial.print("SB: "); Serial.println(valueSB);
  



  servo1.write(valueSA);
  servo2.write(valueSB);

  analogWrite(PWMA, valueFA); 
  digitalWrite(DA, valueA); 
     
  analogWrite(PWMB, valueFB); 
  digitalWrite(DB, valueB);



}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("tk/sto", "hello world");
      // ... and resubscribe
      client.subscribe("tk/ctrlin");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void parseString(String input, int& A, int& FA, int& B, int& FB, int& SA, int& SB) {
  // Find the position of each letter in the string
  int posA = input.indexOf('A');
  int posFA = input.indexOf('B');
  int posB = input.indexOf('C');
  int posFB = input.indexOf('D');
  int posSA = input.indexOf('E');
  int posSB = input.indexOf('F');

  // Extract substrings between letters and convert them to integers
  A = input.substring(posA + 1, posFA).toInt();
  FA = input.substring(posFA + 1, posB).toInt();
  B = input.substring(posB + 1, posFB).toInt();
  FB = input.substring(posFB + 1, posSA).toInt();
  SA = input.substring(posSA + 1, posSB).toInt();
  SB = input.substring(posSB + 1).toInt();
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);

  servo1.attach(D5);
  servo2.attach(D6);


  pinMode(PWMA, OUTPUT); 
  pinMode(PWMB, OUTPUT); 
  pinMode(DA, OUTPUT); 
  pinMode(DB, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


}
