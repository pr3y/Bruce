//SSH borrowed from https://github.com/m5stack/M5Cardputer :)

//TODO: Display is kinda glitchy :P figure out some way to show better outputs also

// SSH libs
#include "libssh_esp32.h"
#include <libssh/libssh.h>

// Telnet libs
#include <Arduino.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lwip/sockets.h>
#include "clients.h"
#include "globals.h"
#include "display.h"
#include "mykeyboard.h"

// SSH server configuration (initialize as mpty strings)
String ssh_host     = "";
String ssh_user     = "";
String ssh_port     = "";
String ssh_password = "";

char* ssh_port_char;

// M5Cardputer setup
//M5Canvas canvas(&DISP);
String commandBuffer              = "> ";
int cursorY                       = 0;
const int lineHeight              = 32;
unsigned long lastKeyPressMillis  = 0;
const unsigned long debounceDelay = 200;  // Adjust debounce delay as needed

//ssh_bind sshbind = (ssh_bind)state->input;

//ssh_init sshbind;
ssh_session my_ssh_session;
ssh_channel channel_ssh;


char* stringTochar(String s)
{
    if (s.length() == 0) {
        return nullptr; // or handle the case where the string is empty
    }

    static char arr[14]; // Make sure it's large enough to hold the IP address
    s.toCharArray(arr, sizeof(arr));
    return arr;
}

bool filterAnsiSequences = true;  // Set to false to disable ANSI sequence filtering


void ssh_loop() {
    
    for(;;){
    #ifdef CARDPUTER
        if (Keyboard.isChange() && Keyboard.isPressed()) {
            unsigned long currentMillis = millis();
            if (currentMillis - lastKeyPressMillis >= debounceDelay) {
                lastKeyPressMillis               = currentMillis;
                Keyboard_Class::KeysState status = Keyboard.keysState();

                for (auto i : status.word) {
                    commandBuffer += i;
                    tft.print(i);
                    cursorY = tft.getCursorY();
                }

                if (status.del && commandBuffer.length() > 2) {
                    commandBuffer.remove(commandBuffer.length() - 1);
                    tft.setCursor(
                        tft.getCursorX() - 6,
                        tft.getCursorY());
                    tft.print(" ");
                    tft.setCursor(
                        tft.getCursorX() - 6,
                        tft.getCursorY());
                    cursorY = tft.getCursorY();
                }

                if (status.enter) {
                    commandBuffer.trim();  // Trim the command buffer to remove
                                        // accidental TFT_WHITEspaces/newlines
                    String message = commandBuffer.substring(
                        2);  // Get the command part, exclude the "> "
                    ssh_channel_write(channel_ssh, message.c_str(),
                                    message.length());  // Send the command
                    ssh_channel_write(channel_ssh, "\r",
                                    1);  // Send exactly one carriage return (try
                                        // "\n" or "\r\n" if needed)

                    commandBuffer = "> ";  // Reset command buffer
                    tft.print(
                        '\n');  // Move to the next line on display
                    cursorY =
                        tft.getCursorY();  // Update cursor position
                }
            }
        }

    #else
        if(checkSelPress()) {
            while(checkSelPress()) { yield(); } // timerless debounce
            commandBuffer = keyboard(commandBuffer,76,"SSH Command: ");
            while(checkSelPress()) { yield(); } // timerless debounce
            commandBuffer.trim();  // Trim the command buffer to remove
                                    // accidental TFT_WHITEspaces/newlines
            String message = "";
            if(commandBuffer.startsWith("> ")) message = commandBuffer.substring(2);  // Get the command part, exclude the "> "
            else message = commandBuffer;
            ssh_channel_write(channel_ssh, message.c_str(),
                              message.length());  // Send the command
                              ssh_channel_write(channel_ssh, "\r",
                              1);                // Send exactly one carriage return (try
                                                 // "\n" or "\r\n" if needed)

            commandBuffer = "> ";  // Reset command buffer
                    tft.print('\n');  // Move to the next line on display
                    cursorY =tft.getCursorY();  // Update cursor position
        }
                    
    #endif

        // Check if the cursor has reached the bottom of the display
        if (cursorY > tft.height() - lineHeight) {
            tft.setCursor(0, -lineHeight);
            cursorY -= lineHeight;
            tft.setCursor(tft.getCursorX(),
                                        cursorY);
        }

        // Read data from SSH server and display it, handling ANSI sequences
        char buffer[128];  // TFT_REDuced buffer size for less memory usage
        int nbytes =
            ssh_channel_read_nonblocking(channel_ssh, buffer, sizeof(buffer), 0);
        bool isAnsiSequence =
            false;  // To track when we are inside an ANSI sequence

        if (nbytes > 0) {
            for (int i = 0; i < nbytes; ++i) {
                char c = buffer[i];
                if (filterAnsiSequences) {
                    if (c == '\033') {
                        isAnsiSequence = true;  // Enter ANSI sequence mode
                    } else if (isAnsiSequence) {
                        if (isalpha(c)) {
                            isAnsiSequence = false;  // Exit ANSI sequence mode at
                                                    // the end character
                        }
                    } else {
                        if (c == '\r') continue;  // Ignore carriage return
                        tft.write(c);
                        cursorY = tft.getCursorY();
                    }
                } else {
                    if (c == '\r') continue;  // Ignore carriage return
                    tft.write(c);
                    cursorY = tft.getCursorY();
                }
            }
        }

        // Handle channel closure and other conditions
        if (nbytes < 0 || ssh_channel_is_closed(channel_ssh)) {
            ssh_channel_close(channel_ssh);
            ssh_channel_free(channel_ssh);
            ssh_disconnect(my_ssh_session);
            ssh_free(my_ssh_session);
            displayRedStripe("\nSSH session closed.");
            tft.setTextColor(FGCOLOR, BGCOLOR);
            return;  // Exit the loop upon session closure
        }
    }
}

