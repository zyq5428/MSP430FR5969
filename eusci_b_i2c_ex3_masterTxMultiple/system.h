
#ifndef SYSTEM_H
#define SYSTEM_H

//-- Includes -----------------------------------------------------------------
#include "typedefs.h"              // type definitions

//-- Enumerations -------------------------------------------------------------
// Error codes
typedef enum{
  NO_ERROR       = 0x00, // no error
  ACK_ERROR      = 0x01, // no acknowledgment error
  CHECKSUM_ERROR = 0x02, // checksum mismatch error
  TIMEOUT_ERROR  = 0x04, // timeout error
  PARM_ERROR     = 0x80, // parameter out of range error
}etError;

//=============================================================================
void SystemInit(void);
//=============================================================================
// Initializes the system
//-----------------------------------------------------------------------------

//=============================================================================
void DelayMicroSeconds(u32t nbrOfUs);
//=============================================================================
// Wait function for small delays.
//-----------------------------------------------------------------------------
// input:  nbrOfUs   wait x times approx. one micro second (fcpu = 8MHz)
// return: -
// remark: smallest delay is approx. 15us due to function call

#endif
