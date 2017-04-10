
#ifndef __IR_TRANSMITTER_H__
#define __IR_TRANSMITTER_H__

#define IR_PIN              0
#define FUTRONIX_INTERVAL   1560
#define FUTRONIX_BITMARK    700
#define FUTRONIX_CMD_GAP    102

/****************************************
 * IRTransmitter
 * -------------
 * Transmits infrared pulses through a selected pin. 
 */
class IRTransmitter
{
  private: 
    int _pin; 
    bool _enabled; 
    int _halfPeriodicTime;

  public:
    IRTransmitter();
    ~IRTransmitter();

    void begin();   
    void test(int repeat);
    void sendFutronix(unsigned short command); 

  private: 
    void pinOn();
    void pinOff(); 
    void enableIROut(int khz);
    void mark(unsigned int usec);
    void space(unsigned long usec);
};
/****************************************/


/****************************************
 * IRtimer
 * -------
 */
class IRtimer 
{
  public:
    IRtimer();
    void reset();
    uint32_t elapsed();
    
  private:
    uint32_t start;
};
/****************************************/

/*---------------------------------------*/
IRtimer::IRtimer() {
  reset();
}

/*---------------------------------------*/
void IRtimer::reset() {
  start = micros();
}

/*---------------------------------------*/
uint32_t IRtimer::elapsed() {
  uint32_t now = micros();
  if (start <= now)  // Check if the system timer has wrapped.
    return (now - start);  // No wrap.
  else
    return (0xFFFFFFFF - start + now);  // Has wrapped.
}

/*---------------------------------------*/
IRTransmitter::IRTransmitter()
{
  this->_pin = IR_PIN; 
  this->_enabled = false; 
}

/*---------------------------------------*/
IRTransmitter::~IRTransmitter()
{
}

/*---------------------------------------*/
void IRTransmitter::begin()
{
  this->_enabled = true; 
  DEBUG_PRINTLN("Setting ir pin to OUTPUT - pin ");
  DEBUG_PRINTLN(IR_PIN); 
  pinMode(IR_PIN, OUTPUT);
  this->pinOff(); 
}

/*---------------------------------------*/
void IRTransmitter::test(int repeat)
{
  this->pinOff();  
  this->sendFutronix(17); 
  this->pinOff();
}

/*---------------------------------------*/
void IRTransmitter::sendFutronix(unsigned short command) 
{
  if (this->_enabled)
  {
    //reverse the 6 command bits
    bool commandBits[6]; 
    int nbits = 6; 
    int onesCount = 0; 
    int index = 0; 
    for (unsigned short mask = 1UL << (nbits - 1); mask; mask >>= 1) 
    {
      if (command & mask) 
      {  
        // 1
        DEBUG_PRINTLN("one");
        commandBits[index++] = true; 
        onesCount++; 
      } 
      else
      {
        DEBUG_PRINTLN("zero");
        commandBits[index++] = false; 
      }
    }
      
    DEBUG_PRINTLN("Setting scene "); //, bitmark %d, headerType %d, footerType %d", bitmark, headerType, footerType); 
    
    for(int i=0; i<2; i++)
    {
      // Set IR carrier frequency
      enableIROut(38);
      
      //DEBUG_PRINTLN(FUTRONIX_BITMARK); 
      
      //header
      //DEBUG_PRINTLN("\nheader:"); 
      mark(FUTRONIX_BITMARK); 
      space((FUTRONIX_INTERVAL * 4) - FUTRONIX_BITMARK); 
      mark(FUTRONIX_BITMARK); 
      space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 

      //address (4 0s) 
      //DEBUG_PRINTLN("\address:"); 
      for(int n=0; n<4; n++)
      {
        mark(FUTRONIX_BITMARK);
        space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 
      }
      
      //command 6 bits
      //DEBUG_PRINTLN("\command:"); 
      int bitPosition = 5; 
      for(int n = 5; n>=0; n--)
      {
        if (commandBits[n])
        {
          mark(FUTRONIX_BITMARK);
          if (bitPosition % 2 == 0)
            space((FUTRONIX_INTERVAL * 3) - FUTRONIX_BITMARK); 
          else 
            space((FUTRONIX_INTERVAL * 2) - FUTRONIX_BITMARK); 
        }
        else
        {
          mark(FUTRONIX_BITMARK);
          space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 
        }
        
        bitPosition++; 
      }
      
      //parity bit
      //DEBUG_PRINTLN("\nparity:"); 
      if (onesCount %2 == 0) 
      {
        mark(FUTRONIX_BITMARK);
        space((FUTRONIX_INTERVAL * 2) - FUTRONIX_BITMARK); 
      }
      else
      {
        mark(FUTRONIX_BITMARK);
        space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 
      }
      
      //footer
      //DEBUG_PRINTLN("\nfooter:"); 
      mark(FUTRONIX_BITMARK); 
      space((FUTRONIX_INTERVAL * 4) - FUTRONIX_BITMARK); 
      mark(FUTRONIX_BITMARK); 
  
      this->pinOff();
  
      delay(FUTRONIX_CMD_GAP);  
    }
    
    DEBUG_PRINTLN("------------------");
  }
}

