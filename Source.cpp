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

#define LAUNCH ENGINE_PORT &= _BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT)
#define LEFT ENGINE_PORT |= _BV(ENGINE_LEFT)
#define RIGHT ENGINE_PORT |= _BV(ENGINE_RIGHT)
#define STOP_LEFT ENGINE_PORT &= ~(_BV(ENGINE_LEFT))
#define STOP_RIGHT ENGINE_PORT &= ~(_BV(ENGINE_RIGHT))

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
volatile uint8_t counter = 0; //счётчик ШИМ
volatile uint8_t PWM_LEFT = 0;
volatile uint8_t PWM_RIGHT = 0;

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
//changes
void commands_work()
{
	switch (cmd) {
	case (01)
			RIGHT_PWM = 5;
			break;
	case (02)
			RIGHT_PWM = 0;
			break;
	case (03)
			right_pwm = 10;
			break;
	}
}

/*void engine_work()
{
	switch(step){
	case(01)
			step = 02
			break;
	case(02)
			step = 03
			break;
	case(03)
			step = 04
			break;
	case(04)
			step = 01
			break;
	}
}*/

void pwm_work()
{
	if (cnt++ > 10) cnt = 0;
	if (0 < cnt < pwm_right) COM1 = 1 else COM1 = 0;
	if (0 < cnt < pwm_left ) COM2 = 1 else COM2 = 0;
}

ISR(TIMER_OVF_vect)
{
	engine_work();
	commands_work();
	pwm_work();
}

ISR(TIMER0_OVF_vect)
{
	if (counter++ == 0) LAUNCH;
	if (counter++ == 10)
	{
		counter = 0;
	}
	switch (z)
	{
		case 0b00000000: // 00 - стоп

			if(counter == pwm_right) STOP_RIGHT;
			if(counter == 10) STOP_LEFT;
		break;
		case 0b0000010: // 10 - влево
			if(counter == 5) STOP_RIGHT;
			if(counter == 10) STOP_LEFT; //Едем или стоим
			//if (counter < 122) STOP_LEFT;
		break;
		case 0b00000001: // 01 - вправо
			if(counter == 10) STOP_RIGHT;
			if(counter == 5) STOP_LEFT; //Едем или стоим
			//if (counter < 122) STOP_RIGHT;
		break;
		case 0b00000011: // 11 - вперед
			if(counter == 10) STOP_RIGHT;
			if(counter == 10) STOP_LEFT;
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
		y = 0b10101010;
		for (int i = 0; i < 4; i++)
		{
			z = y;
			z &= 0b00000011;
			check=false;
			y = y >> 2;
			_delay_ms(2000);
		}
	}
	return 0;
}
