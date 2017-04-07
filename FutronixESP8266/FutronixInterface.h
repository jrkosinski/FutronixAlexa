
#ifndef __FUTRONIX_INTERFACE_H__
#define __FUTRONIX_INTERFACE_H__

#include "IRManager.h"

class FutronixInterface 
{
  private: 
    IRManager _irManager; 
    bool _enabled = false; 
    
  public: 
    FutronixInterface(); 

    void begin(); 
    void test(); 
    void setScene(int scene); 
};


FutronixInterface::FutronixInterface()
{
}

void FutronixInterface::begin()
{
  this->_enabled = true; 
  this->_irManager.begin();
}

void FutronixInterface::test()
{
  this->_irManager.test(1);
}

void FutronixInterface::setScene(int scene)
{
  //this->_irManager->getSender()->sendFutronix(17); 
}

#endif
