#include <avr/io.h>
#include <avr/common.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DETECTOR_DIR DDRB

#define DETECTOR_PORT PORTB

#define DETECTOR_PIN PINB

#define COM_DETECTOR_INPUT PB2 //×ÒÅÍÈÅ

//ÇÀÏÈÑÜ
#define COM_DETECTOR_OUTPUT_LEFT PB4
#define COM_DETECTOR_OUTPUT_RIGHT PB1

#define ENGINE_LEFT PD7
#define ENGINE_RIGHT PD5

#define ENGINE_DIR DDRD

#define ENGINE_PORT PORTD

#define LEFT ENGINE_PORT |= _BV(ENGINE_LEFT)
#define RIGHT ENGINE_PORT |= _BV(ENGINE_RIGHT)

#define STOP_LEFT ENGINE_PORT &= ~_BV(ENGINE_LEFT)
#define STOP_RIGHT ENGINE_PORT &= ~_BV(ENGINE_RIGHT)

#define INPUT_PIN PINA

volatile uint8_t cmd = 0;
volatile bool left_wh = 1;
volatile bool right_wh = 1;
volatile uint8_t detector = 0;
volatile uint8_t cnt0 = 0;
volatile uint8_t cnt1 = 0;
volatile uint8_t cnt2 = 0;
volatile bool left_engine_run = false;
volatile bool right_engine_run = false;

void
configure_pins()
{
	DETECTOR_DIR |= (_BV(COM_DETECTOR_OUTPUT_LEFT) | _BV(COM_DETECTOR_OUTPUT_RIGHT)); //0 - ÍÀÑÒÐÎÉÊÀ ÏÈÍÀ ÍÀ ÇÀÏÈÑÜ
	DETECTOR_DIR &= ~_BV(COM_DETECTOR_INPUT); //1 - ÍÀÑÒÐÎÉÊÀ ÏÈÍÀ ÍÀ ×ÒÅÍÈÅ

	DETECTOR_PORT &= ~(_BV(COM_DETECTOR_OUTPUT_LEFT) | _BV(COM_DETECTOR_OUTPUT_RIGHT)); //ÇÀÏÈÑÛÂÀÅÌ Â ÏÎÐÒ ÍÓËÈ
	//DETECTOR_PIN &= ~(_BV(COM_DETECTOR_INPUT)); //ÇÀÏÈÛÂÀÅÌ Â ÏÈÍ ÍÎËÜ

	ENGINE_DIR |= _BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT);
	ENGINE_PORT &= ~(_BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT));
}

void
configure_timer()
{
	TCCR0 = _BV(CS02);
	TIMSK |= _BV(TOIE0);
}

int
counter() //ÔÓÍÊÖÈß ÎÁÍÓËÅÍÈß Ñ×¨Ò×ÈÊÀ
{
	cnt0++;
	if (cnt0 == 10) cnt0 = 0;
	return cnt0;
}

int
delay_counter() //ÔÓÍÊÖÈß ÎÁÍÓËÅÍÈß Ñ×¨Ò×ÈÊÀ
{
	cnt2++;
	if (cnt2 == 92) cnt2 = 0;
	return cnt2;
}

int
pwm_counter() {
	cnt1++;
	if (cnt1 == 20) cnt1 = 0;
	return cnt1;
}

ISR(TIMER0_OVF_vect)
{
	switch(cmd) {
	case 0: //ÂÊËÞ×ÅÍÈÅ ËÅÂÎÃÎ ÄÀÒ×ÈÊÀ
		DETECTOR_PORT |= _BV(COM_DETECTOR_OUTPUT_LEFT);
		cmd = 1;
		break;
	case 1: //ÍÈ×ÅÃÎ ÍÅ ÄÅËÀÒÜ
		if (counter() > 8) cmd = 2;
		break;
	case 2: // Ñ×ÈÒÛÂÀÅÍÈÅ ÈÇ ËÅÂÎÃÎ ÄÀÒ×ÈÊÀ
		if (DETECTOR_PIN & _BV(COM_DETECTOR_INPUT)) right_wh = true;//ENGINE_PORT |= _BV(ENGINE_RIGHT)
		//if (detector == 0b00000100) right_wh = true;
		if(!right_wh) right_engine_run = false;
		cmd = 3;
		break;
	case 3: //ÂÛÊËÞ×ÅÍÈÅ ËÅÂÎÃÎ ÄÀÒ×ÈÊÀ
		DETECTOR_PORT &= ~(_BV(COM_DETECTOR_OUTPUT_LEFT));
		cmd = 4;
		break;
	case 4:
		DETECTOR_PORT |= _BV(COM_DETECTOR_OUTPUT_RIGHT);
		cmd = 5;
		break;
	case 5:
		if(counter() > 8) cmd = 6;
		break;
	case 6:
		if (DETECTOR_PIN & _BV(COM_DETECTOR_INPUT)) left_wh = true;
		//if (detector == 0b00000100) left_wh = true;
		if(!left_wh) left_engine_run = false;
		cmd = 7;
		break;
	case 7:
		DETECTOR_PORT &= ~(_BV(COM_DETECTOR_OUTPUT_RIGHT));
		cmd = 8;
		break;
	case 8:
		if(left_wh) {
			left_engine_run = true;
			//ENGINE_PORT |= _BV(ENGINE_LEFT);
		}
		if(right_wh) {
			right_engine_run = true;
			//ENGINE_PORT |= _BV(ENGINE_RIGHT);
		}
		if(delay_counter() > 30) {
			cmd = 0;
			right_wh = false;
			left_wh = false;
		}

	}
	if (left_engine_run & (pwm_counter() < 5)) ENGINE_PORT |= _BV(ENGINE_LEFT);
	else ENGINE_PORT &= ~_BV(ENGINE_LEFT);

	if (right_engine_run & (pwm_counter() < 5)) ENGINE_PORT |= _BV(ENGINE_RIGHT);
	else ENGINE_PORT &= ~_BV(ENGINE_RIGHT);
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
		detector = DETECTOR_PIN;
		detector &= 0b00000100;
	}
	return 0;
}
