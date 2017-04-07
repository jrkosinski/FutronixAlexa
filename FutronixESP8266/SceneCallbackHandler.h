#ifndef __SCENE_CALLBACK_HANDLER_H__
#define __SCENE_CALLBACK_HANDLER_H__

#include "CommandInterface.h"
#include "WemoEmulator.h"
#include "DatabaseManager.h"

/****************************************
 * SceneNumberCallbackHandler
 * --------------------------
 */
class SceneNumberCallbackHandler : public IWemoCallbackHandler 
{
  private: 
    int _sceneNo; 
    CommandInterface* _command; 
    
  public: 
    SceneNumberCallbackHandler(CommandInterface* command, int sceneNo); 

    virtual void handleCallback(int param); 
}; 
/****************************************/

/****************************************
 * SceneNameCallbackHandler
 * ------------------------
 */
class SceneNameCallbackHandler : public IWemoCallbackHandler 
{
  private: 
    char* _sceneName; 
    DatabaseManager* _dbManager; 
    CommandInterface* _command; 
    
  public: 
    SceneNameCallbackHandler(CommandInterface* command, DatabaseManager* dbManager, char* sceneName); 

    virtual void handleCallback(int param); 
}; 
/****************************************/


/*---------------------------------------*/
SceneNumberCallbackHandler::SceneNumberCallbackHandler(CommandInterface* command, int sceneNo)
{
  this->_sceneNo = sceneNo;
  this->_command = command; 
}

/*---------------------------------------*/
void SceneNumberCallbackHandler::handleCallback(int param)
{
  if (param)
  {
    this->_command->setScene(this->_sceneNo); 
  }
}

/*---------------------------------------*/
SceneNameCallbackHandler::SceneNameCallbackHandler(CommandInterface* command, DatabaseManager* dbManager, char* sceneName)
{
  this->_sceneName = _sceneName;
  this->_dbManager = dbManager; 
  this->_command = command; 
}

/*---------------------------------------*/
void SceneNameCallbackHandler::handleCallback(int param)
{
  if (param)
  {    
    int sceneNo = -1; 
    DatabaseRecord* record = this->_dbManager->getRecordByName(this->_sceneName); 
    if (record != 0)
    {
      sceneNo = record->getSceneNumber(); 
      this->_command->setScene(sceneNo); 
    }
  }
}

#endif 
