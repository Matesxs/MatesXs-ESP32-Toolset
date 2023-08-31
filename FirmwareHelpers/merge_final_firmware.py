import os
import subprocess

build_name = input("Build name: ")

FLASH_SIZE = "4MB"
CHIP = "esp32"
BOOTLOADER_PATH = rf".\.pio\build\{build_name}\bootloader.bin"
BOOTLOADER_OFFSET = 0x1000
PARTITIONS_PATH = rf".\.pio\build\{build_name}\partitions.bin"
PARTITIONS_OFFSET = 0x8000
FIRMWARE_PATH = rf".\.pio\build\{build_name}\firmware.bin"
FIRMWARE_OFFSET = 0x10000

if __name__ == "__main__":
  assert os.path.exists(BOOTLOADER_PATH), "Cant find bootloader file"
  assert os.path.exists(PARTITIONS_PATH), "Cant find partitions file, maybe you didnt compile your project yet so this file is not generated"
  assert os.path.exists(FIRMWARE_PATH), "Cant find firmware file, maybe you didnt compile your project yet so this file is not generated"

  firmware_name = input("Name of firmware: ")

  subprocess.Popen(f"python -m esptool --chip {CHIP} merge_bin -o {firmware_name}.bin --flash_mode dio --flash_freq 40m --flash_size {FLASH_SIZE} {BOOTLOADER_OFFSET} {BOOTLOADER_PATH} {PARTITIONS_OFFSET} {PARTITIONS_PATH} {FIRMWARE_OFFSET} {FIRMWARE_PATH}").wait()
