#define WRITE_EN 53
#define OUTPUT_ENABLE 51
//The address and data buses. LSB-first.
const char ADDR[] = {52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22};
const char DATA[] = {45, 43, 41, 39, 37, 35, 33, 31};
bool logger = false;

void setAddress(int address) {
  for(int i = 0; i < 16; i++)
  {
    //For each cycle of i, grab the last bit. So on first cycle i=1 so get the
    //least significant bit. Write it to the least significant address line. Then take the
    //commanded address, lop off the bit you just dealt with, and repeat, writing to ADDR 1.
    int ourbit = (address & 000000000001);
    digitalWrite(ADDR[i], ourbit);
    address = address >> 1;
  }
}
void setData(int byte_to_set) {
  setDataBusMode(OUTPUT);
  for(int i = 0; i < 8; i++)
  {
    int ourbit = (byte_to_set & 1);
    digitalWrite(DATA[i], ourbit);
    byte_to_set = byte_to_set >> 1;
  }
}
void setDataBusMode(int mode){
  for(int j =0; j<8; j++)
  {
    pinMode(DATA[j], mode);
  }
}
void setAddressBusMode(int mode){
  for(int i=0; i<16; i++)
  {
    pinMode(ADDR[i], mode);
  }
}
//These handle the active-low nature of the OE pin
void enable_EEPROM_OUTPUT(){
  digitalWrite(OUTPUT_ENABLE, LOW);
}
void disable_EEPROM_OUTPUT(){
  digitalWrite(OUTPUT_ENABLE, HIGH);
}
void readiness_check(){
  Serial.println("DO NOT WRITE TO EEPROM WHILE COMPUTER IS POWERED!");
  Serial.println("DID YOU REMOVE THE 5V POWER FROM THE REST OF THE COMPUTER???");
  Serial.println("MUST TYPE \"YES\" TO BE ABLE TO PROGRAM EEPROM.");
  while(1)
  {
    if (Serial.available() > 0){
      delay(1);
      String input = "";
      while(Serial.available() > 0){
        char byte_read = Serial.read();
        input += byte_read;
      }
      if(input == "YES\n"){
        Serial.println("\n");
        Serial.println("Cool. Also, make sure the chip enable line (white wire running to NAND gate) is tied low.");
        return;
      }
      else{
        Serial.println("=========EEPROM WRITE ABORTED!!!!!=========");
        while(1){}
      }
    }
  }
}

void eraseEEPROM(){
  //Erase entire EEPROM
  Serial.print("Erasing EEPROM");
  for (int address = 0; address <= 2047; address += 1) {
    writeEEPROM(address, 0xff);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");

}
byte readEEPROM(int address) {
    setDataBusMode(INPUT);
    setAddress(address);
    enable_EEPROM_OUTPUT();
    byte data = 0;
    for (int k = 7; k >= 0; k--) {
      int thisbit = digitalRead(DATA[k]);
      data = (data << 1) + thisbit;
    }
    disable_EEPROM_OUTPUT();
    return data;  

}
void writeEEPROM(int address, byte byte_to_write) {
  //If we're writing data then we HAVE to make sure the chip isn't outputting.
  disable_EEPROM_OUTPUT();
  delay(1);
  setAddress(address);
  setData(byte_to_write);
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents() {
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }
    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}
void setup() {
  Serial.begin(57600);

  readiness_check();
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  pinMode(OUTPUT_ENABLE, OUTPUT);
  disable_EEPROM_OUTPUT();
  setAddressBusMode(OUTPUT);
  
  eraseEEPROM();
  printContents();
  logger = true;
  writeEEPROM(0x60, 0xB3);
  writeEEPROM(0x81, 0xA7);
  printContents();

}
void loop() {
  
}
