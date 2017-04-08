
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "EEPROMInterface.h"

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
      this->_data[len] = 0; 
    }
    
    char* getData() { return this->_data; }
    int getSceneNumber() { return this->Index+1;}
};
/****************************************/

/****************************************
 * Database
 * --------
 */
class Database 
{
  private: 
    bool _enabled = false; 
    EEPROMInterface _eeprom;
    DatabaseRecord _allRecords[MAX_SCENES];
  
  public: 
    Database(); 

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
Database::Database()
{
  for(int n=0; n<MAX_SCENES; n++)
    this->_allRecords[n].Index = n; 
}

/*---------------------------------------*/
unsigned int Database::getRecordCount()
{
  if (!this->_enabled)
    return 0;
    
  return MAX_SCENES; 
}

/*---------------------------------------*/
DatabaseRecord* Database::getRecord(unsigned int recordIndex)
{
  if (this->_enabled)
  {
    DEBUG_PRINTLN(String("Database:test ") + String(recordIndex)); 
    
    if (recordIndex >= MAX_SCENES)
      recordIndex = (MAX_SCENES-1); 
  
    char buffer[RECORD_FIXED_SIZE];
    this->_eeprom.readRaw(buffer, (recordIndex * RECORD_FIXED_SIZE), RECORD_FIXED_SIZE); 
    this->_allRecords[recordIndex].setData(buffer); 
    return &(this->_allRecords[recordIndex]); 
  }
}

/*---------------------------------------*/
DatabaseRecord* Database::getRecordByName(const char* name)
{
  if (this->_enabled)
  {
    DEBUG_PRINTLN(String("Database:getRecordByName ") + name); 
    
    for(int n=0; n<MAX_SCENES; n++)
    {
      if (strcmp(this->_allRecords[n].getData(), name))
        return &this->_allRecords[n];
    }
  }
  
  return NULL; 
}

/*---------------------------------------*/
void Database::setRecord(unsigned int recordIndex, const char* data)
{
  if (!this->_enabled)
    return;
    
  DEBUG_PRINTLN(String("Database:setRecord ") + String(recordIndex) + " " + data); 
  
  if (recordIndex >= MAX_SCENES)
    recordIndex = (MAX_SCENES-1); 
    
  this->_allRecords[recordIndex].setData(data); 
  this->_eeprom.writeString(this->_allRecords[recordIndex].getData(), recordIndex * RECORD_FIXED_SIZE); 
}

/*---------------------------------------*/
DatabaseRecord* Database::getAllRecords()
{
  if (!this->_enabled)
    return NULL;
    
  DEBUG_PRINTLN("Database:getAllRecords"); 
  
  int len = MAX_SCENES * RECORD_FIXED_SIZE;
  char buffer[len + 1]; 
  buffer[len+1] = 0; 
  char* pBuf = (char*)buffer; 
  
  this->_eeprom.readRaw(buffer, len); 

  for(int n=0; n<MAX_SCENES; n++)
  {
    this->_allRecords[n].Index = n; 
    this->_allRecords[n].setData(pBuf); 
    pBuf += (RECORD_FIXED_SIZE * sizeof(char)); 
    
    DEBUG_PRINTLN(String(this->_allRecords[n].Index) + ":" + this->_allRecords[n].getData()); 
  }
  
  return this->_allRecords; 
}

/*---------------------------------------*/
void Database::begin()
{
  DEBUG_PRINTLN("Database:begin"); 
  
  this->_enabled = true; 
  this->_eeprom.begin();
  this->getAllRecords();
}

/*---------------------------------------*/
void Database::clear(bool clearProgramMemory)
{
  if (!this->_enabled)
    return;
    
  DEBUG_PRINTLN("Database:clear");  
  
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
void Database::save()
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
void Database::test()
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
