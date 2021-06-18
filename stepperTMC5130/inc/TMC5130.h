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
#include "TMC5130_Pins.h"


#define BYTE(value, n)    (((value) >> ((n) << 3)) & 0xFF)

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

	// INITIALIZATION
	GPIO_TypeDef	*cs_port;
	uint16_t		cs_pin;

	GPIO_TypeDef	*en_port;
	uint16_t		en_pin;

} TMC5130TypeDef;




// API Functions
// All functions act on one IC identified by the TMC5130TypeDef pointer

/*
 * PUBLIC FUNCTIONS
 */
void 		stpr_initStepper	(TMC5130TypeDef *tmc5130, SPI_HandleTypeDef *spi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t dir, uint8_t current);
//uint8_t	 	stpr_home			(TMC5130TypeDef *tmc5130, uint16_t velocity, uint8_t stallguard);   // home stepper motor - stallguard = sgt value
void 		stpr_home			(TMC5130TypeDef *tmc5130, uint16_t homing_speed, uint8_t stallguardthreshold);
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
int32_t 	stpr_getPos			(TMC5130TypeDef *tmc5130);
void 		stpr_waitMove		(TMC5130TypeDef *tmc5130);
void 		stpr_setCurrent		(TMC5130TypeDef *tmc5130, uint8_t current);

void homing_test(TMC5130TypeDef *tmc5130, uint16_t homing_speed, uint8_t stallguardthreshold);

/*
 * PRIVATE FUNCTIONS
 */
void 		tmc5130_writeDatagram	(TMC5130TypeDef *tmc5130, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
void 		tmc5130_rotate			(TMC5130TypeDef *tmc5130, int32_t velocity);


#endif /* TMC_IC_TMC5130_H_ */
