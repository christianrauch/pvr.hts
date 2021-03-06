/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "addon.h"

#include "Tvheadend.h"
#include "tvheadend/Settings.h"
#include "tvheadend/utilities/Logger.h"

using namespace P8PLATFORM;
using namespace tvheadend;
using namespace tvheadend::utilities;

ADDON_STATUS CHTSAddon::Create()
{
  /* Configure the logger */
  Logger::GetInstance().SetImplementation([](LogLevel level, const char* message) {
    /* Convert the log level */
    AddonLog addonLevel;

    switch (level)
    {
      case LogLevel::LEVEL_FATAL:
        addonLevel = AddonLog::ADDON_LOG_FATAL;
        break;
      case LogLevel::LEVEL_ERROR:
        addonLevel = AddonLog::ADDON_LOG_ERROR;
        break;
      case LogLevel::LEVEL_WARNING:
        addonLevel = AddonLog::ADDON_LOG_WARNING;
        break;
      case LogLevel::LEVEL_INFO:
        addonLevel = AddonLog::ADDON_LOG_INFO;
        break;
      default:
        addonLevel = AddonLog::ADDON_LOG_DEBUG;
    }

    /* Don't log trace messages unless told so */
    if (level == LogLevel::LEVEL_TRACE && !Settings::GetInstance().GetTraceDebug())
      return;

    kodi::Log(addonLevel, "%s", message);
  });

  Logger::GetInstance().SetPrefix("pvr.hts");

  Logger::Log(LogLevel::LEVEL_INFO, "starting PVR client");

  Settings::GetInstance().ReadSettings();

  return ADDON_STATUS_OK;
}

ADDON_STATUS CHTSAddon::SetSetting(const std::string& settingName,
                                   const kodi::CSettingValue& settingValue)
{
  CLockObject lock(m_mutex);
  return Settings::GetInstance().SetSetting(settingName, settingValue);
}

ADDON_STATUS CHTSAddon::CreateInstance(int instanceType,
                                       const std::string& instanceID,
                                       KODI_HANDLE instance,
                                       const std::string& version,
                                       KODI_HANDLE& addonInstance)
{
  CLockObject lock(m_mutex);

  if (instanceType == ADDON_INSTANCE_PVR)
  {
    Logger::Log(LogLevel::LEVEL_DEBUG, "%s: Creating PVR-Client instance", __FUNCTION__);

    /* Connect to ARGUS TV */
    CTvheadend* client = new CTvheadend(instance, version);
    client->Start();
    addonInstance = client;

    return ADDON_STATUS_OK;
  }

  return ADDON_STATUS_UNKNOWN;
}

ADDONCREATOR(CHTSAddon)
