/**
 * GH Action OTA
 * (c) 2023 mcuw
 *
 * Licensed under MIT
 **/

#ifndef ESPGHOTA_H
#define ESPGHOTA_H

// This URL is the symbolic link which returns the location of the latest release
#define FW_VERSION_URL "https://github.com/%s/releases/latest"

// The URL of a firmware from a release build
#define FW_BIN_URL "https://github.com/%s/releases/download/%s/firmware_%s_%s.bin"

class WiFiClientSecure;

class EspGhOta
{
public:
  /**
   * Constructor
   *
   * @param repository The repository parameter is the github fullpath including the user, e.g "mcuw/esp-closet-lighting"
   */
  EspGhOta(const char *repository);

  virtual ~EspGhOta();

  /**
   * Checks if there is a newer firmware then the current version
   *
   * @returns The latest version e.g. v1 if it exists otherwise empty string
   */
  String checkFirmwareVersion(const uint8_t currentVersion = 0);

  /**
   * Executes a firmware download from the latest github repository release and restarts afterwards
   *
   * @param latestVersion The version of the firmware with or w/o v charactor as prefix
   */
  void updateFirmware(const String &latestVersion);

private:
  // The repository variable is the github fullpath including the user, e.g "mcuw/esp-closet-lighting"
  const char *_repository;

  // The lookup URL to get the latest firmware release from github.com
  char urlGetLatestRelease[256];

  WiFiClientSecure *client;
};

#endif