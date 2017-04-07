
#ifndef __DATABASE_MANAGER_H__
#define __DATABASE_MANAGER_H__

#include "EEPROMManager.h"

#define RECORD_FIXED_SIZE   50
#define MAX_SCENES          12


class DatabaseRecord
{
  private: 
    int _index; 
    char _data[RECORD_FIXED_SIZE]; 
    
  public: 
    int Index; 

    DatabaseRecord() { 
      this->_index = 0; 
      this->setData("");
    }

    void setData(char* data)
    {
      int len = strlen(data);
      if (len > (RECORD_FIXED_SIZE-1))
      {
        data[RECORD_FIXED_SIZE-1] = 0; 
        len = RECORD_FIXED_SIZE-1; 
      }

      memcpy(this->_data, data, len);
      this->_data[len+1] = 0; 
    }
    
    char* getData() { return this->_data; }
    int getSceneNumber() { return this->Index+1;}
};

class DatabaseManager 
{
  private: 
    bool _enabled = false; 
    EEPROMManager _eeprom;
    DatabaseRecord _allRecords[MAX_SCENES];
  
  public: 
    DatabaseManager(); 

    unsigned int getRecordCount(); 
    
    DatabaseRecord* getRecord(unsigned int recordIndex); 
    DatabaseRecord* getRecordByName(char* name);
    void setRecord(unsigned int recordIndex, char* data); 
    DatabaseRecord* getAllRecords(); 

    void begin(); 
    void save();
    void clear();
    void test(); 
};


DatabaseManager::DatabaseManager()
{
  for(int n=0; n<MAX_SCENES; n++)
    this->_allRecords[n].Index = n; 
}

unsigned int DatabaseManager::getRecordCount()
{
  return MAX_SCENES; 
}

DatabaseRecord* DatabaseManager::getRecord(unsigned int recordIndex)
{
  if (recordIndex >= MAX_SCENES)
    recordIndex = (MAX_SCENES-1); 

  char buffer[RECORD_FIXED_SIZE];
  this->_eeprom.readRaw(buffer, (recordIndex * RECORD_FIXED_SIZE), RECORD_FIXED_SIZE); 
  this->_allRecords[recordIndex].setData(buffer); 
  return &(this->_allRecords[recordIndex]); 
}

DatabaseRecord* DatabaseManager::getRecordByName(char* name)
{
  for(int n=0; n<MAX_SCENES; n++)
  {
    if (strcmp(this->_allRecords[n].getData(), name))
      return &this->_allRecords[n];
  }

  return 0; 
}

void DatabaseManager::setRecord(unsigned int recordIndex, char* data)
{
  if (recordIndex >= MAX_SCENES)
    recordIndex = (MAX_SCENES-1); 

  if (strlen(data) > RECORD_FIXED_SIZE)
    data[RECORD_FIXED_SIZE-1] = 0; 
    
  this->_allRecords[recordIndex].setData(data); 
  this->_eeprom.writeString(this->_allRecords[recordIndex].getData(), recordIndex); 
}

DatabaseRecord* DatabaseManager::getAllRecords()
{
  int len = MAX_SCENES * RECORD_FIXED_SIZE;
  char buffer[len + 1]; 
  buffer[len+1] = 0; 
  char* pBuf = (char*)buffer; 
  
  this->_eeprom.readRaw(buffer, len); 

  for(int n=0; n<MAX_SCENES; n++)
  {
    this->_allRecords[n].Index = n; 
    pBuf += (RECORD_FIXED_SIZE * sizeof(char)); 
    this->_allRecords[n].setData(pBuf); 
  }
  
  return this->_allRecords; 
}

void DatabaseManager::begin()
{
  Serial.println("DB: begin"); 
  this->_enabled = true; 
  this->_eeprom.begin();
  this->getAllRecords();
}

void DatabaseManager::clear()
{
  Serial.println("DB: Clearing"); 
  this->_eeprom.clear();
}

void DatabaseManager::save()
{
  Serial.println("DB: Saving"); 

  int len = MAX_SCENES * RECORD_FIXED_SIZE;
  char buffer[len]; 
  char* pBuf = buffer; 

  for(int n=0; n<MAX_SCENES; n++)
  {
    memcpy(pBuf, this->_allRecords[n].getData(), RECORD_FIXED_SIZE); 
    pBuf += (RECORD_FIXED_SIZE * sizeof(char)); 
  }

  this->_eeprom.write(buffer, len); 
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
