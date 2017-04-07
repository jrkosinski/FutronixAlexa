
#ifndef __DATABASE_MANAGER_H__
#define __DATABASE_MANAGER_H__

#include "EEPROMManager.h"

#define RECORD_FIXED_SIZE   50
#define MAX_SCENES          12


/****************************************
 * DatabaseRecord
 * --------------
 * One database fixed-length record. 
 * Consists of an integer index, and a string of data. 
 */
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

    void setData(const char* data)
    {
      int len = strlen(data);
      if (len > (RECORD_FIXED_SIZE-1))
      {
        len = RECORD_FIXED_SIZE-1; 
      }

      memcpy(this->_data, data, len);
      this->_data[len+1] = 0; 
    }
    
    char* getData() { return this->_data; }
    int getSceneNumber() { return this->Index+1;}
};
/****************************************/

/****************************************
 * DatabaseManager
 * ---------------
 */
class DatabaseManager 
{
  private: 
    bool _enabled = false; 
    EEPROMManager _eeprom;
    DatabaseRecord _allRecords[MAX_SCENES];
  
  public: 
    DatabaseManager(); 

    unsigned int getRecordCount(); 

    /***
     * Reads a single record off of EEPROM.
     */
    DatabaseRecord* getRecord(unsigned int recordIndex); 
    
    /***
     * Finds a record in program memory, with the specified name, 
     * or returns NULL if none found. 
     */
    DatabaseRecord* getRecordByName(const char* name);
    
    /***
     * Writes a single record to EEPROM.
     */
    void setRecord(unsigned int recordIndex, const char* data); 

    /***
     * Retrieves entire contents of EEPROM DB, and returns a 
     * pointer to its in-memory representation. 
     */
    DatabaseRecord* getAllRecords(); 

    void begin(); 

    /***
     * Takes entire contents of records stored in program memory, and 
     * writes it out to EEPROM. 
     */
    void save();

    /***
     * Clears EEPROM DB, but retains what's currently in program memory unless
     * specified to clear it. 
     */
    void clear(bool clearProgramMemory);
    void test(); 
};
/****************************************/


/*---------------------------------------*/
DatabaseManager::DatabaseManager()
{
  for(int n=0; n<MAX_SCENES; n++)
    this->_allRecords[n].Index = n; 
}

/*---------------------------------------*/
unsigned int DatabaseManager::getRecordCount()
{
  if (!this->_enabled)
    return 0;
    
  return MAX_SCENES; 
}

/*---------------------------------------*/
DatabaseRecord* DatabaseManager::getRecord(unsigned int recordIndex)
{
  if (recordIndex >= MAX_SCENES)
    recordIndex = (MAX_SCENES-1); 

  char buffer[RECORD_FIXED_SIZE];
  this->_eeprom.readRaw(buffer, (recordIndex * RECORD_FIXED_SIZE), RECORD_FIXED_SIZE); 
  this->_allRecords[recordIndex].setData(buffer); 
  return &(this->_allRecords[recordIndex]); 
}

/*---------------------------------------*/
DatabaseRecord* DatabaseManager::getRecordByName(const char* name)
{
  if (this->_enabled)
  {
    for(int n=0; n<MAX_SCENES; n++)
    {
      if (strcmp(this->_allRecords[n].getData(), name))
        return &this->_allRecords[n];
    }
  }
  
  return NULL; 
}

/*---------------------------------------*/
void DatabaseManager::setRecord(unsigned int recordIndex, const char* data)
{
  if (!this->_enabled)
    return;
    
  if (recordIndex >= MAX_SCENES)
    recordIndex = (MAX_SCENES-1); 
    
  this->_allRecords[recordIndex].setData(data); 
  this->_eeprom.writeString(this->_allRecords[recordIndex].getData(), recordIndex); 
}

/*---------------------------------------*/
DatabaseRecord* DatabaseManager::getAllRecords()
{
  if (!this->_enabled)
    return NULL;
    
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

/*---------------------------------------*/
void DatabaseManager::begin()
{
  DEBUG_PRINTLN("DB: begin"); 
  this->_enabled = true; 
  this->_eeprom.begin();
  this->getAllRecords();
}

/*---------------------------------------*/
void DatabaseManager::clear(bool clearProgramMemory)
{
  if (!this->_enabled)
    return;
    
  DEBUG_PRINTLN("DB: Clearing"); 
  this->_eeprom.clear();

  if (clearProgramMemory)
  {
    for(int n=0; n<MAX_SCENES; n++)
    {
      this->_allRecords[n].setData(""); 
    }
  }
}

/*---------------------------------------*/
void DatabaseManager::save()
{
  if (!this->_enabled)
    return;
    
  DEBUG_PRINTLN("DB: Saving"); 

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

/*---------------------------------------*/
void DatabaseManager::test()
{
  this->_eeprom.clear(); 
  delay(1000); 
  this->_eeprom.writeString("this is a string"); 
  delay(1000); 

  char buffer[512]; 
  this->_eeprom.readAllString(buffer); 
  DEBUG_PRINTLN(buffer); 
  delay(1000); 
}


#endif