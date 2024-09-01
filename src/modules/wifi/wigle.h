/**
 * @file wigle.h
 * @author Rennan Cockles - https://github.com/rennancockles
 * @brief Wigle connector
 * @version 0.1
 */

#ifndef WIGLE_H
#define WIGLE_H

#include "core/globals.h"
#include <WiFiClientSecure.h>


class Wigle {
public:
    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    Wigle();
    ~Wigle();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    bool get_user(void);
    bool upload(FS *fs, String filepath);
    void send_upload_headers(WiFiClientSecure &client, String filename, int filesize, String boundary);
    void display_banner(void);
    void dump_wigle_info(void);

private:
    String wigle_user;
    String auth_header;
    const char* host = "api.wigle.net";

    bool _check_token(void);
};

#endif // WIGLE_H
