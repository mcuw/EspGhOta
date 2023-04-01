#include <Arduino.h>
#include <WiFi.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <EspGhOta.h>

// const char MY_SSID[] = SECRET_SSID; // Loaded from arduino_secrets.h
// const char MY_PASS[] = SECRET_PASS; // Loaded from arduino_secrets.h

#define FirmwareVersion 1
#define REPOSITORY "mcuw/EspGhOta" // change to your repository!

EspGhOta ota(REPOSITORY);

void initializeWiFi()
{
  Serial.println("Initialize WiFi");
  WiFi.begin(MY_SSID, MY_PASS);
  Serial.print(F("Attempting to connect to SSID: "));
  Serial.println(MY_SSID);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(F("."));
    delay(200);
  }

  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.print(F("Active firmware version: "));
  Serial.println(FirmwareVersion);

  initializeWiFi();
}

void loop()
{
  String maybelatestVersion(ota.checkFirmwareVersion(FirmwareVersion));
  ota.updateFirmware(maybelatestVersion);

  delay(10000);
}