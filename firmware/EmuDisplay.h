// TODO -- adjustable x and y scale per profile
// TODO -- temperature history table (ttbuffer) that fills itself according to the
// adjustable x scale of the current profile

// Probably nice stats like peak temperature, time above critical temperature, etc


void sendData(byte b[], int t[]) {
  int r = freeRam();
  for (byte i = 0; i < 6; i++) {
    buff[i] = b[i];

    buff[i * 2 + 6] = t[i] & 255;
    buff[i * 2 + 6 + 1] = t[i] >> 8;
  }
  
  Serial.write(253);
  Serial.write(buff, commandIndex + HEADER + 1);
  //Serial.flush();
  Serial.write(255);
  Serial.write(byte(r & 255));
  Serial.write(byte(r >> 8));
  
  //  Serial.println(commandIndex + HEADER);
  //  Serial.println(buff[commandIndex + HEADER]);
  commandIndex = HEADER;
  buff[HEADER] = 255;
}
