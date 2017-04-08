
#ifndef __FUTRONIX_INTERFACE_H__
#define __FUTRONIX_INTERFACE_H__

#include "IRTransmitter.h"

/****************************************
 * CommandInterface
 * ----------------
 * Interface for sending Futronix-specific commands to the Futronix chip. 
 */
class CommandInterface 
{
  private: 
    IRTransmitter _irTransmitter; 
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
  this->_irTransmitter.begin();
}

/*---------------------------------------*/
void CommandInterface::test()
{
  if (this->_enabled)
  {
  }
}

/*---------------------------------------*/
void CommandInterface::setScene(int scene)
{
  if (this->_enabled)
  {
  }
}

#endif
