#include "EthernetClientHandler.h"

#ifdef DEBUG_LEVEL
#include "debug.h"
#endif

EthernetClientHandler::EthernetClientHandler(const IPAddress ip, const uint16_t port, bool acceptSerial) :
  server_ip(ip),
  server_port(port),
  acceptSerial(acceptSerial)
{
}

bool EthernetClientHandler::check_connection()
{
  for (uint8_t i = 0; i <= 20; i++) 
  {
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      vTaskDelay(pdMS_TO_TICKS(50));
      continue;
    }
    
    break;
  }

  if (Ethernet.hardwareStatus() != EthernetNoHardware)
    return true;
  return false;
}

bool EthernetClientHandler::available()
{
  return buff != "";
}

void EthernetClientHandler::get_message(String &dest)
{
  dest = buff.substring(0);
  buff.remove(0, buff.length());
}

void EthernetClientHandler::send_message(const String& message)
{
  if (client.connected())
  {
    client.print(message + "/r/n");
  }

  if (acceptSerial)
    Serial.println(message);
}

void EthernetClientHandler::handleReceivedMessage()
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
      send_message("OK " + String(check_connection()));
    }
  }
}

void EthernetClientHandler::update()
{
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
        DEBUG("Server disconnected");
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
      DEBUG("Server disconnected");
#endif
    }
  }
  else if (check_connection())
  {
    // Try to connect    
    if (client.connect(server_ip, server_port))
    {
      connectedState = true;
      
#ifdef DEBUG_LEVEL
      DEBUG("Server connected");
#endif
    }
  }
  else
  {
#ifdef DEBUG_LEVEL
    ERROR("Can't connect to ethernet module, restarting...");
#endif

    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP.restart();
  }
}