void ssh_setup(){
    if (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("Connect to wifi before using wireguard");
      displayRedStripe("CONNECT TO WIFI",TFT_WHITE, TFT_RED);
      delay(5000);
      return;
    }
    tft.fillScreen(BGCOLOR);
    tft.setCursor(0, 0);
    Serial.begin(115200);  // Initialize serial communication for debugging
    Serial.println("Starting Setup");

    //auto cfg = M5.config();
    //M5Cardputer.begin(cfg, true);
    tft.setRotation(1);
    tft.setTextSize(1);  // Set text size
    
    cursorY = tft.getCursorY();

    tft.setCursor(0, 0);
    // Prompt for SSH host, username, and password
    //tft.print("SSH Host: \n");
    // waitForInput(ssh_host);
    ssh_host=keyboard("",76,"SSH HOST");
    //tft.print("SSH Port: \n");
    //waitForInput(ssh_port);
    ssh_port=keyboard("",76,"SSH PORT");
    
    ssh_port_char = stringTochar(ssh_port);
    uint16_t ssh_port_int = atoi(ssh_port_char);

    //tft.print("\nSSH Username: ");
    
    //waitForInput(ssh_user);
    ssh_user=keyboard("",76,"SSH USER");
    //tft.print("\nSSH Password: ");
    
    //waitForInput(ssh_password);
    ssh_password=keyboard("",76,"SSH PASSWORD");

    Serial.println("BEFORE SSH");
    my_ssh_session = ssh_new();
    Serial.println("AFTER SSH");


    
    if (my_ssh_session == NULL) {
        tft.setTextColor(TFT_RED, BGCOLOR);
        displayRedStripe("SSH Shell request error.");
        Serial.println("SSH Session creation failed.");
        delay(5000);
        return;
    }
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, ssh_host.c_str());
    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &ssh_port_int);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, ssh_user.c_str());
    Serial.println("AFTER COMPARE AND OPTION SET");
    
    if (ssh_connect(my_ssh_session) != SSH_OK) {
        tft.setTextColor(TFT_RED, BGCOLOR);
        displayRedStripe("SSH Shell request error.");
        Serial.println("SSH Connect error.");
        ssh_free(my_ssh_session);
        delay(5000);
        return;
    }

    if (ssh_userauth_password(my_ssh_session, NULL, ssh_password.c_str()) !=
        SSH_AUTH_SUCCESS) {
        tft.setTextColor(TFT_RED, BGCOLOR);
        displayRedStripe("SSH Shell request error.");
        Serial.println("SSH Authentication error.");
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        delay(5000);
        return;
    }

    channel_ssh = ssh_channel_new(my_ssh_session);
    if (channel_ssh == NULL || ssh_channel_open_session(channel_ssh) != SSH_OK) {
        tft.setTextColor(TFT_RED, BGCOLOR);
        displayRedStripe("SSH Shell request error.");
        Serial.println("SSH Channel open error.");
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        delay(5000);
        return;
    }

    if (ssh_channel_request_pty(channel_ssh) != SSH_OK) {
        tft.setTextColor(TFT_RED, BGCOLOR);
        displayRedStripe("SSH Shell request error.");
        Serial.println("SSH PTY request error.");
        ssh_channel_close(channel_ssh);
        ssh_channel_free(channel_ssh);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        delay(5000);
        return;
    }

    if (ssh_channel_request_shell(channel_ssh) != SSH_OK) {
        tft.setTextColor(TFT_RED, BGCOLOR);
        displayRedStripe("SSH Shell request error.");
        Serial.println("SSH Shell request error.");
        ssh_channel_close(channel_ssh);
        ssh_channel_free(channel_ssh);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        delay(5000);
        return;
    }
    

    Serial.println("SSH setup completed.");
    tft.setTextColor(TFT_GREEN, BGCOLOR);
    displayRedStripe("SSH Conected!", TFT_WHITE, TFT_DARKGREEN );
    delay(2000);
    tft.fillScreen(BGCOLOR);
    tft.setTextColor(TFT_WHITE, BGCOLOR);
    ssh_loop();
    
}


