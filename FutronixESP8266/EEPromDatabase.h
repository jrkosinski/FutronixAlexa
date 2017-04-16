
#ifndef __EEPROM_DATABASE_H__
#define __EEPROM_DATABASE_H__

#include "EEPROMInterface.h"

#define RECORD_MAX_SIZE   200

/****************************************
 * DatabaseEntry
 * --------------
 * One database fixed-length record. 
 * Consists of an integer index, and a string of data. 
 */
class DatabaseEntry
{
  private: 
    char* _data = NULL;      
    
  public: 
    int startIndex = 0; 
    int index = 0; 
    int fixedSize = 10; 
    bool initialized = false;

    DatabaseEntry() { }
    DatabaseEntry(int index, int fixedSize) { 
      this->index = index;
      this->fixedSize = fixedSize;         
    }
    ~DatabaseEntry() { 
      if (_data != NULL)
        delete _data;
    }    
    
    DatabaseEntry(int index, int fixedSize, const char* data) { 
      this->index = index;
      this->fixedSize = fixedSize;   
      this->setData(data); 
    }
    
    char* getData() { return this->_data; }
    
    void setData(const char* data)
    {
      if (this->fixedSize > 0)
      {
        if (this->_data == NULL)
          this->_data = (char*)malloc(this->fixedSize); 
        
        int len = strlen(data);
        if (len > (this->fixedSize-1))
          len = this->fixedSize-1; 
  
        memcpy(this->_data, data, len);
        this->_data[len]= 0;
      }
    }
};
/****************************************/


/****************************************
 * Database
 * --------
 */
class EEPROMDatabase 
{
  private: 
    EEPROMInterface _eeprom;
    DatabaseEntry* _entries = NULL; 
    int _recordCount = 0; 
    int _totalSize = 0; 
  
  public: 
    EEPROMDatabase(); 
    ~EEPROMDatabase(); 
    
    unsigned int getRecordCount() { return this->_recordCount; }
    void configure(const DatabaseEntry* entries, int count);
    void begin(); 
    
    /***
     * Reads a single record off of EEPROM.
     */
    DatabaseEntry* getEntry(unsigned int recordIndex); 
    DatabaseEntry* getEntry(unsigned int recordIndex, bool forceRefresh); 
    
    /***
     * Writes a single record to EEPROM.
     */
    void setEntry(unsigned int recordIndex, const char* data); 
    void setEntry(unsigned int recordIndex, const char* data, bool save); 

    /***
     * Retrieves entire contents of EEPROM DB, and returns a 
     * pointer to its in-memory representation. 
     */
    DatabaseEntry* readAllEntries(); 

    /***
     * Returns a pointer to in-memory cache. 
     */
    DatabaseEntry* getAllEntries(); 

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
    
    void debugDump(); 
};
/****************************************/


/*---------------------------------------*/
EEPROMDatabase::EEPROMDatabase()
{
}

/*---------------------------------------*/
EEPROMDatabase::~EEPROMDatabase()
{
  delete this->_entries;
}

/*---------------------------------------*/
void EEPROMDatabase::begin()
{
  this->_eeprom.begin();
}

/*---------------------------------------*/
void EEPROMDatabase::configure(const DatabaseEntry* entries, int count)
{
  this->_entries = (DatabaseEntry*)malloc(count * sizeof(DatabaseEntry)); 
  this->_recordCount = count; 
  this->_totalSize = 0; 

  int startIndex = 0; 
  for(int n=0; n<count; n++)
  {
    this->_entries[n].startIndex = startIndex; 
    this->_entries[n].fixedSize = entries[n].fixedSize; 

    //enforce max size 
    if (this->_entries[n].fixedSize > RECORD_MAX_SIZE)
      this->_entries[n].fixedSize = RECORD_MAX_SIZE; 
      
    startIndex += this->_entries[n].fixedSize;
    this->_totalSize += this->_entries[n].fixedSize; 
  }
}

/*---------------------------------------*/
DatabaseEntry* EEPROMDatabase::getEntry(unsigned int recordIndex)
{
  return this->getEntry(recordIndex, false);
}

/*---------------------------------------*/
DatabaseEntry* EEPROMDatabase::getEntry(unsigned int recordIndex, bool forceRefresh)
{
  if (recordIndex > this->_recordCount)
    return NULL; 

  DatabaseEntry* entry = &this->_entries[recordIndex]; 

  //return cached data 
  if (entry->initialized && !forceRefresh)
    return entry; 

  //refresh ffrom eeprom 
  char buffer[RECORD_MAX_SIZE];
  this->_eeprom.readRaw(buffer, (entry->startIndex), (entry->fixedSize)); 
  entry->setData(buffer); 

  return entry; 
}

/*---------------------------------------*/
void EEPROMDatabase::setEntry(unsigned int recordIndex, const char* data)
{
  this->setEntry(recordIndex, data, false); 
}

/*---------------------------------------*/
void EEPROMDatabase::setEntry(unsigned int recordIndex, const char* data, bool save)
{
  if (recordIndex > this->_recordCount)
    return; 

  DatabaseEntry* entry = &this->_entries[recordIndex]; 
  entry->setData(data); 

  //save to EEPROM
  if (save)
  {
    this->_eeprom.writeString(entry->getData(), entry->startIndex); 
  }
}

/*---------------------------------------*/
DatabaseEntry* EEPROMDatabase::readAllEntries()
{
  char buffer[this->_totalSize + 1]; 
  buffer[this->_totalSize] = 0; 
  char* pBuf = (char*)buffer; 
  
  this->_eeprom.readRaw(buffer, this->_totalSize); 

  for(int n=0; n<this->_recordCount; n++)
  {
    this->_entries[n].setData(pBuf); 
    this->_entries[n].initialized = false;
    pBuf += (this->_entries[n].fixedSize * sizeof(char)); 
  }

  this->debugDump();
  return this->_entries; 
}

/*---------------------------------------*/
DatabaseEntry* EEPROMDatabase::getAllEntries()
{
  return this->_entries;
}

/*---------------------------------------*/
void EEPROMDatabase::clear(bool clearProgramMemory)
{
  this->_eeprom.clear();

  if (clearProgramMemory)
  {
    for(int n=0; n<this->_recordCount; n++)
    {
      this->_entries[n].setData(""); 
    }
  }
}

/*---------------------------------------*/
void EEPROMDatabase::save()
{
  int len = this->_totalSize;
  char buffer[len]; 
  char* pBuf = buffer; 

  for(int n=0; n<this->_recordCount; n++)
  {
    memcpy(pBuf, this->_entries[n].getData(), this->_entries[n].fixedSize); 
    pBuf += (this->_entries[n].fixedSize * sizeof(char)); 
  }

  this->_eeprom.write(buffer, len); 
}

/*---------------------------------------*/
void EEPROMDatabase::debugDump()
{
  for(int n=0; n<this->_recordCount; n++)
    DEBUG_PRINTLN(String(this->_entries[n].index) + ":" + this->_entries[n].getData()); 
}


#endif
