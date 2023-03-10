/* Copyright 2017, 2018 David Conran
*
* An IR LED circuit *MUST* be connected to the ESP8266 on a pin
* as specified by kIrLed below.
*
* TL;DR: The IR LED needs to be driven by a transistor for a good result.
*
* Suggested circuit:
*     https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
*
* Common mistakes & tips:
*   * Don't just connect the IR LED directly to the pin, it won't
*     have enough current to drive the IR LED effectively.
*   * Make sure you have the IR LED polarity correct.
*     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
*   * Typical digital camera/phones can be used to see if the IR LED is flashed.
*     Replace the IR LED with a normal LED if you don't have a digital camera
*     when debugging.
*   * Avoid using the following pins unless you really know what you are doing:
*     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
*     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
*     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
*   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
*     for your first time. e.g. ESP-12 etc.
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

const char *ssid = "Prolink_DL7303_47DA";
const char *password = "rvd25Fz6wQ";

ESP8266WebServer server(80);
String webpage;
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRPanasonicAc ac(kIrLed);  // Set the GPIO used for sending messages.


void printState() {
  // Display the settings.
  Serial.println("Panasonic A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());
  // Display the encoded IR sequence.
  unsigned char* ir_code = ac.getRaw();
  Serial.print("IR Code: 0x");
  for (uint8_t i = 0; i < kPanasonicAcStateLength; i++)
    Serial.printf("%02X", ir_code[i]);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  ac.begin();
  delay(200);
  Serial.print("Koneksi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }

  Serial.print("\n");
  Serial.print("IP: ");
  Serial.print(WiFi.localIP());
  Serial.print("\n");
  Serial.print("Terhubung ke: ");
  Serial.print(ssid);
  Serial.print("\n");

  webpage += "<!DOCTYPE html>";
  webpage += "<html lang=\"en\">";
  webpage += "<head>";
  webpage += "<meta charset='UTF-8'>";
  webpage += "<meta http-equiv='X-UA-Compatible' content='IE=edge'>";
  webpage += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  webpage += "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-GLhlTQ8iRABdZLl6O3oVMWSktQOp6b7In1Zl3/Jr59b6EGGoI1aFkw7cmDA6j6gD' crossorigin='anonymous'>";
  webpage += "<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/uikit@3.15.19/dist/css/uikit.min.css'/>";
  webpage += "<script src='https://cdn.jsdelivr.net/npm/uikit@3.15.20/dist/js/uikit.min.js'></script>";
  webpage += "<script src='https://cdn.jsdelivr.net/npm/uikit@3.15.20/dist/js/uikit-icons.min.js'></script>";
  webpage += "<title>Smart School</title>";
  webpage += "</head>";
  webpage += "<body>";
  webpage += "<div class='container mt-5'>";
  webpage += "<div id='grid' class='uk-child-width-1-1@m uk-grid-small uk-grid-match' uk-grid>";
  webpage += "<div>";
  webpage += "</div>";
  webpage += "<script src='https://smaispa.sch.id/switchAC.js'></script>";
  webpage += "<script src='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/js/bootstrap.bundle.min.js' integrity='sha384-w76AqPfDkMBDXo30jS1Sgez6pr3x5MlQ1ZAGC+nuZB+EYdgRZgiwxhTBTkF7CXvN' crossorigin='anonymous'></script>";
  webpage += "</body>";
  webpage += "</html>";  
  
  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });   
  
  server.on("/acOn", []() {
    server.send(200, "text/html", webpage);
    acActive();
  });   
  
  server.on("/acOff", []() {
    server.send(200, "text/html", webpage);
    nonActive();
  });  

  server.begin();
  Serial.println("Server Murup");
}

void loop() {
  server.handleClient();
}

void acActive() {
  Serial.println("Default state of the remote.");
  printState();
  Serial.println("Setting desired state for A/C.");
  ac.setModel(kPanasonicRkr);
  ac.on();
  ac.setFan(kPanasonicAcFanAuto);
  ac.setMode(kPanasonicAcCool);
  ac.setTemp(26);
  ac.setSwingVertical(kPanasonicAcSwingVAuto);
  ac.setSwingHorizontal(kPanasonicAcSwingHAuto);

  #if SEND_PANASONIC_AC
    Serial.println("Sending IR command to A/C ...");
    ac.send();
  #endif  // SEND_PANASONIC_AC
    printState();
    delay(5000);
}

void nonActive() {
  ac.off();
  Serial.println(ac.toString());
  #if SEND_PANASONIC_AC
    ac.send();
  #endif // SEND_PANASONIC_AC
}