/*---------------------------------------*/
void IRTransmitter::mark(unsigned int usec) 
{
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  IRtimer usecTimer = IRtimer();
  
  while (usecTimer.elapsed() < usec) {
    this->pinOn();
    delayMicroseconds(_halfPeriodicTime);
    this->pinOff();
    // e.g. 38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
    delayMicroseconds(_halfPeriodicTime);
    //DEBUG_PRINTLN("one cycle");
  }
}

/*---------------------------------------*/
void IRTransmitter::pinOn() 
{
  digitalWrite(IR_PIN, LOW);
}

/*---------------------------------------*/
void IRTransmitter::pinOff() 
{
  digitalWrite(IR_PIN, HIGH);
}

/*---------------------------------------*/
/* Leave pin off for time (given in microseconds) */
void IRTransmitter::space(unsigned long time) 
{
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  this->pinOff();
  if (time == 0) return;
  if (time <= 16383)  // delayMicroseconds is only accurate to 16383us.
    delayMicroseconds(time);
  else {
    // Invoke a delay(), where possible, to avoid triggering the WDT.
    delay(time / 1000UL);  // Delay for as many whole ms as we can.
    delayMicroseconds((int) time % 1000UL);  // Delay the remaining sub-msecond.
  }
}

/*---------------------------------------*/
void IRTransmitter::enableIROut(int khz) 
{
  // Enables IR output.
  // The khz value controls the modulation frequency in kilohertz.

  // T = 1/f but we need T/2 in microsecond and f is in kHz
  _halfPeriodicTime = 500/khz;
}

