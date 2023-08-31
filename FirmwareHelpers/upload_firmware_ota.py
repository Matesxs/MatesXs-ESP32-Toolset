import os
import sys
from pathlib import Path
import socket
import logging
import hashlib
import random

def update_progress(progress):
  barLength = 60 # Modify this to change the length of the progress bar
  status = ""
  if isinstance(progress, int):
    progress = float(progress)
  if not isinstance(progress, float):
    progress = 0
    status = "error: progress var must be float\r\n"
  if progress < 0:
    progress = 0
    status = "Halt...\r\n"
  if progress >= 1:
    progress = 1
    status = "Done...\r\n"
  block = int(round(barLength*progress))
  text = "\rUploading: [{0}] {1}% {2}".format( "="*block + " "*(barLength-block), int(progress*100), status)
  sys.stderr.write(text)
  sys.stderr.flush()

def serve(remoteAddr, localAddr, remotePort, localPort, password, filename):
  # Create a TCP/IP socket
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  server_address = (localAddr, localPort)
  logging.info('Starting on %s:%s', str(server_address[0]), str(server_address[1]))
  try:
    sock.bind(server_address)
    sock.listen(1)
  except:
    logging.error("Listen Failed")
    return 1

  content_size = os.path.getsize(filename)
  f = open(filename,'rb')
  file_md5 = hashlib.md5(f.read()).hexdigest()
  f.close()
  logging.info('Upload size: %d', content_size)
  message = '0 %d %d %s\n' % (localPort, content_size, file_md5)

  # Wait for a connection
  inv_trys = 0
  data = ''
  msg = 'Sending invitation to %s ' % (remoteAddr)
  sys.stderr.write(msg)
  sys.stderr.flush()
  while (inv_trys < 10):
    inv_trys += 1
    sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    remote_address = (remoteAddr, int(remotePort))
    try:
      sent = sock2.sendto(message.encode(), remote_address)
    except:
      sys.stderr.write('failed\n')
      sys.stderr.flush()
      sock2.close()
      logging.error('Host %s Not Found', remoteAddr)
      return 1
    sock2.settimeout(10)
    try:
      data = sock2.recv(37).decode()
      break;
    except:
      sys.stderr.write('.')
      sys.stderr.flush()
      sock2.close()
  sys.stderr.write('\n')
  sys.stderr.flush()
  if (inv_trys == 10):
    logging.error('No response from the ESP')
    return 1
  if (data != "OK"):
    if(data.startswith('AUTH')):
      nonce = data.split()[1]
      cnonce_text = '%s%u%s%s' % (filename, content_size, file_md5, remoteAddr)
      cnonce = hashlib.md5(cnonce_text.encode()).hexdigest()
      passmd5 = hashlib.md5(password.encode()).hexdigest()
      result_text = '%s:%s:%s' % (passmd5 ,nonce, cnonce)
      result = hashlib.md5(result_text.encode()).hexdigest()
      sys.stderr.write('Authenticating...')
      sys.stderr.flush()
      message = '%d %s %s\n' % (200, cnonce, result)
      sock2.sendto(message.encode(), remote_address)
      sock2.settimeout(10)
      try:
        data = sock2.recv(32).decode()
      except:
        sys.stderr.write('FAIL\n')
        logging.error('No Answer to our Authentication')
        sock2.close()
        return 1
      if (data != "OK"):
        sys.stderr.write('FAIL\n')
        logging.error('%s', data)
        sock2.close()
        sys.exit(1);
        return 1
      sys.stderr.write('OK\n')
    else:
      logging.error('Bad Answer: %s', data)
      sock2.close()
      return 1
  sock2.close()

  logging.info('Waiting for device...')
  try:
    sock.settimeout(10)
    connection, client_address = sock.accept()
    sock.settimeout(None)
    connection.settimeout(None)
  except:
    logging.error('No response from device')
    sock.close()
    return 1
  try:
    f = open(filename, "rb")
    update_progress(0)
    offset = 0
    while True:
      chunk = f.read(1024)
      if not chunk: break
      offset += len(chunk)
      update_progress(offset/float(content_size))
      connection.settimeout(10)
      try:
        connection.sendall(chunk)
        res = connection.recv(10)
        lastResponseContainedOK = 'OK' in res.decode()
      except:
        sys.stderr.write('\n')
        logging.error('Error Uploading')
        connection.close()
        f.close()
        sock.close()
        return 1

    if lastResponseContainedOK:
      logging.info('Success')
      connection.close()
      f.close()
      sock.close()
      return 0

    sys.stderr.write('\n')
    logging.info('Waiting for result...')
    try:
      count = 0
      while True:
        count=count+1
        connection.settimeout(60)
        data = connection.recv(32).decode()
        logging.info('Result: %s' ,data)

        if "OK" in data:
          logging.info('Success')
          connection.close()
          f.close()
          sock.close()
          return 0;
        if count == 5:
          logging.error('Error response from device')
          connection.close()
          f.close()
          sock.close()
          return 1
    except e:
      logging.error('No Result!')
      connection.close()
      f.close()
      sock.close()
      return 1

  finally:
    connection.close()
    f.close()

  sock.close()
  return 1
# end serve

def upload_firmware(name, adress, password):
  serve(adress, "0.0.0.0", 3232, random.randint(10000,60000), password, name)

if __name__ == '__main__':
  device_adress = input("Device adress: ")
  device_upload_password = input("OTA password: ").strip()

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
  else:
    selected_file_index = 0
    
  upload_firmware(file_names[selected_file_index].absolute(), device_adress, device_upload_password)
  sys.exit(0)

print("Cant find firmware file in folder!")