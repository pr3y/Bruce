#ifndef __SERIAL_CLI_H__
#define __SERIAL_CLI_H__

#include <Arduino.h>
#include <SimpleCLI.h>

class SerialCli {
public:
    SerialCli();
    void setup(void);

    SimpleCLI getCli() { return _cli; };
    bool parse(const String &input) { return _cli.parse(input); }

private:
    SimpleCLI _cli;
};

void cliErrorCallback(cmd_error *e);

#endif

//  TODO: more commands https://docs.flipper.net/development/cli#0Z9fs
