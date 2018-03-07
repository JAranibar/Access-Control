#include <EEPROM.h>

void setup() { 
  EEPROM.write(0, 8);
  EEPROM.write(1, 3);
  EEPROM.write(2, 4);
  EEPROM.write(3, 9);
  EEPROM.write(4, 3);
}


void loop() {
  
}
