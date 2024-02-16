#pragma once

#include <Arduino.h>
#include <Ethernet.h>

class EthernetClientHandler
{
  private:
    EthernetClient client;    
    String buff = "";
    bool connectedState = false;

    const IPAddress server_ip;
    const uint16_t server_port;
    const bool acceptSerial;

  private:
    void handleReceivedMessage();

  public:
    EthernetClientHandler(IPAddress ip, const uint16_t port, bool acceptSerial);
    bool available();
    void get_message(String &dest);
    void update();
    bool check_connection();
    void send_message(String message);
};
