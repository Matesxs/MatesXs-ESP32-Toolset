#pragma once

#include <Arduino.h>
#include <Ethernet.h>

class EthernetServerHandler
{
  private:
    EthernetServer *server = NULL;
    EthernetClient client;    
    String buff = "";
    bool connectedState = false;

    const uint16_t server_port;
    const bool acceptSerial;

  private:
    void handleReceivedMessage();

  public:
    EthernetServerHandler(const uint16_t port, bool acceptSerial);
    bool available();
    void get_message(String &dest);
    void update();
    void send_message(String message);
};
