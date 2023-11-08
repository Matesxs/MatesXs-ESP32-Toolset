import time
import serial.tools.list_ports
import sys
import threading

typical_boudrates = [
  110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000
]

class SerialReceiver(threading.Thread):
  def __init__(self, port: serial.Serial):
    super(SerialReceiver, self).__init__(daemon=True)

    self.port = port

  def run(self) -> None:
    while self.port.isOpen():
      if self.port.inWaiting() > 0:
        line = ser.readline().decode("utf-8").strip()
        print(f"{time.time():.2f} Recv:\t{line}")

if __name__ == '__main__':
  com_ports = [p.device for p in serial.tools.list_ports.comports()]
  if len(com_ports) == 0:
    print("No COM ports")
    sys.exit(-1)

  if len(com_ports) > 1:
    print("Avaible COM ports:")
    for idx, i in enumerate(com_ports):
      print(f"{idx} - {i}")

    selected_comport_index = int(input("Select COM port index: "))
    if selected_comport_index < 0:
      raise Exception("Invalid COM port index")

    try:
      selected_comport = com_ports[selected_comport_index]
    except:
      raise Exception("Invalid COM port index")
  else:
    selected_comport = com_ports[0]

  for idx, i in enumerate(typical_boudrates):
    print(f"{idx} - {i}")

  selected_boudrate_index = int(input("Select baudrate index: "))
  try:
    baudrate = typical_boudrates[selected_boudrate_index]
  except:
    raise Exception("Invalid baudrate index")

  try:
    ser = serial.Serial(port=selected_comport, baudrate=baudrate, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=5)
  except Exception:
    print("Failed to connect to serial device")
    sys.exit(-1)
    
  print(f"\nConnected to {selected_comport}\nOpening communication channel:")
  monitor = SerialReceiver(ser)
  monitor.start()

  while True:
    try:
      inp = input().strip()
      ser.write((inp + "\r\n").encode("utf-8"))
    except KeyboardInterrupt:
      break
    except Exception:
      print("Connection lost")
      break

  ser.close()