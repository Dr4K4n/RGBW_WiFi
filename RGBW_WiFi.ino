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
  FastLED.delay(100);
}