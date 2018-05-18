#include <avr/io.h>
#include <avr/common.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define ENGINE_LEFT PD7
#define ENGINE_RIGHT PD5
/*
#define COM3 PD4
#define COM4 PB3
*/

#define ENGINE_DIR DDRD
/*
#define COM2_DIR DDRD
#define COM3_DIR DDRD
#define COM4_DIR DDRB
*/

#define ENGINE_PORT PORTD
/*
#define COM2_PORT PORTD
#define COM3_PORT PORTD
#define COM4_PORT PORTB
*/

//#define LAUNCH ENGINE_PORT &= _BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT)
#define LEFT ENGINE_PORT |= _BV(ENGINE_LEFT)
#define RIGHT ENGINE_PORT |= _BV(ENGINE_RIGHT)

#define STOP_LEFT ENGINE_PORT &= ~_BV(ENGINE_LEFT)
#define STOP_RIGHT ENGINE_PORT &= ~_BV(ENGINE_RIGHT)

/*#define ENGINE_PIN PIND
#define COM2_PIN PIND
#define COM3_PIN PIND
#define COM4_PIN PINB
#define COM5_PIN PINA */
#define INPUT_PIN PINA

//volatile uint8_t x = 0;
volatile uint8_t pin_copy = 0;

volatile uint8_t engine_switch = 0;
volatile uint8_t commands_switch = 0;

//volatile bool check=false;
volatile uint8_t counter = 0;
volatile uint8_t pwm_left = 0;
volatile uint8_t pwm_right = 0;

void
configure_pins()
{
	ENGINE_DIR |= _BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT);
	ENGINE_PORT &= ~(_BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT));

	DDRA = 0x00;
	INPUT_PIN = 0xFF;
}



void
configure_timer()
{
	TCCR0 = _BV(CS02);
	TIMSK |= _BV(TOIE0);
}



void
Engine_work() //Можно записать сюда программу, которая будет выполнятся без использования переключателей
{
	switch(engine_switch)
	{
		case(0):

			engine_switch = 1;
			break;

		case(1):

			engine_switch = 2;
			break;

		case(2):

			engine_switch = 3;
			break;

		case(3):

			engine_switch = 0;
			break;
	}
}



void
Commands_work() // комманды, отправленные парами переключателей
{
	switch (commands_switch)
	{
		case 0b00000000: // 00 - стоп
			pwm_left = 1;
			pwm_right = 1;
			break;

		case 0b00000010: // 10 - влево
			pwm_left = 10;
			pwm_right = 5;
			break;

		case 0b00000001: // 01 - вправо
			pwm_left = 5;
			pwm_right = 10;
			break;

		case 0b00000011: // 11 - вперед
			pwm_left = 10;
			pwm_right = 10;
			break;
	}
}



void
Pwm_work() //Скорость вращения колес
{
	if(counter++ == 10) counter = 0;
	if(counter < pwm_left) LEFT; else STOP_LEFT;
	if(counter < pwm_right) RIGHT; else STOP_RIGHT;
}



ISR(TIMER0_OVF_vect)
{
	Engine_work();
	Commands_work();
	Pwm_work();
}



int
main(void)
{
	cli();

	configure_pins();
	configure_timer();

	sei();

	while (1)
	{
		pin_copy = INPUT_PIN;
		for (int i = 0; i < 4; i++)
		{
			commands_switch = pin_copy;
			commands_switch &= 0b00000011;
			//check=false;
			pin_copy = pin_copy >> 2;
			_delay_ms(2000);
		}
	}
	return 0;
}
