
#ifndef __FUTRONIX_INTERFACE_H__
#define __FUTRONIX_INTERFACE_H__

#include "IRManager.h"

class FutronixInterface 
{
  public: 
    FutronixInterface(IRManager* irManager); 
    ~FutronixInterface();

    void begin(); 
    void setScene(int scene); 

  private: 
    IRManager* _irManager; 
};


FutronixInterface::FutronixInterface(IRManager* irManager)
{
  this->_irManager = irManager;
}

FutronixInterface::~FutronixInterface()
{
  delete this->_irManager; 
}

void FutronixInterface::begin()
{
  this->_irManager->begin(); 
}

void FutronixInterface::setScene(int scene)
{
  //this->_irManager->getSender()->sendFutronix(17); 
}

#endif
