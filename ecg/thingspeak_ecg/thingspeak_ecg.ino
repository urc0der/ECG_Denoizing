#include <WiFi.h>
#include <HTTPClient.h>
#include <ThingSpeak.h>

// WiFi Credentials
#define WIFISSID "samsung a55"
#define PASSWORD "jingalalahuhu"

// ThingSpeak Settings
#define CHANNEL_ID 3098843
#define WRITE_API_KEY "MLVF614F2YKSLL9D"

// AD8232 Pin Configuration
#define ECG_PIN 36
#define LO_PLUS 22
#define LO_MINUS 23

// Timing Settings
#define UPLOAD_INTERVAL 15000  // Send data every 15 seconds (ThingSpeak free tier limit)
#define ECG_SAMPLE_RATE 100    // Sample ECG every 100ms for display

/****************************************
 * Global Variables
 ****************************************/
WiFiClient client;
unsigned long lastUploadTime = 0;
float ecgAccumulator = 0;
int sampleCount = 0;

/****************************************
 * Helper Functions
 ****************************************/
void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFISSID, PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\nFailed to connect to WiFi");
    Serial.println("Restarting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }
}

void sendToThingSpeak(float ecgValue) {
  // Set the field with ECG value
  ThingSpeak.setField(1, ecgValue);
  
  // Optional: Add more fields if needed
  // ThingSpeak.setField(2, someOtherValue);
  
  // Send data to ThingSpeak
  int responseCode = ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);
  
  if (responseCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
    Serial.print("ECG Value sent: ");
    Serial.println(ecgValue);
  } else {
    Serial.print("Error sending data to ThingSpeak. HTTP error code: ");
    Serial.println(responseCode);
  }
}

// Alternative method using HTTP (if ThingSpeak library has issues)
void sendToThingSpeakHTTP(float ecgValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Construct the URL with API key and field data
    String url = "http://api.thingspeak.com/update?api_key=";
    url += WRITE_API_KEY;
    url += "&field1=";
    url += String(ecgValue, 2);  // 2 decimal places
    
    Serial.print("HTTP Request URL: ");
    Serial.println(url);
    
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
      
      if (response != "0") {
        Serial.println("Data sent to ThingSpeak successfully");
      } else {
        Serial.println("ThingSpeak update failed (returned 0)");
      }
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
    connectWiFi();  // Try to reconnect
  }
}

float readECG() {
  // Check if leads are properly connected
  if((digitalRead(LO_PLUS) == 1) || (digitalRead(LO_MINUS) == 1)) {
    Serial.println("Warning: Leads off or not properly connected");
    return -1;  // Return -1 to indicate lead-off condition
  }
  
  // Read ECG value
  float ecgValue = analogRead(ECG_PIN);
  
  // Convert to voltage (optional - ESP32 ADC is 12-bit, 0-4095)
  // float voltage = (ecgValue / 4095.0) * 3.3;
  
  return ecgValue;
}

/****************************************
 * Setup Function
 ****************************************/
void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("========================================");
  Serial.println("ESP32 ECG Monitor with ThingSpeak");
  Serial.println("========================================");
  
  // Configure AD8232 pins
  pinMode(ECG_PIN, INPUT);
  pinMode(LO_PLUS, INPUT);
  pinMode(LO_MINUS, INPUT);
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  
  Serial.println("System Initialized Successfully!");
  Serial.println("Starting ECG monitoring...");
  Serial.println("----------------------------------------");
}

/****************************************
 * Main Loop
 ****************************************/
void loop() {
  unsigned long currentTime = millis();
  
  // Read ECG value for local monitoring
  float ecgValue = readECG();
  
  if (ecgValue != -1) {  // If leads are connected
    // Display real-time ECG value
    Serial.print("ECG: ");
    Serial.print(ecgValue);
    Serial.print(" | ");
    
    // Calculate average for ThingSpeak upload
    ecgAccumulator += ecgValue;
    sampleCount++;
    
    // Simple peak detection for heart rate (optional)
    static float lastValue = 0;
    if (ecgValue > 2500 && lastValue < 2500) {  // Threshold may need adjustment
      Serial.print(" [PEAK DETECTED] ");
    }
    lastValue = ecgValue;
    
    Serial.println();
  }
  
  // Upload to ThingSpeak at specified interval
  if (currentTime - lastUploadTime >= UPLOAD_INTERVAL) {
    if (sampleCount > 0) {
      float avgECG = ecgAccumulator / sampleCount;
      
      Serial.println("----------------------------------------");
      Serial.print("Uploading average ECG value: ");
      Serial.println(avgECG);
      
      // Use ThingSpeak library method
      sendToThingSpeak(avgECG);
      
      // OR use HTTP method (comment out one)
      // sendToThingSpeakHTTP(avgECG);
      
      // Reset accumulator and counter
      ecgAccumulator = 0;
      sampleCount = 0;
      lastUploadTime = currentTime;
      
      Serial.println("----------------------------------------");
    } else {
      Serial.println("No valid ECG data to upload (check lead connections)");
    }
  }
  
  // Check WiFi connection periodically
  static unsigned long lastWiFiCheck = 0;
  if (currentTime - lastWiFiCheck >= 30000) {  // Check every 30 seconds
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi connection lost. Reconnecting...");
      connectWiFi();
    }
    lastWiFiCheck = currentTime;
  }
  
  delay(ECG_SAMPLE_RATE);  // Sampling delay
}