#include "EthernetServerHandler.h"

#ifdef DEBUG_LEVEL
#include "debug.h"
#endif

EthernetServerHandler::EthernetServerHandler(const uint16_t port, bool acceptSerial) :
  server_port(port),
  acceptSerial(acceptSerial)
{
  server = new EthernetServer(server_port);
}

bool EthernetServerHandler::available()
{
  return buff != "";
}

void EthernetServerHandler::get_message(String &dest)
{
  dest = buff.substring(0);
  buff.remove(0, buff.length());
}

void EthernetServerHandler::send_message(String message)
{
  if (client.connected())
  {
    client.print(message + "/r/n");
  }

  if (acceptSerial)
    Serial.println(message);
}

void EthernetServerHandler::handleReceivedMessage()
{
  if (buff != "")
  {
    // Process static commands
    // Check state of ethernet connection (0 - disconnected, 1 - connected)
    if (buff == "STATE")
    {
      buff.remove(0, buff.length());
      send_message("OK " + String(connectedState));
    }
#ifdef ESP32
    // Restart ESP - there will be restarting ethernet modul too
    else if (buff == "RESTART")
    {
      send_message("OK");
      delay(10);
      ESP.restart();
    }
#endif
    // Check if ethernet modul is working properly (1 - OK, 0 - NOK)
    else if (buff == "CHECK")
    {
      buff.remove(0, buff.length());
      send_message("OK " + (Ethernet.hardwareStatus() != EthernetHardwareStatus::EthernetNoHardware));
    }
  }
}

void EthernetServerHandler::update()
{
  if (Ethernet.hardwareStatus() != EthernetHardwareStatus::EthernetNoHardware && Ethernet.linkStatus() == EthernetLinkStatus::LinkON)
  {
    EthernetClient tmp = server->available();

    if (tmp.available() && tmp != client)
    {
      if (connectedState)
      {
        // Disconnect routine
        client.stop();
        connectedState = false;

#ifdef DEBUG_LEVEL
        DEBUG("Client disconnected");
#endif
      }

      client = tmp;
      connectedState = true;

#ifdef DEBUG_LEVEL
      DEBUG("Client connected");
#endif
    }
  }

  if (acceptSerial && Serial.available())
  {
    // Receive serial
    buff.remove(0, buff.length());

    buff = Serial.readStringUntil('\n');
    buff.trim();

#ifdef DEBUG_LEVEL
    DEBUG("Serial message received: %s", buff.c_str());
#endif
    vTaskDelay(pdMS_TO_TICKS(1));

    handleReceivedMessage();
  }

  if (connectedState)
  {
    if (client.connected())
    {
      if (Ethernet.linkStatus() != EthernetLinkStatus::LinkON)
      {
#ifdef DEBUG_LEVEL
        DEBUG("Client disconnected");
#endif

        client.stop();
        connectedState = false;
      }
      else if (client.available())
      {
        // Receive TCP
        buff.remove(0, buff.length());

        buff = client.readStringUntil('\n');
        buff.trim();

#ifdef DEBUG_LEVEL
        DEBUG("TCP message received: %s", buff.c_str());
#endif
        vTaskDelay(pdMS_TO_TICKS(1));

        handleReceivedMessage();
      }
    }
    else
    {
      // Disconnect routine
      client.stop();
      connectedState = false;

#ifdef DEBUG_LEVEL
      DEBUG("Client disconnected");
#endif
    }
  }
  else if (Ethernet.hardwareStatus() == EthernetHardwareStatus::EthernetNoHardware)
  {
#ifdef DEBUG_LEVEL
    ERROR("Can't connect to ethernet module, restarting...");
#endif

    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP.restart();
  }
}
