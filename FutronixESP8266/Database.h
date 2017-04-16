
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "EEPROMDatabase.h" 

#define RECORD_FIXED_SIZE   50
#define MAX_SCENES          5

#define WIFI_SSID_INDEX     (MAX_SCENES+0)
#define WIFI_PASSWD_INDEX   (MAX_SCENES+1)
#define MAGIC_NUMBER_INDEX  (MAX_SCENES+2)
#define RECORD_COUNT        (MAX_SCENES+3)

#define MAGIC_WORD "Futronix:012345678998765432102017:Futronix" 

/****************************************
 * Database
 * --------
 */
class Database 
{
  private: 
    bool _enabled = false; 
    EEPROMDatabase _eeprom;
  
  public: 
    Database(); 

    unsigned int getSceneCount(); 
    char* getWifiSsid();
    char* getWifiPasswd(); 
    bool hasBeenSetUp(); 
    void setWifiSsid(const char* data);
    void setWifiPasswd(const char* data); 

    /***
     * Reads a single scene name off of EEPROM.
     */
    char* getSceneName(unsigned int sceneNumber); 
    
    /***
     * Finds a scene in program memory, with the specified name, 
     * or returns NULL if none found. 
     */
    int getSceneNumber(const char* name);
    
    /***
     * Writes a single scene name to the DB. 
     */
    void setSceneName(unsigned int sceneNumber, const char* name); 

    /***
     * Begin. 
     */
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
    
    /***
     * Debug-print entire current DB contents. 
     */
    void debugDump(); 
};
/****************************************/


/*---------------------------------------*/
Database::Database() {}

/*---------------------------------------*/
unsigned int Database::getSceneCount()
{
  if (!this->_enabled)
    return 0;
    
  return MAX_SCENES; 
}

/*---------------------------------------*/
char* Database::getWifiSsid()
{
  if (!this->_enabled)
    return NULL;
    
  DatabaseEntry* record = this->_eeprom.getEntry(WIFI_SSID_INDEX); 
  if (record != NULL)
    return record->getData(); 
  else
    return NULL; 
}

/*---------------------------------------*/
char* Database::getWifiPasswd()
{
  if (!this->_enabled)
    return NULL;
    
  DatabaseEntry* record = this->_eeprom.getEntry(WIFI_PASSWD_INDEX); 
  if (record != NULL)
    return record->getData(); 
  else
    return NULL; 
}

/*---------------------------------------*/
bool Database::hasBeenSetUp()
{
  if (!this->_enabled)
    return false;
    
  DatabaseEntry* entry = this->_eeprom.getEntry(MAGIC_NUMBER_INDEX, true); 
  return (strcmp(MAGIC_WORD, entry->getData()) == 0);
}

/*---------------------------------------*/
void Database::setWifiSsid(const char* data)
{
  if (!this->_enabled)
    return;
    
  DEBUG_PRINTLN(String("set wifi ssid:") + data); 
  
  DatabaseEntry* record = this->_eeprom.getEntry(WIFI_SSID_INDEX); 
  if (record != NULL)
    record->setData(data); 
}

/*---------------------------------------*/
void Database::setWifiPasswd(const char* data)
{
  if (!this->_enabled)
    return;
    
  DEBUG_PRINTLN(String("set wifi passwd:") + data); 
    
  DatabaseEntry* record = this->_eeprom.getEntry(WIFI_PASSWD_INDEX); 
  if (record != NULL)
    record->setData(data);
}

/*---------------------------------------*/
char* Database::getSceneName(unsigned int sceneNumber)
{
  if (this->_enabled)
  {
    DatabaseEntry* record = this->_eeprom.getEntry(sceneNumber); 
    if (record != NULL)
      return record->getData();    

    return NULL;
  }
}

/*---------------------------------------*/
int Database::getSceneNumber(const char* name)
{
  if (this->_enabled)
  {
    for(int n=0; n<MAX_SCENES; n++)
    {
      DatabaseEntry* entry = this->_eeprom.getEntry(n); 
      if (entry != NULL)
      {
        if (strcmp(name, entry->getData()) == 0)
          return n; 
      }
    }
  }
  
  return -1; 
}

/*---------------------------------------*/
void Database::begin()
{
  DEBUG_PRINTLN("Database:begin"); 
  
  this->_enabled = true; 
  this->_eeprom.begin();

  DatabaseEntry entriesConfig[RECORD_COUNT]; 
  for(int n=0; n<RECORD_COUNT; n++)
  {
    entriesConfig[n].index = n;
    entriesConfig[n].fixedSize = RECORD_FIXED_SIZE; 
  }
  
  this->_eeprom.configure(entriesConfig, RECORD_COUNT); 
  this->_eeprom.readAllEntries();
}

/*---------------------------------------*/
void Database::clear(bool clearProgramMemory)
{
  if (!this->_enabled)
    return;
    
  DEBUG_PRINTLN("Database:clear");  

  this->_eeprom.clear(clearProgramMemory);
}

/*---------------------------------------*/
void Database::save()
{
  if (!this->_enabled)
    return;
    
  DEBUG_PRINTLN("Database:save"); 

  this->_eeprom.save();
}

/*---------------------------------------*/
void Database::debugDump()
{
  this->_eeprom.debugDump();
}

#endif 
