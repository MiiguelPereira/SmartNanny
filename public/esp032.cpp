/*
  Rui Santos
  Complete project details at our blog: https://RandomNerdTutorials.com/esp32-esp8266-firebase-bme280-rtdb/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"
#include <DHT.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

#define DHTPIN 18
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Insert your network credentials                                                                //
#define WIFI_SSID "xxxxxxxx"                                                                      //
#define WIFI_PASSWORD "xxxxxxx"                                                                   //
                                                                                                  //
// Insert Firebase project API Key                                                                //
#define API_KEY "xxxxxxxxxxxxxxxxxxxxxxx"                                                         //
                                                                                                  //
// Insert Authorized Email, Corresponding Password, and User ID                                   //
#define USER_EMAIL "xxxxxx@xxxx.com"                                                              //
#define USER_PASSWORD "xxxxxxxxxxxxxx"                                                            //
#define USER_UID "xxxxxxxxxx"                                                                     //
                                                                                                  //
// Insert RTDB URLefine the RTDB URL                                                              //
#define DATABASE_URL "https://xxxxxxxxxxxxxx"                                                     //
                                                                                                  //
String phoneNumber = "0000000000000";                                                             //
String apiKey = "000000000";                                                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;

// Database child nodes
String tempPath;
String humPath;
String movPath;
String souPath;
String timePath;

int pirPin = 19;  // Pino de entrada do sensor de movimento
int soundSensorPin = 14; // Define o pino do sensor KY-037

//Variables for the DHT readings
float temperature;
float humidity;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 10000;

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void sendMessage(String message)
{
  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200)
  {
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}

void setup(){
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();
  initWiFi();

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  //Serial.println("Getting User UID");
  //while ((auth.token.uid) == "") {
    //Serial.print('.');
  //  delay(1000);
  //}

  // Print user UID
  uid = USER_UID;
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/ESP032/Dados_Coletados";

  // Update database path for sensor readings
  tempPath = databasePath + "/Temperatura"; // --> UsersData/<user_uid>/temperature
  humPath = databasePath + "/Humidade"; // --> UsersData/<user_uid>/humidity
  movPath = databasePath + "/Movimento";
  souPath = databasePath + "/Som";
}

void loop()
{
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    int soundValue = digitalRead(soundSensorPin); // Lê o valor digital do sensor
    int pirValue = digitalRead(pirPin);  // Lê o valor do pino do sensor de movimento

    // Get latest sensor readings
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    // Send readings to database:
    //Firebase.RTDB.setFloat(&fbdo, souPath.c_str(), soundValue);
    Firebase.RTDB.setFloat(&fbdo, tempPath.c_str(), temperature);
    Firebase.RTDB.setFloat(&fbdo, humPath.c_str(), humidity);
    if (pirValue == HIGH)
    {
        Firebase.RTDB.setString(&fbdo, movPath.c_str(), String("Detectado"));
    }
    else
    {
       Firebase.RTDB.setString(&fbdo, movPath.c_str(), String("Não Detectado"));
    }
    if (soundValue == 1)
    {
    Firebase.RTDB.setString(&fbdo, souPath.c_str(), String("Detectado"));
    }
    else
    {
    Firebase.RTDB.setString(&fbdo, souPath.c_str(), String("Não Detectado"));
    }
    if(pirValue == HIGH && soundValue == 1)
    {
      sendMessage("Atenção!");
    }
  }
}