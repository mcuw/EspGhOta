
#include <string.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>
#include "./cert.h"
#include "./EspGhOta.h"

static const char *MODULE_NAME = "EspGhOta";

EspGhOta::EspGhOta(const char *repository) : _repository(repository)
{
  sprintf(urlGetLatestRelease, FW_VERSION_URL, repository);

  this->client = new WiFiClientSecure();
  this->client->setCACert(rootCACertificate);
}

EspGhOta::~EspGhOta()
{
  if (this->client)
  {
    delete this->client;
  }
}

String EspGhOta::checkFirmwareVersion(const uint8_t currentVersion)
{
  String latestVersion("");

  ESP_LOGD(MODULE_NAME, "GET latest release version");
  HTTPClient https;
  if (!https.begin(*this->client, urlGetLatestRelease))
  {
    ESP_LOGE(MODULE_NAME, "Failed to connect: %s", urlGetLatestRelease);
    return "";
  }

  int httpCode = https.GET();
  if (httpCode == HTTP_CODE_FOUND)
  {
    String location(https.getLocation());
    String token(location.substring(location.indexOf("/tag/")));
    if (!token.length())
    {
      ESP_LOGW(MODULE_NAME, "No releases exists yet");
      return "";
    }
    latestVersion = token.substring(5); // w/o "/tag/"
  }
  else
  {
    ESP_LOGE(MODULE_NAME, "Failed to get the latest release - HTTP code: %s", httpCode);
  }
  https.end();

  ESP_LOGD(MODULE_NAME, "Check newer version or not?");
  String latestVersionNumber(latestVersion);
  latestVersionNumber.replace("v", "");
  uint8_t version = latestVersionNumber.toInt();
  if (version <= currentVersion) // if a version exists
  {
    ESP_LOGI(MODULE_NAME, "Device already on latest firmware version: %d, release: %d", currentVersion, version);
    return "";
  }

  ESP_LOGI(MODULE_NAME, "New firmware detected: %d", latestVersion);
  return latestVersion;
}

void EspGhOta::updateFirmware(const String &latestVersion)
{
  if (!latestVersion.length())
    return;

  char urlFirmware[256];
  sprintf(urlFirmware, FW_BIN_URL, this->_repository, latestVersion, PIOENV, latestVersion);

  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

  t_httpUpdate_return ret = httpUpdate.update(*this->client, urlFirmware);
  switch (ret)
  {
  case HTTP_UPDATE_FAILED:
    ESP_LOGE(MODULE_NAME, "OTA - HTTP_UPDATE_FAILED (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    ESP_LOGD(MODULE_NAME, "OTA - HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    ESP_LOGI(MODULE_NAME, "OTA - HTTP_UPDATE_OK");
    break;
  }
}