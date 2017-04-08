
#ifndef __EEPROM_INTERFACE_H__
#define __EEPROM_INTERFACE_H__

#include <EEPROM.h>

#define EEPROM_SIZE 4096

/****************************************
 * EEPROMInterface
 * ---------------
 * Read/write interface to the EEPROM memory. 
 */
class EEPROMInterface
{
  private: 
    bool _enabled = false; 
    
  public: 
    EEPROMInterface(); 

    void begin(); 
    void readRaw(char* buffer, int len); 
    void readRaw(char* buffer, int start, int len); 
    void readAllRaw(char* buffer); 
    
    void readString(char* buffer, int len); 
    void readString(char* buffer, int start, int len); 
    void readAllString(char* buffer); 
    
    void write(const char* buffer, int len);
    void write(const char* buffer, int start, int len);
    
    void writeString(const char* buffer); 
    void writeString(const char* buffer, int start); 
    void clear(); 

  private: 
    void open(); 
    void close(); 
    void doRead(char* buffer, int start, int len, bool stopOnNull, bool terminateString);
    void doWrite(const char* buffer, int start, int len, bool stopOnNull, bool terminateString);
};
/****************************************/


/*---------------------------------------*/
EEPROMInterface::EEPROMInterface()
{
}

/*---------------------------------------*/
void EEPROMInterface::begin()
{
  this->_enabled = true; 
}

/*---------------------------------------*/
void EEPROMInterface::readRaw(char* buffer, int len)
{
  this->readRaw(buffer, 0, len); 
}

/*---------------------------------------*/
void EEPROMInterface::readRaw(char* buffer, int start, int len)
{
  this->doRead(buffer, start, len, false, false); 
}

/*---------------------------------------*/
void EEPROMInterface::readAllRaw(char* buffer)
{
  this->readRaw(buffer, 0, EEPROM_SIZE); 
}

/*---------------------------------------*/
void EEPROMInterface::readString(char* buffer, int len)
{
  this->readString(buffer, 0, len); 
}

/*---------------------------------------*/
void EEPROMInterface::readString(char* buffer, int start, int len)
{
  this->doRead(buffer, start, len, true, true); 
}

/*---------------------------------------*/
void EEPROMInterface::readAllString(char* buffer)
{
  this->readString(buffer, 0, EEPROM_SIZE+1); 
}

/*---------------------------------------*/
void EEPROMInterface::doRead(char* buffer, int start, int len, bool stopOnNull, bool terminateString)
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

/*---------------------------------------*/
void EEPROMInterface::write(const char* buffer, int len)
{
  this->doWrite(buffer, 0, len, false, (len < EEPROM_SIZE)); 
}

/*---------------------------------------*/
void EEPROMInterface::writeString(const char* buffer)
{
  int len = strlen(buffer);
  this->doWrite(buffer, 0, len, true, (len < EEPROM_SIZE)); 
}

/*---------------------------------------*/
void EEPROMInterface::writeString(const char* buffer, int start)
{
  int len = strlen(buffer);
  this->doWrite(buffer, start, len, true, (len < EEPROM_SIZE)); 
}

/*---------------------------------------*/
void EEPROMInterface::doWrite(const char* buffer, int start, int len, bool stopOnNull, bool terminateString)
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

/*---------------------------------------*/
void EEPROMInterface::clear()
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

/*---------------------------------------*/
void EEPROMInterface::open()
{
  if (this->_enabled)
    EEPROM.begin(EEPROM_SIZE);
}

/*---------------------------------------*/
void EEPROMInterface::close()
{
  if (this->_enabled)
    EEPROM.end();
}

#endif
