/*************************************************************

https://github.com/oleost/KidLampBlynkFastled

This sketch allows changing Hue, Sat and brightness on LEDs based on if the timer
is between time 1 and time 2.

Must be used together with Blynk APP. Used with RTC widghet, 2x input time and
6x sliders.

Virtual pin 0 in blynk app is used to enable "Party Mode".
Virtual pin 1 in blynk app is used to set start time.
Virtual pin 2 in blynk app is used to set stop time.
Virtual pin 3,4 and 5 is used to set Hue, Sat, and Brightness when time is between
start and stop time.
Virtual pin 6, 7 and 8 is used to set Hue, Sat and Brightness when time is not
between start and stop time.

Sketch uses ArduinoOTA, default password is "Secret"
*************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <FastLED.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

BlynkTimer timer;

WidgetRTC rtc;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "***";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "***";
char pass[] = "***";

// Light start and stop

int inputStart = 7; // Default time on boot
int inputStop = 19; // Default time on boot
int hueDay = 48; // Default hue when time is between time 1 and 2.
int satDay = 255; // Default sat when time is between time 1 and 2.
int briDay = 255; // Default brightness when time is between time 1 and 2.
int hueNight = 169; // Default hue when time is not between time 1 and 2.
int satNight = 255; // Default sat when time is not between time 1 and 2.
int briNight = 255; // Default brightness when time is not between time 1 and 2.

//Configure FastLED / NEOPIXEL
#define DATA_PIN D4
#define NUM_LEDS 33

CRGB leds[NUM_LEDS];

// End of configuration

int partymodeButton = 1;

// Timer Settings
unsigned long previousMillis = 0;
const long interval = 50;



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

BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V7 Slider value is: ");
  Serial.println(pinValue);
  partymodeButton = pinValue;
}

BLYNK_WRITE(V1) {
  long startTimeInSecs = param[0].asLong();
  Serial.println(startTimeInSecs / 60 / 60);
  Serial.println();
  inputStart = startTimeInSecs / 60 / 60;
}

BLYNK_WRITE(V2) {
  long startTimeInSecs = param[0].asLong();
  //Serial.println(startTimeInSecs / 60 / 60);
  //Serial.println();
  inputStop = startTimeInSecs / 60 / 60;
}

BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V3 Slider value is: ");
  Serial.println(pinValue);
  hueDay = pinValue;
}

BLYNK_WRITE(V4)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V4 Slider value is: ");
  Serial.println(pinValue);
  satDay = pinValue;
}

BLYNK_WRITE(V5)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V5 Slider value is: ");
  Serial.println(pinValue);
  briDay = pinValue;
}

BLYNK_WRITE(V6)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V6 Slider value is: ");
  Serial.println(pinValue);
  hueNight = pinValue;
}

BLYNK_WRITE(V7)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V7 Slider value is: ");
  Serial.println(pinValue);
  satNight = pinValue;
}

BLYNK_WRITE(V8)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V8 Slider value is: ");
  Serial.println(pinValue);
  briNight = pinValue;
}

void writeLeds() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    //Serial.println(inputStart);
    //Serial.println(inputStop);
    if (hour() > inputStart && hour() < inputStop) {
    /*Serial.println();
    Serial.print("Time is between ");
    Serial.print(inputStart);
    Serial.print(" and ");
    Serial.print(inputStop);
    Serial.println();
    Serial.println(hueDay);
    */
      for(int i=0;i<NUM_LEDS;i++){
        leds[i] = CHSV( hueDay, satDay, briDay);
      }
      FastLED.show();
    }
    else {
    //Serial.println("The time is not inside range");
      for(int i=0;i<NUM_LEDS;i++){
      leds[i] = CHSV( hueNight, satNight, briNight);
      FastLED.show();
      }
    }
  }
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void partyMode() {
	static uint8_t hue = 0;
	Serial.print("x");
	// First slide the led in one direction
	for(int i = 0; i < NUM_LEDS; i++) {
		// Set the i'th led to red
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
	Serial.print("x");

	// Now go in the other direction.
	for(int i = (NUM_LEDS)-1; i >= 0; i--) {
		// Set the i'th led to red
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
}

void updateVirtualpins() {
  Blynk.virtualWrite(V0, partymodeButton);
  Blynk.virtualWrite(V1, inputStart * 60 * 60);
  Blynk.virtualWrite(V2, inputStop * 60 * 60);
  Blynk.virtualWrite(V3, hueDay);
  Blynk.virtualWrite(V4, satDay);
  Blynk.virtualWrite(V5, briDay);
  Blynk.virtualWrite(V6, hueNight);
  Blynk.virtualWrite(V7, satNight);
  Blynk.virtualWrite(V8, briNight);
}

void setup()
{
  ArduinoOTA.setPassword((const char *)"Secret"); // for password prompt insert between " "
  ArduinoOTA.begin();
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
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  updateVirtualpins();
}


void loop()
{
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
  if (partymodeButton >= 3) {
    partyMode();
  }
  else {
  writeLeds();
}
}
