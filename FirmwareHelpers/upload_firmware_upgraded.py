import esptool
import serial
import serial.tools.list_ports
from pathlib import Path
import time

BAUDRATE = 115200
SET_DEVICE_ID_COMMAND = "deviceId:"
READ_DEVICE_ID_COMMAND = "deviceId?"
USED_IDS_FILE = "used_ids.txt"

def upload_firmware(name, port):
  try:
    esptool.main(["--port", f"{port}", "erase_flash"])
    esptool.main(["--port", f"{port}", "write_flash", "0x000000", f"{name}"])
  except:
    print("Cant execute firmware uploading!")

def write_device_id(device_id, port):
  with serial.Serial(port, BAUDRATE) as serial_port:
    serial_port.write(f"{SET_DEVICE_ID_COMMAND}{device_id}\r\n".encode("utf-8"))
    time.sleep(0.1)
    serial_port.write(f"{READ_DEVICE_ID_COMMAND}\r\n".encode("utf-8"))
    return_device_id = serial_port.readline().decode("utf-8").strip()
    assert return_device_id == device_id, "Returned device ID is different from device ID set by user"

if __name__ == '__main__':
  com_ports = [p.device for p in serial.tools.list_ports.comports()]
  file_names = list(Path(".").rglob('*.bin'))
  if len(com_ports) == 0:
    raise Exception("No COM ports")
  
  if len(file_names) == 0:
    raise Exception("No Firmwares")
  
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

  if len(file_names) > 1:
    print("\nSelect firmware to upload")
    for idx, file_name in enumerate(file_names):
      print(f"{idx} - {file_name.name}")

    try:
      selected_file_index = int(input("Select firmware index: "))
      file_path = file_names[selected_file_index]
    except:
      raise Exception("Invalid firmware index")
  else:
    file_path = file_names[0]

  device_id = input("Device ID: ").strip()
  if device_id != "":
    print("\nSearching for duplicated device ID")
    with open(USED_IDS_FILE, "r") as f:
      for line in f:
        line = line.strip()
        if device_id == line:
          raise Exception(f"ID {device_id} already used")

  print(f"\nUploading {file_path.name} firmware")
  upload_firmware(file_path.absolute(), selected_comport)

  if device_id != "":
    print(f"\nWriting device ID")
    write_device_id(device_id, selected_comport)
    with open(USED_IDS_FILE, "a") as f:
      f.write(f"{device_id}\n")
