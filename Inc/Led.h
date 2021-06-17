
#ifndef LED_H_
#define LED_H_

void OnBoard_Leds_Initialize (void);

#define LED1	0UL
#define LED2	1UL

#define DMASK 		3UL
#define VERY_HIGH_SPEED	3UL
#define QMASK 		15UL
#define MASK 		1UL
#define OUTPUT_MODE 1UL
#define ALTF_MODE   2UL
#define PUSH_PULL	0UL
#define PULL_UP  	1UL
#define HIGH		1UL

#endif /* LED_H_ */
