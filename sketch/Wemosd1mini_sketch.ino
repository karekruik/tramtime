
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
ESP8266WiFiMulti WiFiMulti;

// screen > Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V
 
// include library, include base class, make path known
#include <GxEPD.h>
#include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w 
#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h> 

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
 
GxIO_Class io(SPI, /*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 5); // 
GxEPD_Class display(io, /*RST=D4*/ 5, /*BUSY=D2*/ 4); // 

//
void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  display.init(115200); // enable diagnostic output on Serial
  Serial.println();
//  for (uint8_t t = 4; t > 0; t--) {
//    Serial.printf("[SETUP] WAIT %d...\n", t);
//    Serial.flush();
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Wifi_name_here", "Wifi_password_here"); 
}

unsigned long t; //millisecond
unsigned long int_time_to_next_tram_2; 
String next_tram1 = "";
String next_tram2 = "";
void loop() {
  
  // wait for WiFi connection
    
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    Serial.println("Wifi connected");
    HTTPClient http;
    // make sure you use HTTP address instead of HTTPS. Current code does not accept HTTPS
    if (http.begin(client, "Google_Functions_API_request_address_here  e.g.:http://xxxxxxxxxx.cloudfunctions.net/xxxxxxxxxxx")) { 
    // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error      
      Serial.print("http code: ");
      Serial.println(httpCode);
      // Sometimes Google Directions API does not work. 
      // Instead of Wemos to keep trying and wasting energy, let it sleep and try later.
      if (httpCode != 200) {
        Serial.println("HTTP code is not 200, sleeping 15 min before trying again");
        write2screenError("", &FreeMonoBold18pt7b);
        ESP.deepSleep(15e8); // sleep for 15 min
      }
      if (httpCode > 0) {
        // HTTP header has been sent and Server response header has been handled
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          
          // Check the payload's first three letters. If they are equal to "Now", then it comes from Google API
          // also it means Google API is running and result is valid
          // Otherwise sleep for 15 min to try again later
          if (payload.substring(0,3).equals("Now") == false) {
            Serial.println("Payload check: The data is not valid");
            Serial.println("Sleeping for 15 min");
            write2screenError("", &FreeMonoBold18pt7b);
            ESP.deepSleep(15e8); // sleep for 15 min
          }
          
          // parse payload          
          String now_text = separateString(payload, ',', 0);
          next_tram1 = separateString(payload, ',', 1);
          next_tram2 = separateString(payload, ',', 2);
          String time_to_next_tram_2 = separateString(payload, ',', 3);

          Serial.println("now_text:\t" + now_text);
          Serial.println("next_tram1:\t" + next_tram1);
          Serial.println("next_tram2:\t" + next_tram2);
          Serial.println("time_to_next_tram_2:\t" + time_to_next_tram_2);
          int_time_to_next_tram_2 = time_to_next_tram_2.toInt();
          
          unsigned long long sleep_duration; // in microsecond
          long sleep_duration_in_sec; // in second
          
          // time_to_next_tram_2 is in seconds. Wemos is put to sleep and woke up 5 min before the next_tram_2 departure time in case of any delay
          sleep_duration_in_sec = int_time_to_next_tram_2 - 300; //  seconds till wake-up

          // if sleep duration is longer than 3 hours, ESP is not able to sleep that long, therefore:
          // if sleep_duration_in_sec > 3 h, sleep 3 h
          if (sleep_duration_in_sec > 10800) { sleep_duration_in_sec = 10800 ;}
          
          // if duration is <0 then sleep only 5 min
          if (sleep_duration_in_sec < 300){
            Serial.println("Second tram is in less than 5 min");
            Serial.println("Sleeping only 5 min");
            sleep_duration_in_sec = 300;          
          }
          sleep_duration = sleep_duration_in_sec * 1e6; // sleep duration is microsecond 
          
          // Serial print sleep time
          Serial.print("sleep duration [us]:\t"); 
          Serial.println(sleep_duration); 
          Serial.print("sleep duration [s]:\t\t"); 
          Serial.println(sleep_duration_in_sec);
           
          // Print tram times to screen
          write2screen("", &FreeMonoBold24pt7b);

          // sleep
          ESP.deepSleep(sleep_duration);
        }
      } else {Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());   }
      http.end();
    } else {Serial.printf("[HTTP} Unable to connect\n");}
    
    Serial.println("end of loop");
  }

  //t = millis();
  //while ( (millis() < t+2000)  ){
  //  yield();
  //}
}


// string separator tool
String separateString(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// print text for tram times on e-ink display
void write2screen(const char name[], const GFXfont* f)
{
  display.setRotation(270);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  //display.println(name);
  display.println(separateString(next_tram1, ' ', 0)); 
  display.println(separateString(next_tram1, ' ', 1));
  display.println(separateString(next_tram2, ' ', 0)); 
  display.println(separateString(next_tram2, ' ', 1));
  display.update();
  delay(500);
}

// print text for error screen on e-ink display 
void write2screenError(const char name[], const GFXfont* f)
{
  display.setRotation(2);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  //display.println(name);
  display.println("Error."); 
  display.println("Sleeping 15 min");
  display.println("Will try again.");  
  display.update();
  delay(500);
}
