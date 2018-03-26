/*
obs-websocket
Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <obs-frontend-api.h>

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/base64.h>
#include <mbedtls/sha256.h>
#include <string>

#define WEBSOCKET_LWS_SERVER_STRING "obs-websocket"
#define WEBSOCKET_LWS_PROTOCOL_NAME "obswebsocketapi"

#define DEFAULT_PORT 4444

#define SECTION_NAME "WebsocketAPI"
#define PARAM_ENABLE "ServerEnabled"
#define PARAM_PORT "ServerPort"
#define PARAM_DEBUG "DebugEnabled"
#define PARAM_ALERT "AlertsEnabled"
#define PARAM_AUTHREQUIRED "AuthRequired"
#define PARAM_SECRET "AuthSecret"
#define PARAM_SALT "AuthSalt"

#define QT_TO_UTF8(str) str.toUtf8().constData()

using namespace std;

class Config {
  public:
    Config();
    ~Config();
    void Load();
    void Save();

    void SetPassword(string password);
    bool CheckAuth(string userChallenge);
    string GenerateSalt();
    static string GenerateSecret(string password, string salt);

    bool ServerEnabled;
    uint16_t ServerPort;

    bool DebugEnabled;
    bool AlertsEnabled;

    bool AuthRequired;
    string Secret;
    string Salt;
    string SessionChallenge;
    bool SettingsLoaded;

    static Config* Current();

  private:
    static Config* _instance;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context rng;
};

#endif // CONFIG_H