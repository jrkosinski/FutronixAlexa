
#ifndef __SEND_LG_H__
#define __SEND_LG_H__

#include <stdlib.h>
#include "espressif/esp_common.h"

#include "send.h" 

#define LG_TICK                       	50U
#define LG_HDR_MARK_TICKS            	160U
#define LG_HDR_MARK                  	(LG_HDR_MARK_TICKS * LG_TICK)
#define LG_HDR_SPACE_TICKS            	80U
#define LG_HDR_SPACE                 	(LG_HDR_SPACE_TICKS * LG_TICK)
#define LG_BIT_MARK_TICKS             	11U
#define LG_BIT_MARK                  	(LG_BIT_MARK_TICKS * LG_TICK)
#define LG_ONE_SPACE_TICKS            	32U
#define LG_ONE_SPACE                 	(LG_ONE_SPACE_TICKS * LG_TICK)
#define LG_ZERO_SPACE_TICKS           	11U
#define LG_ZERO_SPACE                	(LG_ZERO_SPACE_TICKS * LG_TICK)
#define LG_RPT_SPACE_TICKS            	45U
#define LG_RPT_SPACE                 	(LG_RPT_SPACE_TICKS * LG_TICK)
#define LG_MIN_GAP_TICKS             	795U
#define LG_MIN_GAP                   	(LG_MIN_GAP_TICKS * LG_TICK)
#define LG_MIN_MESSAGE_LENGTH_TICKS  	2161U
#define LG_MIN_MESSAGE_LENGTH        	(LG_MIN_MESSAGE_LENGTH_TICKS * LG_TICK)
#define LG32_HDR_MARK_TICKS          	90U
#define LG32_HDR_MARK                	(LG32_HDR_MARK_TICKS * LG_TICK)
#define LG32_HDR_SPACE_TICKS          	89U
#define LG32_HDR_SPACE               	(LG32_HDR_SPACE_TICKS * LG_TICK)
#define LG32_RPT_HDR_MARK_TICKS      	179U
#define LG32_RPT_HDR_MARK            	(LG32_RPT_HDR_MARK_TICKS * LG_TICK)


void sendLG(uint64_t data, uint16_t nbits, uint16_t repeat);


//////////////////////////////////////////////////////////////////////////////////////////////////////////


// ******************************************************************************
// Send an LG formatted message.
//
// Args:
//   data:   The contents of the message you want to send.
//   nbits:  The bit size of the message being sent.
//           Typically LG_BITS or LG32_BITS.
//   repeat: The number of times you want the message to be repeated.
//
// Status: Beta / Should be working.
//
// Notes:
//   LG has a separate message to indicate a repeat, like NEC does.
void sendLG(uint64_t data, uint16_t nbits, uint16_t repeat)
{
	// set IR carrier frequency
  	enableIROut(38, DEFAULT_DUTY);

  	uint16_t repeatHeaderMark = 0;
  	//IRtimer usecTimer = IRtimer();

	// LG (28-bit) protocol.
	repeatHeaderMark = LG_HDR_MARK;
	// header
	//usecTimer.reset();
	mark(LG_HDR_MARK);
	space(LG_HDR_SPACE);
	    	
	// data
	sendData(LG_BIT_MARK, LG_ONE_SPACE, LG_BIT_MARK, LG_ZERO_SPACE, data, nbits, true);
	// footer
	mark(LG_BIT_MARK);
	space(MAX_ARG((uint32_t)(LG_MIN_MESSAGE_LENGTH /*- usecTimer.elapsed()*/), (uint32_t) LG_MIN_GAP));
  	
  	// repeat
  	// protocol has a mandatory repeat-specific code sent after every command.
  	for (uint16_t i = 0; i < repeat; i++) {
	    	//usecTimer.reset();
	    	mark(repeatHeaderMark);
	    	space(LG_RPT_SPACE);
	    	mark(LG_BIT_MARK);
	    	space(MAX_ARG((uint32_t) LG_MIN_MESSAGE_LENGTH /*- usecTimer.elapsed()*/, (uint32_t)LG_MIN_GAP));
  	}
}

#endif


