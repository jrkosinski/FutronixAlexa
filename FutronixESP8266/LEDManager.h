
#ifndef __LED_MANAGER_H__
#define __LED_MANAGER_H__

#define   LED_PIN   2

class LEDManager
{
  public:
    LEDManager();
    
    void begin(); 
    void ledOn(); 
    void ledOff(); 
    void blink(); 
    void blink(int);     

  private: 
    bool _enabled; 
};


LEDManager::LEDManager()
{
  this->_enabled = false;
}

void LEDManager::begin()
{
  pinMode(LED_PIN, OUTPUT); 
  this->_enabled = true;
}

void LEDManager::ledOn()
{
  if (this->_enabled)
  {
    digitalWrite(LED_PIN, LOW);
  }
}

void LEDManager::ledOff()
{
  if (this->_enabled)
  {
    digitalWrite(LED_PIN, HIGH);
  }
}

void LEDManager::blink()
{
  this->blink(1); 
}

void LEDManager::blink(int times)
{
  for(int i=0; i<times; i++)
  {
    this->ledOff(); 
    delay(500); 
    this->ledOn(); 
    delay(500); 
  }
}


#endif
