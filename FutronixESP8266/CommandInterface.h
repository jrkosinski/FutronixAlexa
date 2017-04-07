
#ifndef __FUTRONIX_INTERFACE_H__
#define __FUTRONIX_INTERFACE_H__

#include "IRManager.h"

/****************************************
 * CommandInterface
 * ----------------
 * Interface for sending Futronix-specific commands to the Futronix chip. 
 */
class CommandInterface 
{
  private: 
    IRManager _irManager; 
    bool _enabled = false; 
    
  public: 
    CommandInterface(); 

    void begin(); 
    void test(); 
    void setScene(int scene); 
};
/****************************************/


/*---------------------------------------*/
CommandInterface::CommandInterface()
{
}

/*---------------------------------------*/
void CommandInterface::begin()
{
  this->_enabled = true; 
  this->_irManager.begin();
}

/*---------------------------------------*/
void CommandInterface::test()
{
  this->_irManager.test(1);
}

/*---------------------------------------*/
void CommandInterface::setScene(int scene)
{
  //this->_irManager->getSender()->sendFutronix(17); 
}

#endif
