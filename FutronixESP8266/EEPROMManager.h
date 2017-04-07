
#ifndef __EEPROM_MANAGER_H__
#define __EEPROM_MANAGER_H__

#include <EEPROM.h>

#define EEPROM_SIZE 4096

class EEPROMManager 
{
  private: 
    bool _enabled = false; 
    
  public: 
    EEPROMManager(); 

    void begin(); 
    void readRaw(char* buffer, int len); 
    void readRaw(char* buffer, int start, int len); 
    void readAllRaw(char* buffer); 
    void readString(char* buffer, int len); 
    void readString(char* buffer, int start, int len); 
    void readAllString(char* buffer); 
    void write(char* buffer, int len);
    void write(char* buffer, int start, int len);
    void writeString(char* buffer); 
    void writeString(char* buffer, int start); 
    void clear(); 

  private: 
    void doRead(char* buffer, int start, int len, bool stopOnNull, bool terminateString);
    void doWrite(char* buffer, int start, int len, bool stopOnNull, bool terminateString);
    void open(); 
    void close(); 
};


EEPROMManager::EEPROMManager()
{
}

void EEPROMManager::begin()
{
  this->_enabled = true; 
}

void EEPROMManager::readRaw(char* buffer, int len)
{
  this->readRaw(buffer, 0, len); 
}

void EEPROMManager::readRaw(char* buffer, int start, int len)
{
  this->doRead(buffer, start, len, false, false); 
}

void EEPROMManager::readAllRaw(char* buffer)
{
  this->readRaw(buffer, 0, EEPROM_SIZE); 
}

void EEPROMManager::readString(char* buffer, int len)
{
  this->readString(buffer, 0, len); 
}

void EEPROMManager::readString(char* buffer, int start, int len)
{
  this->doRead(buffer, start, len, true, true); 
}

void EEPROMManager::readAllString(char* buffer)
{
  this->readString(buffer, 0, EEPROM_SIZE+1); 
}

void EEPROMManager::doRead(char* buffer, int start, int len, bool stopOnNull, bool terminateString)
{
  if (this->_enabled)
  {
    this->open();
    
    if (len > EEPROM_SIZE)
      len = EEPROM_SIZE; 

    if (terminateString)
    {
      if (len > EEPROM_SIZE)
        buffer[len] = 0; 
    }
  
    for(int n=0; n<len; n++)
    {
      buffer[n] = EEPROM.read(n); 
      if (stopOnNull && buffer[n] == 0)
        break;
    }
    
    this->close();
  }
}

void EEPROMManager::write(char* buffer, int len)
{
  this->doWrite(buffer, 0, len, false, (len < EEPROM_SIZE)); 
}

void EEPROMManager::writeString(char* buffer)
{
  int len = strlen(buffer);
  this->doWrite(buffer, 0, len, true, (len < EEPROM_SIZE)); 
}

void EEPROMManager::writeString(char* buffer, int start)
{
  int len = strlen(buffer);
  this->doWrite(buffer, start, len, true, (len < EEPROM_SIZE)); 
}

void EEPROMManager::doWrite(char* buffer, int start, int len, bool stopOnNull, bool terminateString)
{
  if (this->_enabled)
  {
    this->open();
    
    if (len > EEPROM_SIZE)
      len = EEPROM_SIZE; 

    for(int n=0; n<len; n++)
    {
      EEPROM.write(n, buffer[n]); 

      if (stopOnNull && buffer[n] == 0)
        break;
    }
    
    this->close();
  }
}

void EEPROMManager::clear()
{
  if (this->_enabled)
  {
    this->open();

    for(int n=0; n<EEPROM_SIZE; n++)
    {
      EEPROM.write(n, 0); 
    }
    
    this->close();
  }
}

void EEPROMManager::open()
{
  if (this->_enabled)
    EEPROM.begin(EEPROM_SIZE);
}

void EEPROMManager::close()
{
  if (this->_enabled)
    EEPROM.end();
}

#endif
