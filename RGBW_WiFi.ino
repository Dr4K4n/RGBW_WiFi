#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include "FastLED_RGBW.h"

const char* ssid     = "TinyTarup";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "FriedeFreudeEierkuchen!";

ESP8266WebServer server ( 80 );

constexpr int LED_IO = 5;
constexpr int NUM_LEDS = 72;
CRGBWArray<NUM_LEDS> leds;

CHSV color = CHSV(60, 60, 200);

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0"> 
<title>Stefans Leuchte</title>
<script src="https://cdn.jsdelivr.net/npm/@jaames/iro@5"></script>
<style>
body {
  font-family: sans-serif;
  text-align: center;
}
</style>
</head>
<body>
<div style="width: 400px; margin: 0 auto;">
<h1>Leuchte</h1>
<div id="picker"></div>
</div>
<script>
var colorPicker = new iro.ColorPicker('#picker', {width:400});
colorPicker.on('color:change', function(color) {
  var h = Math.floor(color.hsv.h * 255 / 360);
  var s = Math.floor(color.hsv.s * 255 / 100);
  var v = Math.floor(color.hsv.v * 255 / 100);  
  fetch("/?h=" + h + "&s=" + s + "&v=" + v);
});
</script>
</body>
</html>
)=====";

void handleRoot() {

  String h = server.arg("h");
  String s = server.arg("s");
  String v = server.arg("v");

  if (h != emptyString && s != emptyString && v != emptyString) {
    color = CHSV(h.toInt(), s.toInt(), v.toInt());
  }

  //char temp[400];
  //sprintf ( temp, "H:%d S:%d V:%d", color.h, color.s, color.v);
  //server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send_P ( 200, "text/html", MAIN_page);
}

void setup() {
  // WiFi setup
  WiFi.mode(WIFI_STA);
  WiFi.hostname("StefansLeuchte");
  WiFi.begin(ssid, password);

  // send order set to RGB here, but is indeed GRBW, which is handled by rgb2rgbw()
  FastLED.addLeds<WS2812, LED_IO, RGB>(leds, leds.rgb_size());
  delay(2000);

  // wait for wifi
  while ( WiFi.status() != WL_CONNECTED ) {
    leds[0] = CHSV(120, 60, 200);
    FastLED.delay(100);
    delay ( 500 );
    leds[0] = CHSV(60, 60, 200);
    FastLED.delay(100);
    Serial.print ( "." );
  }

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
  for(int i = 0; i < NUM_LEDS; ++i) {
    // set only RGB values
    leds[i] = color;
  }
  leds.convert_to_rgbw();
  FastLED.delay(300);

 /* // fill first half of LEDs with a HSV rainbow
  static uint8_t hue = 0;
  constexpr int HALF_LEDS = NUM_LEDS/2;
  for(int i = 0; i <= HALF_LEDS; ++i) {
    // set only RGB values
    leds[i] = CHSV(hue+i, 200-i, 128);
  }
  
  // alternatively use fill_gradient to get the same result as the for loop:
  //fill_gradient(&leds[0], HALF_LEDS + 1, CHSV(hue, 200, 255), CHSV(hue + HALF_LEDS, 200 - HALF_LEDS, 255));
  
  ++hue;

  
  // CRGBWArray allows for some convenience functions, like copying some RGB values
  constexpr int ODD_FIX = NUM_LEDS % 2 - 1; // For even NUM_LEDs: -1, for odd NUM_LEDs: 0
  leds(HALF_LEDS, NUM_LEDS-1) = leds(HALF_LEDS + ODD_FIX, 0);


  // add moving bar with screen blending
  static uint16_t pos = 0;
  constexpr uint16_t WIDTH = 4;
  for (uint16_t i = 0; i < WIDTH; ++i) {
    uint16_t wrapped_pos = (pos + i) % NUM_LEDS;
    leds[wrapped_pos].screen_blend(CRGB::FairyLightNCC);
  }
  
  if (++pos >= NUM_LEDS)
    pos -= NUM_LEDS;

  // prepare to send
  leds.convert_to_rgbw();
  
  // send
  FastLED.delay(100);

  // optionally revert the conversion, if you want to modify the values instead of overwriting
  //leds.revert_to_rgb();*/
}