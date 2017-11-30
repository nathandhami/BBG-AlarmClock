/*
 * led.h
 * Used for blinking led during alarm
 */

#ifndef _LED_H_
#define _LED_H_


void LED_init(void);
void LED_activate(void);
void LED_deactivate(void);

int LED_getStatus(void);

#endif /* _LED_H_ */