String telnet_server_string = "";
String telnet_port_string = "";
char* telnet_server_ip;
char* telnet_server_port_char;

int telnet_server_port; 

static int sock;


void telnet_loop() {
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(telnet_server_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(telnet_server_port);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        Serial.println("Unable to create socket");
        tft.setTextColor(TFT_RED, BGCOLOR);
        displayRedStripe("Unable to create socket");
        tft.setTextColor(FGCOLOR, BGCOLOR);
        delay(5000);
        return;
    }

    if (connect(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) != 0) {
        Serial.println("Socket connection failed");
        tft.setTextColor(TFT_RED, BGCOLOR);
        displayRedStripe("Socket connection failed");
        tft.setTextColor(FGCOLOR, BGCOLOR);
        close(sock);
        delay(5000);
        return;
    }

    Serial.println("Connected to TELNET server");
    tft.setTextColor(TFT_GREEN, BGCOLOR);
    displayRedStripe("Connected to TELNET server", TFT_WHITE, TFT_DARKGREEN );
    tft.setTextColor(FGCOLOR, BGCOLOR);
    delay(2000);
    tft.fillScreen(BGCOLOR);
    tft.setCursor(0, 0);

    String commandInput;

    while (1) {
        tft.print("> ");
        //waitForInput(commandInput);
        commandInput=keyboard("",76,"COMMAND");
        const char *command = commandInput.c_str();
        send(sock, command, strlen(command), 0);

        // You can also receive data from the server
        char buffer[128];
        int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len > 0) {
            buffer[len] = '\0';
                        // Check for Telnet negotiation commands (IAC)
                        /*
            if (buffer[0] == 0xFF) {
                // Skip Telnet negotiation command
                continue;
            }
            */
            tft.setTextColor(TFT_WHITE, BGCOLOR);
            Serial.printf("Received from server %s\n", buffer);
            //tft.printf("Received from server %s\n", buffer);
            for (int i = 0; i < len; i++) {
             Serial.printf("%02X ", buffer[i]);
            }
            tft.printf("%s\n", buffer);

            tft.setTextColor(FGCOLOR, BGCOLOR);

        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void telnet_setup() {
    if (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("Connect to wifi before using wireguard");
      displayRedStripe("CONNECT TO WIFI",TFT_WHITE, TFT_RED);
      delay(5000);
      return;
    }
   tft.fillScreen(BGCOLOR);
    tft.setCursor(0, 0);
    Serial.begin(115200);  // Initialize serial communication for debugging
    Serial.println("Starting Setup");

    // auto cfg = M5.config();
    // M5Cardputer.begin(cfg, true);
    tft.setRotation(1);
    tft.setTextSize(1);  // Set text size
    
    cursorY = tft.getCursorY();

    tft.setCursor(0, 0);
    //tft.print("TELNET Host: \n");

    // Here the telnet_server_ip needs to be a char*, thats why the stringTochar()

    //waitForInput(telnet_server_string);
    telnet_server_string=keyboard("",76,"TELNET_SERVER");
    telnet_server_ip = stringTochar(telnet_server_string);
    delay(300);
    //Serial.println(telnet_server_ip);

    //tft.print("TELNET Port: \n");
    //waitForInput(telnet_port_string);
    telnet_port_string=keyboard("",76,"TELNET PORT");
    delay(300);
    char arr2[5];
    //telnet_server_port_char = 
    telnet_port_string.toCharArray(arr2, sizeof(arr2));
    //telnet_server_port_char = stringTochar(telnet_port_string);
    telnet_server_port = atoi(arr2);
    Serial.println(telnet_server_ip);
    Serial.println(telnet_server_port);

    telnet_loop();
}
