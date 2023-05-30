#define BLYNK_PRINT Serial
#include 
#include 
#include 
#include "DHT.h"
//DHT sensor information 
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321
#define DHTPIN 27 // Digital pin connected to the DHT sensor
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.
//define input pins and outputs
int soil_sensor = 34; //define analog input pin number connected to moisture sensor
int output_value ;//define as output
int moisturelevel;//define as output
int notified = 0; //define notifed as 0
int timedelay= 60000L; //set timer to run get data once every minute or 60,000 miliseconds
//set minimum values for plant
int min_moisture =20;
int min_temperature =75;
int min_humidity =60;
// You should get Auth Token in the Blynk App.
char auth[] = "Auth_Token_Here";
// Your WiFi credentials.
char ssid[] = "Wifi_Network_Here";
char pass[] = "Wifi_Password_Here";
BlynkTimer timer;
// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void Sensors () //main function to read sensors and push to blynk 
{
 output_value = analogRead(soil_sensor);// Read analog signal from soil_sensor and define as output_value
 //Map output_vlaue from min,max values to 100,0 and constrain between 0,100
 //Use sample code and serial monitor to find min and max values for individual sensor and soil type for better calibration
 moisturelevel = constrain ( map(output_value, 1000, 4095, 100, 0), 0, 100);
 float h = dht.readHumidity(); // Read humidity 
 float t = dht.readTemperature(); // Read temperature as Celsius (the default)
 float f = dht.readTemperature(true); // Read temperature as Fahrenheit (isFahrenheit = true) 
 // Compute heat index in Fahrenheit (the default)
 float hif = dht.computeHeatIndex(f, h);
 // Check if any reads failed and exit early (to try again).
 if (isnan(h) || isnan(t) || isnan(f)) {
 Serial.println(F("Failed to read from DHT sensor!"));
  return;
 }
 //This connects vales to virtual pins defined in the widgets in the Blynk app
  Blynk.virtualWrite(V5, moisturelevel );// Send moisture level to virtual pin 5
  Blynk.virtualWrite(V6,f);// Send temperature to virtual pin 6
  Blynk.virtualWrite(V7,h);// Send humidity to virtual pin 7 
  Blynk.virtualWrite(V8,hif);// Send heat index to virtual pin 8
 if (notified==0)
{
 if (moisturelevel <= min_moisture) // If moisturelevel is equal to or below min value
 {
  Blynk.email("Email_Here", "Plant Monitor", "Water Plant!"); // Send email to water plant 
 }
 delay (15000); // Blynk emails must be 15 seconds apart. Delay 15000 millisecons 
 if (f <= min_temperature) // If temperature is equal to or below min value 
 {
  Blynk.email("Email_Here", "Plant Monitor", "Temperature Low!"); // Send email that temperature is low
 }
 delay (15000); // Blynk emails must be 15 seconds apart. Delay 15000 millisecons 
 if (h <= min_humidity) // If humidity is equal to or below min value 
 {
  Blynk.email("Emial_Here", "Plant Monitor", "Humidity Low!"); // Send email that humidity is low
 }
notified = 1;
timer.setTimeout(timedelay *5, resetNotified); // multipy timedelay by number of minutes wanted between repeat warning emails 
}
}   
void resetNotified() //function called to reset email frequency  
{
  notified = 0;
}
void setup()
{
  Serial.begin(9600); // Debug console
  Blynk.begin(auth, ssid, pass); // connect to blynk 
  timer.setInterval(timedelay, Sensors); // Setup a function to be called every minute or what timedelay is set to
  dht.begin(); //run DHT sensor
}
//Void loop should only contain blynk.run and timer
void loop()
{
  Blynk.run(); // Run blynk 
  timer.run(); // Initiates BlynkTimer
}