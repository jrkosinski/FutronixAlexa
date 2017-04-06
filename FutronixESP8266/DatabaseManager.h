
#ifndef __DATABASE_MANAGER_H__
#define __DATABASE_MANAGER_H__

#include "EEPROMManager.h"

class DatabaseManager 
{
  public: 
    DatabaseManager(); 

    void begin(); 

  private: 
    bool _enabled = false; 
};


DatabaseManager::DatabaseManager()
{
}

void DatabaseManager::begin()
{
  this->_enabled = true; 
}


#endif
