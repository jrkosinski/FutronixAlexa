
#ifndef __SEND_FUTRONIX_H__
#define __SEND_FUTRONIX_H__

#include <stdlib.h>
#include "espressif/esp_common.h"

#include "send.h" 

#define FUTRONIX_INTERVAL   1560
#define FUTRONIX_BITMARK    700
#define FUTRONIX_CMD_GAP    102

void sendLG(uint64_t data, uint16_t nbits, uint16_t repeat);


//////////////////////////////////////////////////////////////////////////////////////////////////////////


// ******************************************************************************
// Send a Futronix protocol formatted message.
//
void sendFutronix(uint16_t data)
{
	enableIROut(38, DEFAULT_DUTY); 
    
	//reverse the 6 command bits
	bool commandBits[6]; 
	uint8_t nbits = 6; 
	uint8_t onesCount = 0; 
	uint8_t index = 0; 
	
	for (uint16_t mask = 1UL << (nbits - 1); mask; mask >>= 1) 
    	{
      		if (data & mask) 
      		{  
        		commandBits[index++] = true; 
        		onesCount++; 
      		} 
      		else
      		{
		        commandBits[index++] = false; 
		}
	}
    
    	for(uint8_t i=0; i<3; i++)
    	{
      		//header
      		mark(FUTRONIX_BITMARK); 
	      	space((FUTRONIX_INTERVAL * 4) - FUTRONIX_BITMARK); 
	      	mark(FUTRONIX_BITMARK); 
      		space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 

      		//address (4 0s) 
      		for(uint8_t n=0; n<4; n++)
      		{
        		mark(FUTRONIX_BITMARK);
        		space((FUTRONIX_INTERVAL) - FUTRONIX_BITMARK); 
      		}
      
      		//command 6 bits
      		uint8_t bitPosition = 5; 
      		for(uint8_t n = 5; n>=0; n--)
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
	  
	      	//this->pinOff();
	      	//delay(FUTRONIX_CMD_GAP);  
	}
}

#endif


