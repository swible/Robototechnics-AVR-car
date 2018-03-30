#include <avr/io.h> 
#include <avr/common.h> 
#include <avr/interrupt.h> 
#include <util/delay.h> 

#define ENGINE_LEFT PD7 
#define ENGINE_RIGHT PD5 
#define COM3 PD4 
#define COM4 PB3 

#define ENGINE_DIR DDRD 
#define COM2_DIR DDRD 
#define COM3_DIR DDRD 
#define COM4_DIR DDRB 

#define ENGINE_PORT PORTD 
#define COM2_PORT PORTD 
#define COM3_PORT PORTD 
#define COM4_PORT PORTB 

/*#define ENGINE_PIN PIND 
#define COM2_PIN PIND 
#define COM3_PIN PIND 
#define COM4_PIN PINB 
#define COM5_PIN PINA */ 

volatile uint8_t x = 0; 
volatile uint8_t y = 0; 
volatile uint8_t Z = 0; 

void 
configure_pins() 
{ 
	ENGINE_DIR |= _BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT); 
	ENGINE_PORT &= ~(_BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT)); 

	DDRA = 0x00; 
	PORTA = 0xFF; 
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
