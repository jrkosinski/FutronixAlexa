#ifndef __SCENE_CALLBACK_HANDLER_H__
#define __SCENE_CALLBACK_HANDLER_H__

#include "FutronixInterface.h"
#include "WemoEmulator.h"

class SceneCallbackHandler : public IWemoCallbackHandler 
{
  private: 
    int _sceneNo; 
    FutronixInterface* _futronix; 
    
  public: 
    SceneCallbackHandler(FutronixInterface* futronix, int sceneNo); 

    virtual void handleCallback(int param); 
}; 


SceneCallbackHandler::SceneCallbackHandler(FutronixInterface* futronix, int sceneNo)
{
  this->_sceneNo = sceneNo;
}

void SceneCallbackHandler::handleCallback(int param)
{
  if (param)
  {
    this->_futronix->setScene(this->_sceneNo); 
  }
}

#endif 
