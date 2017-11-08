
#ifndef __SEND_H__
#define __SEND_H__

#include <stdlib.h>
#include "espressif/esp_common.h"

#define IR_SEND_PIN  			0

#define DEFAULT_DUTY 			50
#define DEFAULT_USE_OFFSET 		true

#define MAX_ARG(a,b) (a >= b ? a : b)
#define MIN_ARG(a,b) (a <= b ? a : b)


uint16_t onTimePeriod = 0;
uint16_t offTimePeriod = 0;
int8_t periodOffset = 0;


void mark(uint32_t time);
void space (uint32_t time); 
uint32_t calcUSecPeriod(uint32_t hz, bool use_offset); 
void enableIROut(uint32_t khz, uint8_t duty); 
void sendData(	uint16_t onemark, uint32_t onespace,
   		uint16_t zeromark, uint32_t zerospace,
            	uint64_t data, uint16_t nbits, bool MSBfirst);


//////////////////////////////////////////////////////////////////////////////////////////////////////////


// ******************************************************************************
// Calculate the period for a given frequency. (T = 1/f)
//
// Args:
//   freq: Frequency in Hz.
//   use_offset: Should we use the calculated offset or not?
// Returns:
//   nr. of uSeconds.
uint32_t calcUSecPeriod(uint32_t hz, bool use_offset) {
  	if (hz == 0) hz = 1;  // Avoid Zero hz. Divide by Zero is nasty.
  	uint32_t period = (1000000UL + hz/2) / hz;  // The equiv of round(1000000/hz).
  	// Apply the offset and ensure we don't result in a <= 0 value.
  	if (use_offset)
    		return MAX_ARG((uint32_t) 1, period + periodOffset);
  	else
    		return MAX_ARG((uint32_t) 1, period);
}

// ******************************************************************************
// Set the output frequency modulation and duty cycle.
//
// Args:
//   freq: The freq we want to modulate at. Assumes < 1000 means kHz else Hz.
//   duty: Percentage duty cycle of the LED. e.g. 25 = 25% = 1/4 on, 3/4 off.
//
// Note:
//   Integer timing functions & math mean we can't do fractions of
//   microseconds timing. Thus minor changes to the freq & duty values may have
//   limited effect. You've been warned.
void enableIROut(uint32_t freq, uint8_t duty) {
  	// Can't have more than 100% duty cycle.
  	duty = MIN_ARG(duty, (uint8_t) 100);
  	if (freq < 1000)  // Were we given kHz? Supports the old call usage.
    		freq *= 1000;
  	
	uint32_t period = calcUSecPeriod(freq, DEFAULT_USE_OFFSET);
  	// Nr. of uSeconds the LED will be on per pulse.
  	onTimePeriod = (period * duty) / 100;
  	// Nr. of uSeconds the LED will be off per pulse.
  	offTimePeriod = period - onTimePeriod;
}

// ******************************************************************************
// Modulate the IR LED for the given period (usec) and at the duty cycle set.
//
// Args:
//   usec: The period of time to modulate the IR LED for, in microseconds.
// Returns:
//   number of pulses actually sent.
void mark(uint32_t time) {
  	// Sends an IR mark for the specified number of microseconds.
  	// The mark output is modulated at the PWM frequency.
  	////TCCR2A |= _BV(COM2B1); // Enable pin 3 PWM output
  	int loops;
  	int i;
  	loops = time/26;
  	if(loops==0) loops=1;
  	for (i=0;i<loops;i++) {
		gpio_write(IR_SEND_PIN, 0); //Set GPIO12 LO -> IR On
		sdk_os_delay_us(13);  //1M/38K/2
		gpio_write(IR_SEND_PIN, 1); //Set GPIO12 HI -> IR Off
	  	sdk_os_delay_us(13);  //1M/38K/2
  	}
}

// ******************************************************************************
// Turn the pin (LED) off for a given time.
// Sends an IR space for the specified number of microseconds.
// A space is no output, so the PWM output is disabled.
//
// Args:
//   time: Time in microseconds (us).
void space(uint32_t time) {
  	// Sends an IR space for the specified number of microseconds.
  	// A space is no output, so the PWM output is disabled.
	////TCCR2A &= ~(_BV(COM2B1)); // Disable pin 3 PWM output
	gpio_write(IR_SEND_PIN, 1); //Set GPIO12 HI -> IR Off
  	sdk_os_delay_us(time);
}

// ******************************************************************************
// Generic method for sending data that is common to most protocols.
// Will send leading or trailing 0's if the nbits is larger than the number
// of bits in data.
//
// Args:
//   onemark:    Nr. of usecs for the led to be pulsed for a '1' bit.
//   onespace:   Nr. of usecs for the led to be fully off for a '1' bit.
//   zeromark:   Nr. of usecs for the led to be pulsed for a '0' bit.
//   zerospace:  Nr. of usecs for the led to be fully off for a '0' bit.
//   data:       The data to be transmitted.
//   nbits:      Nr. of bits of data to be sent.
//   MSBfirst:   Flag for bit transmission order. Defaults to MSB->LSB order.
void sendData(uint16_t onemark, uint32_t onespace,
                      uint16_t zeromark, uint32_t zerospace,
                      uint64_t data, uint16_t nbits, bool MSBfirst) {
  	if (nbits == 0)  // If we are asked to send nothing, just return.
    		return;

  	if (MSBfirst) {  // Send the MSB first.
    		// Send 0's until we get down to a bit size we can actually manage.
    		while (nbits > sizeof(data) * 8) {
      			mark(zeromark);
      			space(zerospace);
      			nbits--;
    		}		
    
		// Send the supplied data.
    		for (uint64_t mask = 1ULL << (nbits - 1);  mask;  mask >>= 1) {
      			if (data & mask) {  // Send a 1
        			mark(onemark);
        			space(onespace);
      			} 
			else {  // Send a 0
        			mark(zeromark);
        			space(zerospace);
      			}
		}
  	} 
	else {  // Send the Least Significant Bit (LSB) first / MSB last.
		for (uint16_t bit = 0; bit < nbits; bit++, data >>= 1)
		{
	      		if (data & 1) {  // Send a 1
				mark(onemark);
				space(onespace);
	      		} 
			else {  // Send a 0
				mark(zeromark);
				space(zerospace);
			}
		}
	}
}

#endif

