/****************************************
 * Include Libraries
 ****************************************/
#include <WiFi.h>
#include <PubSubClient.h>

/****************************************
 * Define Constants
 ****************************************/
#define WIFISSID "samsung a55" // Put your Wifi SSID here
#define PASSWORD "jingalalahuhu" // Put your WIFI password here
#define TOKEN "BBUS-vz5pAYNiiKwRxV0X4cZJJamPHhrUcJ" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "mohak12d567" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                           //it should be a random and unique ascii string and different from all other devices

#define VARIABLE_LABEL "ecg" // Assign the variable label for ECG data
#define DEVICE_LABEL "esp32_ecg" // Assign the device label

// AD8232 pins
#define ECG_PIN 36 // ESP32 analog input pin A0(VP) (GPIO36) for the AD8232 output
#define LO_PLUS 22 // Lead-off detection LO+
#define LO_MINUS 23 // Lead-off detection LO-

char mqttBroker[] = "industrial.api.ubidots.com";
char payload[150]; // Increased buffer size for larger payloads
char topic[150];
// Space to store values to send
char str_sensor[10];

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = '\0'; // Correct NULL termination (was using NULL instead of '\0')
  Serial.write(payload, length);
  Serial.println(topic);
}

void reconnect() {
  // Loop until we're reconnected
  int attempts = 0;
  while (!client.connected() && attempts < 5) { // Limit connection attempts
    attempts++;
    Serial.println("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected to Ubidots MQTT Broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(115200);
  
  // Configure AD8232 pins
  pinMode(ECG_PIN, INPUT); // ECG signal
  pinMode(LO_PLUS, INPUT); // Lead-off detection LO+
  pinMode(LO_MINUS, INPUT); // Lead-off detection LO-
  
  // Start WiFi connection
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFISSID, PASSWORD);
  
  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Configure MQTT client
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);
  
  Serial.println("ECG Monitoring System Initialized");
}

void loop() {
  // Check MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Check if leads are properly connected
  if((digitalRead(LO_PLUS) == 1) || (digitalRead(LO_MINUS) == 1)) {
    Serial.println("Leads off or not properly connected");
    delay(500);
    return; // Skip reading if leads are not connected
  }
  
  // Read ECG value
  float ecgValue = analogRead(ECG_PIN);
  
  // Prepare MQTT payload
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label
  
  /* 4 is minimum width, 2 is precision; float value is copied onto str_sensor*/
  dtostrf(ecgValue, 4, 2, str_sensor);
  
  sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor); // Adds the value
  
  // Send data to Ubidots
  Serial.print("ECG Value: ");
  Serial.println(ecgValue);
  Serial.println("Publishing data to Ubidots Cloud");
  client.publish(topic, payload);
  
  delay(100); // Shorter delay for better ECG resolution (typical ECG sampling rate is 250-500Hz)
}