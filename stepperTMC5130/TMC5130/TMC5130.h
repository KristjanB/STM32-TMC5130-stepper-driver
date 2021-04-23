/*
 * TMC5130.h
 *
 *  Created on: 03.07.2017
 *      Author: LK
 */

#ifndef TMC_IC_TMC5130_H_
#define TMC_IC_TMC5130_H_

#include "main.h"
#include "TMC5130_Register.h"
#include "TMC5130_Constants.h"
#include "TMC5130_Fields.h"
#include "helpers/Bits.h"

// Helper macros
#define TMC5130_FIELD_READ(tdef, address, mask, shift) \
	FIELD_GET(tmc5130_readInt(tdef, address), mask, shift)
#define TMC5130_FIELD_WRITE(tdef, address, mask, shift, value) \
	(tmc5130_writeInt(tdef, address, FIELD_SET(tmc5130_readInt(tdef, address), mask, shift, value)))



typedef enum
{
	STEPDIR = 0,
	INTERNAL,
}TMC5130ControlType;


typedef enum
{
	HOME = 0,
	RETRACT,
	UNRETRACT,
	WAIT_MOVE,
}HomingState_e;


// Typedefs
typedef struct
{
	HomingState_e			home_state;
	HomingState_e			prev_home_state;
	TMC5130ControlType 		control_type;
	SPI_HandleTypeDef	 	*spi;
	int32_t					drvstat;
	int32_t 				sg_flag;
	int32_t 				sg_result;
	int32_t 				cs_actual;
	int32_t 				rampstat;
	uint8_t 				homing_done;

} TMC5130TypeDef;

//TMC5130TypeDef stepperR;
//TMC5130TypeDef stepperL;

// Default Register values
#define R10 0x00071703  // IHOLD_IRUN
#define R3A 0x00010000  // ENC_CONST
#define R6C 0x000101D5  // CHOPCONF




// API Functions
// All functions act on one IC identified by the TMC5130TypeDef pointer

/*
 * PUBLIC FUNCTIONS
 */
void 		stpr_initStepper	(TMC5130TypeDef *tmc5130, SPI_HandleTypeDef *spi, uint8_t dir, uint8_t current);
uint8_t	 	stpr_home			(TMC5130TypeDef *tmc5130, uint16_t velocity, uint8_t stallguard);   // home stepper motor - stallguard = sgt value
void 		stpr_disableDriver	(TMC5130TypeDef *tmc5130);
void 		stpr_enableDriver	(TMC5130TypeDef *tmc5130);
void 		stpr_writeInt		(TMC5130TypeDef *tmc5130, uint8_t address, int32_t value);
int32_t 	stpr_readInt		(TMC5130TypeDef *tmc5130, uint8_t address);
void 		stpr_right			(TMC5130TypeDef *tmc5130, uint32_t velocity);
void 		stpr_left			(TMC5130TypeDef *tmc5130, uint32_t velocity);
void 		stpr_stop			(TMC5130TypeDef *tmc5130);
void 		stpr_moveTo			(TMC5130TypeDef *tmc5130, int32_t position, uint32_t velocityMax);
void 		stpr_moveBy			(TMC5130TypeDef *tmc5130, int32_t *ticks, uint32_t velocityMax);
void 		stpr_moveAngle		(TMC5130TypeDef *tmc5130, float angle, uint32_t velocityMax);
void 		stpr_setPos			(TMC5130TypeDef *tmc5130, int32_t position);
uint32_t 	stpr_getPos			(TMC5130TypeDef *tmc5130);
void 		stpr_waitMove		(TMC5130TypeDef *tmc5130);

/*
 * PRIVATE FUNCTIONS
 */
void 		tmc5130_writeDatagram	(TMC5130TypeDef *tmc5130, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
void 		tmc5130_rotate			(TMC5130TypeDef *tmc5130, int32_t velocity);


#endif /* TMC_IC_TMC5130_H_ */
