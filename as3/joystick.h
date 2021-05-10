#ifndef JOYSTICK_CONTROL
#define JOYSTICK_CONTROL


enum eJoystickDirections{
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_CENTER,
    JOYSTICK_NUMBER_OF_DIRECTIONS,
};

void Joystick_init(void);
void Joystick_cleanup(void);


#endif