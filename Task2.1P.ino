#include <BH1750FVI.h> // GY-30 light sensor
#include <DHT.h> // HD11 sensor
#include <Wire.h> // I2C communication for GY-30 sensor
#include <WiFiNINA.h> // Wifi communicatino for Nano 33 IoT
#include "secrets.h" // Used to include Wi-Fi and ThingSpeak credentials
#include "ThingSpeak.h" // Communication with thing speak

unsigned long channelID = CHANNEL_ID;         // Use the channelID value from secrets.h

// DHT11 settings
#define DHTPIN 2      // Pin for DHT11 data
#define DHTTYPE DHT11 // Define sensor type
DHT dht(DHTPIN, DHTTYPE); // Initialiser for DHT sensor

// GY-30 (BH1750) settings
#define GY30_ADDRESS 0x23 // I2C address
BH1750FVI lightMeter(BH1750FVI::k_DevModeContLowRes);    // Creates a BH1750 object for light sensor control

WiFiClient client; // Wifi client object

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize DHT11 sensor
  dht.begin();

  // Initialize GY-30 light sensor
  lightMeter.begin();

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Uses Wi-Fi credentials from secrets.h

  // Loop until Wi-Fi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");

  // Initialize ThingSpeak
  ThingSpeak.begin(client); // Using the thingSpeak service through the WiFi client
}

void loop() {
  // Read temperature and humidity from DHT11
  int humidity = dht.readHumidity();
  int temperature = dht.readTemperature();
  
  // Read light intensity from GY-30 sensor
  float lux = lightMeter.GetLightIntensity();

  // Check if the readings from DHT11 are valid
  if (isnan(humidity) || isnan(temperature)) { // Checks both humidity and temperature values for NaN (not-a-number)
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Display the sensor values on the Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  // Display the light intensity on the Serial Monitor
  Serial.print("Light Intensity: ");
  Serial.print(lux);
  Serial.println(" lx");

  // Send the data to ThingSpeak
  ThingSpeak.setField(1, temperature);       // Field 1: Temperature
  ThingSpeak.setField(2, humidity);          // Field 2: Humidity
  ThingSpeak.setField(3, lux);               // Field 3: Light Intensity
  
  // Write the data to ThingSpeak
  int status = ThingSpeak.writeFields(channelID, WRITE_API_KEY);  //ChannelID and WRITE_API_KEY from secrets.h

  // Checks if data has been successfully sent to ThingSpeak
  if (status == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.println("Error sending data to ThingSpeak.");
  }

  // Waits 60 seconds before sending the next set of data
  delay(60000);
}
