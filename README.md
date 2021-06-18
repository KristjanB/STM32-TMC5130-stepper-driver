# STM32 TMC5130 stepper driver
 Stepper driver for Trinamic TMC5130 BOB. Still work in progress!
 Currently tested on STM32F4 boards.
 
 Supports multiple steppers and stallguard functionality using integrated driver (the TMC IC provides DIR/STEP pulses by itself)
 
 How to use:
 
 1) Call stpr_enableDriver(TMC5130TypeDef *tmc5130) to enable it.
 
 2) Call stpr_initStepper(TMC5130TypeDef *tmc5130, SPI_HandleTypeDef *spi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t dir, uint8_t current),
    where argument:
    1) tmc5130TypeDef stepper1;
    2) SPI_HandleTypeDef hspiX (X = spi channel; normally added automatically if using CUBEMX)
    3) GPIO port that is defined in TMC5130_Pins.h
    4) GPIO pin that is defined in TMC5130_Pins.h
    5) direction (1 or 0)
    6) current (20 corresponds to 200mA and so on)
    
 3) Move the stepper by calling any of the functions that have movement verb in it:
 
    Keep in mind that stpr_setPos() will cause the motor to move if the position you pass it is different from the position it's currently in.

```
 stpr_moveTo   (TMC5130TypeDef *tmc5130, int32_t position, uint32_t velocityMax);
 stpr_moveBy   (TMC5130TypeDef *tmc5130, int32_t *ticks, uint32_t velocityMax);
 stpr_moveAngle(TMC5130TypeDef *tmc5130, float angle, uint32_t velocityMax);
 stpr_right    (TMC5130TypeDef *tmc5130, uint32_t velocity);
 stpr_left     (TMC5130TypeDef *tmc5130, uint32_t velocity);
 stpr_setPos   (TMC5130TypeDef *tmc5130, int32_t position);
```

4) Wait for the motor to reach the desired position with stpr_waitMove(). This is using pooling method so the code will stop on this line untill the motor stops.
