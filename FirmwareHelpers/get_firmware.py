import sys
import subprocess
import serial.tools.list_ports
import re

memory_size_regex = re.compile(r"(\d+[\.\,]\d+|\d+)([A-Z]*)")

def get_flash_size(port):
  def process_flash_size(string):
    flash_info = string.split("\n")
    flash_size_string = [split for split in flash_info if split.startswith("Detected flash size:")][0].strip()
    flash_size_parts = memory_size_regex.findall(flash_size_string)[0]
    
    flash_size = float(flash_size_parts[0])
    if len(flash_size_parts) > 1:
      if flash_size_parts[1] == "MB":
        flash_size *= 1048576
      elif flash_size_parts[1] == "KB":
        flash_size *= 1024
      else:
        raise ValueError("Invalid size sufix get")
      
    return hex(int(flash_size))

  flash_info_getter = subprocess.Popen(f"python -m esptool --port {port} flash_id", stdout=subprocess.PIPE)
  for line in flash_info_getter.stdout:
    line = line.decode("utf-8").strip()
    print(line)

    if line.startswith("Detected flash size:"):
      result = process_flash_size(line)
      flash_info_getter.kill()
      return result
  return None


def read_firmware_from_esp(port):
  firmware_name = input("Name of firmware: ")

  flash_size = get_flash_size(port)
  assert flash_size is not None, "Failed to retrieve flash size"

  subprocess.Popen(f"python -m esptool --port {port} read_flash 0x000000 {str(flash_size)} {firmware_name}.bin").wait()

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

  read_firmware_from_esp(selected_comport)
  sys.exit(0)