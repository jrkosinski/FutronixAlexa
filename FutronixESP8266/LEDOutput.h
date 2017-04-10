
#ifndef __LED_OUTPUT_H__
#define __LED_OUTPUT_H__

#define   LED_PIN   2

/****************************************
 * LEDOutput
 * ---------
 * Controls the LED light on pin 2. 
 */
class LEDOutput
{
  private: 
    bool _enabled; 
    
  public:
    LEDOutput();
    
    void begin(); 
    void on(); 
    void off(); 
    void blink(); 
    void blink(int);     
};
/****************************************/


/*---------------------------------------*/
LEDOutput::LEDOutput()
{
  this->_enabled = false;
}

/*---------------------------------------*/
void LEDOutput::begin()
{
  pinMode(LED_PIN, OUTPUT); 
  this->_enabled = true;
}

/*---------------------------------------*/
void LEDOutput::on()
{
  if (this->_enabled)
  {
    digitalWrite(LED_PIN, LOW);
  }
}

/*---------------------------------------*/
void LEDOutput::off()
{
  if (this->_enabled)
  {
    digitalWrite(LED_PIN, HIGH);
  }
}

/*---------------------------------------*/
void LEDOutput::blink()
{
  this->blink(1); 
}

/*---------------------------------------*/
void LEDOutput::blink(int times)
{
  for(int i=0; i<times; i++)
  {
    this->on(); 
    delay(100); 
    this->off(); 
    delay(300); 
  }
}


#endif
