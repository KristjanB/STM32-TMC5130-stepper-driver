/*
 * TMC5130.c
 *
 *  Created on: 03.07.2017
 *      Author: LK
 */
#include "TMC5130/TMC5130.h"
#include "assert.h"

// => SPI wrapper.
extern uint32_t tmc5130_readArray(TMC5130TypeDef *tmc5130, uint8_t *data, size_t length);
extern void 	tmc5130_writeArray(TMC5130TypeDef *tmc5130, uint8_t *data, size_t length);
// <= SPI wrapper

// Writes (x1 << 24) | (x2 << 16) | (x3 << 8) | x4 to the given address
void tmc5130_writeDatagram(TMC5130TypeDef *tmc5130, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
{
	uint8_t data[5] = { address | TMC5130_WRITE_BIT, x1, x2, x3, x4 };
	tmc5130_writeArray(tmc5130, &data[0], 5);

}

// Write an integer to the given address
void stpr_writeInt(TMC5130TypeDef *tmc5130, uint8_t address, int32_t value)
{
	tmc5130_writeDatagram(tmc5130, address, BYTE(value, 3), BYTE(value, 2), BYTE(value, 1), BYTE(value, 0));
}

// Read an integer from the given address
int32_t stpr_readInt(TMC5130TypeDef *tmc5130, uint8_t address)
{
	/*address = TMC_ADDRESS(address);

	// register not readable -> shadow register copy
	if(!TMC_IS_READABLE(tmc5130->registerAccess[address]))
		return tmc5130->config->shadowRegister[address];*/

	uint8_t data[5] = { 0, 0, 0, 0, 0 };
	uint32_t dummy = 0;

	data[0] = address;
	dummy = tmc5130_readArray(tmc5130, data, 5);

	data[0] = address;
	dummy = tmc5130_readArray(tmc5130, data, 5);

	return dummy;
}


// Rotate with a given velocity (to the right)
void tmc5130_rotate(TMC5130TypeDef *tmc5130, int32_t velocity)
{
	// Set absolute velocity
	stpr_writeInt(tmc5130, TMC5130_VMAX, abs(velocity));
	// Set direction
	stpr_writeInt(tmc5130, TMC5130_RAMPMODE, (velocity >= 0) ? TMC5130_MODE_VELPOS : TMC5130_MODE_VELNEG);
}

// Rotate to the right
void stpr_right(TMC5130TypeDef *tmc5130, uint32_t velocity)
{
	tmc5130_rotate(tmc5130, velocity);
}

// Rotate to the left
void stpr_left(TMC5130TypeDef *tmc5130, uint32_t velocity)
{
	tmc5130_rotate(tmc5130, -velocity);
}

// Stop moving
void stpr_stop(TMC5130TypeDef *tmc5130)
{
	tmc5130_rotate(tmc5130, 0);
}

// Move to a specified position with a given velocity
void stpr_moveTo(TMC5130TypeDef *tmc5130, int32_t position, uint32_t velocityMax)
{
	stpr_writeInt(tmc5130, TMC5130_RAMPMODE, TMC5130_MODE_POSITION);

	// VMAX also holds the target velocity in velocity mode.
	// Re-write the position mode maximum velocity here.
	stpr_writeInt(tmc5130, TMC5130_VMAX, velocityMax);

	stpr_writeInt(tmc5130, TMC5130_XTARGET, position);
}

// Move by a given amount with a given velocity
// This function will write the absolute target position to *ticks
void stpr_moveBy(TMC5130TypeDef *tmc5130, int32_t *ticks, uint32_t velocityMax)
{
	// determine actual position and add numbers of ticks to move
	*ticks += stpr_readInt(tmc5130, TMC5130_XACTUAL);

	stpr_moveTo(tmc5130, *ticks, velocityMax);
}

// Translate angle to steps with a resolution of 0.01 degrees
void stpr_moveAngle(TMC5130TypeDef *tmc5130, float angle, uint32_t velocityMax)
{

	int32_t position;


	position = (int32_t)((angle * 51200) / 360);

	//stpr_writeInt(tmc5130, TMC5130_RAMPMODE, (angle >= 0) ? TMC5130_MODE_VELPOS : TMC5130_MODE_VELNEG)

	stpr_writeInt(tmc5130, TMC5130_RAMPMODE, TMC5130_MODE_POSITION);
	// VMAX also holds the target velocity in velocity mode.
	// Re-write the position mode maximum velocity here.
	stpr_writeInt(tmc5130, TMC5130_VMAX, velocityMax);

	stpr_writeInt(tmc5130, TMC5130_XTARGET, position);
}



// Get current stepper position
uint32_t stpr_getPos(TMC5130TypeDef *tmc5130)
{
	int32_t XActual;
	XActual = stpr_readInt(tmc5130, TMC5130_XACTUAL);

	return XActual;
}

void stpr_disableDriver(TMC5130TypeDef *tmc5130)
{
	HAL_GPIO_WritePin(tmc5130->en_port, tmc5130->en_pin, GPIO_PIN_SET);
}

void stpr_enableDriver(TMC5130TypeDef *tmc5130)
{
	HAL_GPIO_WritePin(tmc5130->en_port, tmc5130->en_pin, GPIO_PIN_RESET);
}

void stpr_setPos(TMC5130TypeDef *tmc5130, int32_t position)
{
	stpr_writeInt(tmc5130, TMC5130_XTARGET,	position);
	stpr_writeInt(tmc5130, TMC5130_XACTUAL,	position);
}

void stpr_waitMove(TMC5130TypeDef *tmc5130)
{
	while((stpr_readInt(tmc5130, TMC5130_RAMPSTAT) & 0x400) != 0x400);
}

void stpr_initStepper(TMC5130TypeDef *tmc5130, SPI_HandleTypeDef *spi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t dir, uint8_t current)
{
	 tmc5130->spi = spi;
	 tmc5130->cs_pin = cs_pin;
	 tmc5130->cs_port = cs_port;
	 tmc5130->homing_done = 0;
	 tmc5130->home_state = HOME;
	 tmc5130->prev_home_state = HOME;

	 stpr_writeInt(tmc5130, TMC5130_GCONF, (dir << 4)); 	// GCONF

	 stpr_writeInt(tmc5130, TMC5130_TPOWERDOWN,	0x0000000A); 	//TPOWERDOWN=10

	 stpr_writeInt(tmc5130, TMC5130_IHOLD_IRUN,	(current << 8) | 0x0007006); 	//IHOLD_IRUN: IHOLD=6, IRUN=19 (max.current), IHOLDDELAY=6

	 // Chopconf settings
	 stpr_writeInt(tmc5130, TMC5130_CHOPCONF,	2  << 15 | 		// TBL
			 	 	 	 	 	 	 	 	 	3  << 0  |		// TOFF
												4  << 7  |   	// HEND
												0  << 4	 );  	// HSTART

	 stpr_writeInt(tmc5130, TMC5130_AMAX, 		1000);						// AMAX for stallGuard homing shall be significantly lower than AMAX for printing
	 stpr_writeInt(tmc5130, TMC5130_A1,		 	0x0);			// 0x000003E8);    //A1=1000
	 stpr_writeInt(tmc5130, TMC5130_V1,		 	0x0);			// 0x000186A0);    //V1=100000
	 stpr_writeInt(tmc5130, TMC5130_D1,			0x10);			// must be != 0 !!!!!!!
	 stpr_writeInt(tmc5130, TMC5130_VSTOP,		0x00000000A);   // VSTOP=20
	 stpr_writeInt(tmc5130, TMC5130_VSTART,		0x000000005);   // VSTART=5
	 stpr_writeInt(tmc5130, TMC5130_PWMCONF,	0x00000000); 	// PWMCONF
	 stpr_writeInt(tmc5130, TMC5130_RAMPMODE,	TMC5130_MODE_POSITION);     //RAMPMODE=0
	 stpr_writeInt(tmc5130, TMC5130_XACTUAL,	0x00000000);     //XACTUAL=0
	 stpr_writeInt(tmc5130, TMC5130_XTARGET,	0x00000000);     //XTARGET=0
}



/*
 * Optimal stallguard value is influenced by velocity and stepper current.
 */
uint8_t stpr_home(TMC5130TypeDef *tmc5130, uint16_t velocity, uint8_t stallguard)
{

	/*
	 * rotate home indefinitely until stallguard triggers
	 */
	switch(tmc5130->home_state)
	{

		case HOME:
		{
			//stpr_enableDriver(tmc5130);

			//stpr_writeInt(tmc5130, TMC5130_SWMODE, 0x00);

			stpr_writeInt(tmc5130, TMC5130_COOLCONF, 	((stallguard& 0x7F)<<16));	// sgt <-- Entry the value determined for SGT: lower value=higher sensitivity (lower force for stall detection)
			stpr_writeInt(tmc5130, TMC5130_TCOOLTHRS,	0xFFFF);				// TCOOLTHRS
			stpr_writeInt(tmc5130, TMC5130_SWMODE, 		0x400);						// enable SG
			stpr_writeInt(tmc5130, TMC5130_AMAX, 		1000);						// AMAX for stallGuard homing shall be significantly lower than AMAX for printing

			stpr_readInt(tmc5130, TMC5130_RAMPSTAT);

			stpr_right(tmc5130, velocity);

			HAL_Delay(50);

			tmc5130->prev_home_state = HOME;
			tmc5130->home_state = WAIT_MOVE;

			break;
		}

		case RETRACT:
		{
			// Endstop reached. Reset and retract
			//stpr_writeInt(tmc5130, TMC5130_RAMPMODE, TMC5130_MODE_HOLD);		//HOLD Mode
			stpr_writeInt(tmc5130, TMC5130_RAMPMODE, TMC5130_MODE_POSITION);	//Position MODE
			stpr_writeInt(tmc5130, TMC5130_XACTUAL, 	0x0);					//XACTUAL = 0
			stpr_writeInt(tmc5130, TMC5130_SWMODE, 	0x0);					//SWITCH REGISTER
			stpr_writeInt(tmc5130, TMC5130_VMAX, 	velocity);				//Homing Speed in VMAX
			stpr_writeInt(tmc5130, TMC5130_DMAX, 	0xFFFF);				//DMAX
			stpr_writeInt(tmc5130, TMC5130_XTARGET, 	0);					//XTARGET = homing_retract

			tmc5130->prev_home_state = RETRACT;
			tmc5130->home_state = WAIT_MOVE;

			break;

		}
		case UNRETRACT:
		{
			// Endstop reached. Reset and retract
			//stpr_writeInt(tmc5130, TMC5130_RAMPMODE, TMC5130_MODE_HOLD);		//HOLD Mode
			stpr_writeInt(tmc5130, TMC5130_RAMPMODE, TMC5130_MODE_POSITION);	//Position MODE
			stpr_writeInt(tmc5130, TMC5130_SWMODE, 	0x0);					//SWITCH REGISTER
			stpr_writeInt(tmc5130, TMC5130_VMAX, 	velocity);				//Homing Speed in VMAX
			stpr_writeInt(tmc5130, TMC5130_DMAX, 	0xFFFF);				//DMAX
			stpr_writeInt(tmc5130, TMC5130_XTARGET, 	0);					//XTARGET = homing_retract


			tmc5130->prev_home_state = UNRETRACT;
			tmc5130->home_state = WAIT_MOVE;

			break;
		}

		case WAIT_MOVE:
		{

			tmc5130->drvstat = (stpr_readInt(tmc5130, TMC5130_DRVSTATUS) & 0x11F03FF);
			tmc5130->sg_flag = (tmc5130->drvstat & 0x1000000) >> 24;
			tmc5130->sg_result = tmc5130->drvstat & 0x3FF;
			tmc5130->cs_actual = tmc5130->drvstat & 0x1F0000;

			if((stpr_readInt(tmc5130, TMC5130_RAMPSTAT) & 0x400) == 0x400)
			//if((stpr_readInt(tmc5130, TMC5130_DRVSTATUS) & 0x1000000) >> 24)
			{
				stpr_stop(tmc5130);
				stpr_writeInt(tmc5130, TMC5130_XTARGET, 	0);
				if(tmc5130->prev_home_state == HOME)
				{
					tmc5130->home_state = RETRACT;
				}
				else if(tmc5130->prev_home_state == RETRACT)
				{
					tmc5130->home_state = UNRETRACT;
				}
				else if(tmc5130->prev_home_state == UNRETRACT)
				{
					stpr_writeInt(tmc5130, TMC5130_XACTUAL, 	0x0);					//XACTUAL = 0
					tmc5130->homing_done = 1;
				}
			}
			break;
		}

	}

	return tmc5130->homing_done;
}