/*
void IRTransmitter::doSendFutronix_old(unsigned long command, int headerType, int footerType, bool reverse, int bitmark) 
{
  if (reverse)
    return; 
    
  for(int n=0; n<3; n++)
  {
    // Set IR carrier frequency
    enableIROut(38);
    
    DEBUG_PRINTLN("Setting scene "); //, bitmark %d, headerType %d, footerType %d", bitmark, headerType, footerType); 
    DEBUG_PRINTLN(bitmark); 
    
    //header
    #ifdef __DEBUG_PRINT__
    DEBUG_PRINTLN("\nheader:"); 
    #endif
    switch(headerType)
    {
      case 0: 
        mark(bitmark); 
        space((FUTRONIX_INTERVAL * 4) - bitmark); 
        mark(bitmark); 
        space((FUTRONIX_INTERVAL) - bitmark); 
      break;
      
      case 1: 
        mark(FUTRONIX_INTERVAL * 4); 
      space(FUTRONIX_INTERVAL); 
      break;
    }
    
    //body
    
    //0
    mark(bitmark); 
    space(FUTRONIX_INTERVAL - bitmark); 
    
    //0
    mark(bitmark); 
    space(FUTRONIX_INTERVAL - bitmark); 
    
    //0
    mark(bitmark); 
    space(FUTRONIX_INTERVAL - bitmark); 
    
    //0
    mark(bitmark); 
    space(FUTRONIX_INTERVAL - bitmark); 
    
    
    if (reverse)
    {
      //1
      mark(bitmark); 
      space((FUTRONIX_INTERVAL * 2) - bitmark); 
      
      //1
      mark(bitmark); 
      space((FUTRONIX_INTERVAL * 3) - bitmark); 
      
      //0
      mark(bitmark); 
      space(FUTRONIX_INTERVAL - bitmark); 
      
      //0
      mark(bitmark); 
      space(FUTRONIX_INTERVAL - bitmark); 
      
      //1
      mark(bitmark); 
      space((FUTRONIX_INTERVAL * 2) - bitmark); 
      
      //0
      mark(bitmark); 
      space(FUTRONIX_INTERVAL - bitmark); 
      
      
      //parity bit 
      mark(bitmark); 
      space((FUTRONIX_INTERVAL * 1) - bitmark); 
    }
    else
    {
      //0
      mark(bitmark); 
      space(FUTRONIX_INTERVAL - bitmark); 
      
      //1
      mark(bitmark); 
      space((FUTRONIX_INTERVAL * 3) - bitmark); 
      
      //0
      mark(bitmark); 
      space(FUTRONIX_INTERVAL - bitmark); 
      
      //0
      mark(bitmark); 
      space(FUTRONIX_INTERVAL - bitmark); 
      
      //1
      mark(bitmark); 
      space((FUTRONIX_INTERVAL * 2) - bitmark); 
      
      //1
      mark(bitmark); 
      space((FUTRONIX_INTERVAL * 3) - bitmark); 
      
      
      //parity bit 
      mark(bitmark); 
      space((FUTRONIX_INTERVAL * 1) - bitmark); 
    }
    
    /*
    //body
    #ifdef __DEBUG_PRINT__
    DEBUG_PRINTLN("\nbody:"); 
    #endif
    int onesCount = 0; 
    int bitPosition = 1; 
    int nbits = 10; 
    for (unsigned long mask = 1UL << (nbits - 1); mask; mask >>= 1) {
      if (command & mask) {  // 1
        mark(FUTRONIX_BITMARK);
      if (bitPosition % 2 == 0)
      space((FUTRONIX_INTERVAL * 3) - FUTRONIX_BITMARK); 
      else 
      space((FUTRONIX_INTERVAL * 2) - FUTRONIX_BITMARK); 
    
      onesCount++; 
      } 
    else 
    { // 0
        mark(FUTRONIX_BITMARK);
      space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 
      }
    
    bitPosition++; 
    }
    
    //parity bit
    #ifdef __DEBUG_PRINT__
    DEBUG_PRINTLN("\nparity:"); 
    #endif
    if (onesCount %2 == 0) {
        mark(FUTRONIX_BITMARK);
      space((FUTRONIX_INTERVAL * 2) - FUTRONIX_BITMARK); 
    }
    else{
        mark(FUTRONIX_BITMARK);
      space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 
    }
    -/
    
    //footer
    #ifdef __DEBUG_PRINT__
    DEBUG_PRINTLN("\nfooter:"); 
    #endif
    
    switch(footerType)
    {
      case 0: 
        mark(bitmark); 
        space((FUTRONIX_INTERVAL * 4) - bitmark); 
        mark(bitmark); 
      break;
      
      case 1: 
        space(FUTRONIX_INTERVAL * 4); 
        mark(bitmark); 
      break;
      
      case 2: 
        mark(FUTRONIX_INTERVAL * 4); 
      break;
    }
    this->pinOff();

    delay(102);  
    //DEBUG_PRINTLN("------------------");
  }
}
*/

#endif
