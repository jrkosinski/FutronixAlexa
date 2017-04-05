
#ifndef __IR_MANAGER_H__
#define __IR_MANAGER_H__

#include <IRremoteESP8266.h>

#define IR_PIN 0

class IRManager
{
  public:
    IRManager();
    ~IRManager();

    IRsend* getSender(); 
    void begin();   
    void test();

  private: 
    int _pin; 
    IRsend* _irSend; 
    bool _enabled; 
};

IRManager::IRManager()
{
  this->_pin = IR_PIN; 
  this->_irSend = new IRsend(this->_pin); 
  this->_enabled = false; 
}

IRManager::~IRManager()
{
  delete this->_irSend;
}

IRsend* IRManager::getSender()
{
  return this->_irSend; 
}

void IRManager::begin()
{
  this->_enabled = true; 
}

void IRManager::test()
{
  
}

#endif
