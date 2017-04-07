
#ifndef __DATABASE_MANAGER_H__
#define __DATABASE_MANAGER_H__

#include "EEPROMManager.h"

class DatabaseManager 
{
  public: 
    DatabaseManager(); 

    void begin(); 
    void test(); 

  private: 
    bool _enabled = false; 
    EEPROMManager _eeprom;
};


DatabaseManager::DatabaseManager()
{
}

void DatabaseManager::begin()
{
  this->_enabled = true; 
  this->_eeprom.begin();
}

void DatabaseManager::test()
{
  this->_eeprom.clear(); 
  delay(1000); 
  this->_eeprom.writeString("this is a string"); 
  delay(1000); 

  char buffer[512]; 
  this->_eeprom.readAllString(buffer); 
  Serial.println(buffer); 
  delay(1000); 
}


#endif
