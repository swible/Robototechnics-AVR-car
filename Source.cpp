#include <avr/io.h>
#include <avr/common.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define COM1			PD7
#define	COM2			PD5
#define COM3			PD4
#define COM4			PB3

#define COM1_DIR		DDRD
#define COM2_DIR		DDRD
#define COM3_DIR		DDRD
#define COM4_DIR		DDRB

#define COM1_PORT		PORTD
#define COM2_PORT		PORTD
#define COM3_PORT		PORTD
#define COM4_PORT		PORTB

#define COM1_PIN		PIND
#define COM2_PIN		PIND
#define COM3_PIN		PIND
#define COM4_PIN		PINB

volatile uint8_t x = 0;

void
configure_pins()
{
	COM1_DIR |= _BV(COM1);

	DDRC &= ~(_BV(PC3));
	PORTC |= _BV(PC3);
}

void
configure_timer()
{
	TCCR0 = _BV(CS02);
	TIMSK |= _BV(TOIE0);
}

ISR(TIMER0_OVF_vect)
{
	switch (x)
	{
		case 0:
		if (PINC & _BV(PC3))
		{
			COM1_PORT ^= _BV(COM1);
			x = 1;
		}
		break;
		case 1:
		if (!(PINC & _BV(PC3)))
		{
			x = 0;
		}
		break;
	}
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
	}
	return 0;
}
