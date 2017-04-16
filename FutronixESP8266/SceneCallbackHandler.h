#ifndef __SCENE_CALLBACK_HANDLER_H__
#define __SCENE_CALLBACK_HANDLER_H__

#include "CommandInterface.h"
#include "WemoEmulator.h"
#include "Database.h"

/****************************************
 * SceneCallbackHandler
 * --------------------
 */
class SceneCallbackHandler : public IWemoCallbackHandler 
{
  private: 
    int _sceneNo; 
    
  public: 
    SceneCallbackHandler(int sceneNo); 

    virtual void handleCallback(int param); 
}; 
/****************************************/


/*---------------------------------------*/
SceneCallbackHandler::SceneCallbackHandler(int sceneNo)
{
  this->_sceneNo = sceneNo;
}

/*---------------------------------------*/
void SceneCallbackHandler::handleCallback(int param)
{
  if (param)
  {
    CommandInterface::setSceneOnMainThread(this->_sceneNo); 
  }
}

#endif 
