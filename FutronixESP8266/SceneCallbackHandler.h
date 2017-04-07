#ifndef __SCENE_CALLBACK_HANDLER_H__
#define __SCENE_CALLBACK_HANDLER_H__

#include "FutronixInterface.h"
#include "WemoEmulator.h"
#include "DatabaseManager.h"

class SceneNumberCallbackHandler : public IWemoCallbackHandler 
{
  private: 
    int _sceneNo; 
    FutronixInterface* _futronix; 
    
  public: 
    SceneNumberCallbackHandler(FutronixInterface* futronix, int sceneNo); 

    virtual void handleCallback(int param); 
}; 


class SceneNameCallbackHandler : public IWemoCallbackHandler 
{
  private: 
    char* _sceneName; 
    DatabaseManager* _dbManager; 
    FutronixInterface* _futronix; 
    
  public: 
    SceneNameCallbackHandler(FutronixInterface* futronix, DatabaseManager* dbManager, char* sceneName); 

    virtual void handleCallback(int param); 
}; 


SceneNumberCallbackHandler::SceneNumberCallbackHandler(FutronixInterface* futronix, int sceneNo)
{
  this->_sceneNo = sceneNo;
}

void SceneNumberCallbackHandler::handleCallback(int param)
{
  if (param)
  {
    this->_futronix->setScene(this->_sceneNo); 
  }
}


SceneNameCallbackHandler::SceneNameCallbackHandler(FutronixInterface* futronix, DatabaseManager* dbManager, char* sceneName)
{
  this->_sceneName = _sceneName;
  this->_dbManager = dbManager; 
}

void SceneNameCallbackHandler::handleCallback(int param)
{
  if (param)
  {    
    int sceneNo = -1; 
    DatabaseRecord* record = this->_dbManager->getRecordByName(this->_sceneName); 
    if (record != 0)
    {
      sceneNo = record->getSceneNumber(); 
      this->_futronix->setScene(sceneNo); 
    }
  }
}

#endif 
