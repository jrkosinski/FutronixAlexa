
#ifndef __FUTRONIX_INTERFACE_H__
#define __FUTRONIX_INTERFACE_H__

#include "IRManager.h"

class FutronixInterface 
{
  public: 
    FutronixInterface(); 

    void begin(); 
    void setScene(int scene); 

  private: 
    IRManager _irManager; 
    bool _enabled = false; 
};


FutronixInterface::FutronixInterface()
{
}

void FutronixInterface::begin()
{
  this->_enabled = true; 
}

void FutronixInterface::setScene(int scene)
{
  //this->_irManager->getSender()->sendFutronix(17); 
}

#endif
