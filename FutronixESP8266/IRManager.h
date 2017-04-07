
#ifndef __IR_MANAGER_H__
#define __IR_MANAGER_H__

#define IR_PIN              0
#define FUTRONIX_INTERVAL   1560

class IRManager
{
  public:
    IRManager();
    ~IRManager();

    void begin();   
    void test(int repeat);
    void sendFutronix(unsigned long command); 

  private: 
    int _pin; 
    bool _enabled; 
    int _halfPeriodicTime;

    void pinOn();
    void pinOff(); 
    void enableIROut(int khz);
    void mark(unsigned int usec);
    void space(unsigned long usec);
    void doSendFutronix(unsigned long command, int headerType, int footerType, bool reverse, int bitmark);
};


class IRtimer 
{
  public:
    IRtimer();
    void reset();
    uint32_t elapsed();
    
  private:
    uint32_t start;
};

// IRtimer ---------------------------------------------------------------------
IRtimer::IRtimer() {
  reset();
}

void IRtimer::reset() {
  start = micros();
}

uint32_t IRtimer::elapsed() {
  uint32_t now = micros();
  if (start <= now)  // Check if the system timer has wrapped.
    return (now - start);  // No wrap.
  else
    return (0xFFFFFFFF - start + now);  // Has wrapped.
}

// IRManager ---------------------------------------------------------------------
IRManager::IRManager()
{
  this->_pin = IR_PIN; 
  this->_enabled = false; 
}

IRManager::~IRManager()
{
}

void IRManager::begin()
{
  this->_enabled = true; 
  Serial.print("Setting ir pin to OUTPUT - pin ");
  Serial.println(IR_PIN); 
  pinMode(IR_PIN, OUTPUT);
}

void IRManager::test(int repeat)
{
  for(int n=0; n<repeat; n++)
  {
    Serial.println("testing"); 
    this->mark(1000);
    this->pinOff(); 
    delay(4000); 
  }
}

void IRManager::sendFutronix(unsigned long command) 
{
  if (this->_enabled)
  {
    int footerType = 0; 
    int headerType = 0; 
    
    for(int i=0; i<2; i++)
    {
      headerType = i; 
      
      for(int n=0; n<3; n++)
      {
        footerType = n; 
        
        bool reverse = false;
        doSendFutronix(command, headerType, footerType, reverse, 700); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 750); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 767); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 769); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 800); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 900); 
        delay(2000); 
        
        reverse = true;
        doSendFutronix(command, headerType, footerType, reverse, 700); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 750); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 767); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 769); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 800); 
        delay(2000); 
        doSendFutronix(command, headerType, footerType, reverse, 900); 
        delay(2000); 
      }
    }
  }
}

void IRManager::mark(unsigned int usec) 
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
    Serial.println("one cycle");
  }
}

void IRManager::pinOn() 
{
  digitalWrite(IR_PIN, LOW);
}

void IRManager::pinOff() 
{
  digitalWrite(IR_PIN, HIGH);
}

/* Leave pin off for time (given in microseconds) */
void IRManager::space(unsigned long time) 
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

void IRManager::enableIROut(int khz) 
{
  // Enables IR output.
  // The khz value controls the modulation frequency in kilohertz.

  // T = 1/f but we need T/2 in microsecond and f is in kHz
  _halfPeriodicTime = 500/khz;
}

void IRManager::doSendFutronix(unsigned long command, int headerType, int footerType, bool reverse, int bitmark) {
  
  // Set IR carrier frequency
  enableIROut(38);
  
  Serial.print("Setting scene "); //, bitmark %d, headerType %d, footerType %d", bitmark, headerType, footerType); 
  Serial.println(bitmark); 
  
  //header
  #ifdef __DEBUG_PRINT__
  Serial.println("\nheader:"); 
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
  Serial.println("\nbody:"); 
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
  Serial.println("\nparity:"); 
  #endif
  if (onesCount %2 == 0) {
      mark(FUTRONIX_BITMARK);
    space((FUTRONIX_INTERVAL * 2) - FUTRONIX_BITMARK); 
  }
  else{
      mark(FUTRONIX_BITMARK);
    space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 
  }
  */
  
  //footer
  #ifdef __DEBUG_PRINT__
  Serial.println("\nfooter:"); 
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
  
  Serial.println("------------------");
}

#endif
