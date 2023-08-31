import sys
import serial.tools.list_ports
from pathlib import Path
import esptool

def upload_firmware(name, port):
  try:
    esptool.main(["--port", f"{port}", "erase_flash"])
    esptool.main(["--port", f"{port}", "write_flash", "0x000000", f"{name}"])
  except:
    print("Cant execute firmware uploading!")

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

  file_names = list(Path(".").rglob('*.bin'))
  if len(file_names) >= 1:
    print("\nSelect firmware to upload")
    for idx, file_name in enumerate(file_names):
      print(f"{idx} - {file_name.name}")

    try:
      selected_file_index = int(input("Select firmware index: "))
      print(f"\nUploading {file_names[selected_file_index].name} firmware")
    except:
      print("Invalid firmware index")
      sys.exit(0)

    upload_firmware(file_names[selected_file_index].absolute(), selected_comport)
    sys.exit(0)

  print("Cant find firmware file in folder!")