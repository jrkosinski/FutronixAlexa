#ifndef __SCENE_CALLBACK_HANDLER_H__
#define __SCENE_CALLBACK_HANDLER_H__

#include "CommandInterface.h"
#include "WemoEmulator.h"
#include "Database.h"

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
    Database* _db; 
    CommandInterface* _command; 
    
  public: 
    SceneNameCallbackHandler(CommandInterface* command, Database* dbManager, char* sceneName); 

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
SceneNameCallbackHandler::SceneNameCallbackHandler(CommandInterface* command, Database* dbManager, char* sceneName)
{
  this->_sceneName = _sceneName;
  this->_db = dbManager; 
  this->_command = command; 
}

/*---------------------------------------*/
void SceneNameCallbackHandler::handleCallback(int param)
{
  if (param)
  {    
    int sceneNo = this->_db->getSceneNumber(this->_sceneName); 
    if (sceneNo >= 0)
    {
      this->_command->setScene(sceneNo); 
    }
  }
}

#endif 
