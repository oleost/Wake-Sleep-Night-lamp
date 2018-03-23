/*************************************************************

 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <Adafruit_Neopixel.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "********";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "****";
char pass[] = "****";

BlynkTimer timer;

WidgetRTC rtc;

// Timer Settings
unsigned long previousMillis = 0;
const long interval = 5000;

// Light start and stop

int inputStart = 7; // Default time on boot
int inputStop = 19; // Default time on boot

//Configure Neopixel
#define PIN D4
#define NUMPIXELS 16

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// Digital clock display of the time
void clockDisplay()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();

  // Send time to the App
  Blynk.virtualWrite(V1, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V2, currentDate);
}

BLYNK_WRITE(V1) {

  long startTimeInSecs = param[0].asLong();
  Serial.println(startTimeInSecs / 60 / 60);
  Serial.println();
  inputStart = startTimeInSecs / 60 / 60;
}

BLYNK_WRITE(V2) {
  long startTimeInSecs = param[0].asLong();
  Serial.println(startTimeInSecs / 60 / 60);
  Serial.println();
  inputStop = startTimeInSecs / 60 / 60;
}

void writeLeds() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println(inputStart);
    Serial.println(inputStop);
    if (hour() > inputStart && hour() < inputStop) {
      Serial.println();
      Serial.print("Time is between ");
      Serial.print(inputStart);
      Serial.print(" and ");
      Serial.print(inputStop);
      Serial.println();
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
      }
      pixels.show();
    }
    else {
      Serial.println("The time is not inside range");
      for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(150,0,0)); // Moderately bright red color.
      pixels.show();
      }
    }
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
  // Begin synchronizing time
  rtc.begin();
  timer.setInterval(10000L, clockDisplay);
  int inputStart;
  int inputStop;
  pixels.begin(); // This initializes the NeoPixel library.
}


void loop()
{
  Blynk.run();
  timer.run();
  writeLeds();
}
