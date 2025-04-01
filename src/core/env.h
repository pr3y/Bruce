#ifndef __ENV_H__
#define __ENV_H__

#include <Arduino.h>
#include <LittleFS.h>

class Env {
public:
    static String getEnvValue(const String& key, const String& defaultValue = "") {
        if (!LittleFS.begin()) {
            return defaultValue;
        }

        if (!LittleFS.exists("/.env")) {
            return defaultValue;
        }

        File file = LittleFS.open("/.env", "r");
        if (!file) {
            return defaultValue;
        }

        String value = defaultValue;
        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();

            if (line.startsWith(key + "=")) {
                value = line.substring(key.length() + 1);
                break;
            }
        }

        file.close();
        return value;
    }
};

#endif
