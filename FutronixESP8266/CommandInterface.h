
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
    
    int _scenes[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };  
    int _commands[20] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x10, 0x1a, 0x1b, 0x1c, 0x3d, 0x37, 0x35, 0x32, 0x36, 0x34, 0x3a };
    
  public: 
    CommandInterface(); 

    void begin(); 
    void test(); 
    void setScene(int scene); 
    static void setSceneOnMainThread(int scene); 
};
/****************************************/


/*---------------------------------------*/
CommandInterface::CommandInterface()
{
}

/*---------------------------------------*/
void CommandInterface::begin()
{
  DEBUG_PRINTLN("CommandInterface:begin"); 
  this->_enabled = true; 
  this->_irTransmitter.begin();
}

/*---------------------------------------*/
void CommandInterface::test()
{
  DEBUG_PRINTLN("CommandInterface:test"); 
}

/*---------------------------------------*/
void CommandInterface::setScene(int scene)
{
  DEBUG_PRINTLN(String("CommandInterface:set scene ") + scene); 
  unsigned short command = 0; 
  if (scene >=0 && scene < 20)
  {
    command = this->_commands[scene]; 
  }
  else
  {
    if (scene == -1)
      command = 0x02; 
    else if (scene = 255)
      command = 0x3f; 
  }

  if (command != 0)
    this->_irTransmitter.sendFutronix(command); 
}

/*---------------------------------------*/
void CommandInterface::setSceneOnMainThread(int scene)
{
  //TODO: don't do it this way
  SCENE_TO_SET = scene;
}

#endif
