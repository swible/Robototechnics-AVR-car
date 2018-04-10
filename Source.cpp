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

#define STOP ENGINE_PORT &= ~(_BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT))
#define LEFT ENGINE_PORT |= _BV(ENGINE_LEFT)
#define RIGHT ENGINE_PORT |= _BV(ENGINE_RIGHT)

/*#define ENGINE_PIN PIND 
#define COM2_PIN PIND 
#define COM3_PIN PIND 
#define COM4_PIN PINB 
#define COM5_PIN PINA */ 
#define INPUT_PIN PINA

volatile uint8_t x = 0; 
volatile uint8_t y = 0; 
volatile uint8_t z = 0;
volatile bool check=false;

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

ISR(TIMER0_OVF_vect)
{
	if(!check)
	{
		check=true;
		switch (z)
		{
			case 0b00000000: // 00 - стоп
				STOP;
			break;
			case 0b0000001: // 01 - вправо
				STOP; LEFT;
			break;
			case 0b00000010: // 10 - влево
				STOP; RIGHT;
			break;
			case 0b00000011: // 11 - вперед
				LEFT; RIGHT;
			break;
		}
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
		y = INPUT_PIN;
		for (int i = 0; i < 4; i++)
		{
			z = y;
			z &= 0b00000011;
			check=false;
			y = y >> 2;
			_delay_ms(1000);
		}
	}
	return 0;
}
