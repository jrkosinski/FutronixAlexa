
#ifndef __DATABASE_MANAGER_H__
#define __DATABASE_MANAGER_H__

#include "EEPROMManager.h"

#define RECORD_FIXED_SIZE = 50; 

class DatabaseManager 
{
  private: 
    bool _enabled = false; 
    EEPROMManager _eeprom;
    unsigned int _maxRecords; 
  
  public: 
    DatabaseManager(); 

    unsigned int getRecordCount(); 
    void setRecord(unsigned int recordIndex, char* data); 
    String getRecord(unsigned int recordIndex); 
    void begin(); 
    void test(); 
};


DatabaseManager::DatabaseManager()
{
  this->_maxRecords = (EEPROM_SIZE / 50)-1; 
}

unsigned int DatabaseManager::getRecordCount()
{
  return this->_maxRecords; 
}

void DatabaseManager::setRecord(unsigned int recordIndex, char* data)
{
  if (recordIndex >= this->_maxRecords)
    recordIndex = this->_maxRecords -1;

  this->_eeprom.write(data, (recordIndex * RECORD_FIXED_SIZE), RECORD_FIXED_SIZE); 
}

String DatabaseManager::getRecord(unsigned int recordIndex)
{
  this->_eeprom.read((recordIndex * RECORD_FIXED_SIZE), RECORD_FIXED_SIZE); 
}

unsigned int DatabaseManager::getRecordCount()
{
  return this->_maxRecords; 
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
