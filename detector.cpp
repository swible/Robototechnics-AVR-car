#include <avr/interrupt.h>
#include <avr/iom16.h>
#include <avr/sfr_defs.h>
#include <stdint.h>

//TODO: Снизить скорость, ЗАСТАВИТЬ ВСЕ ЭТО РАБОТАТЬ!

#define DETECTOR_DIR DDRB

#define DETECTOR_PORT PORTB

#define DETECTOR_PIN PINB

#define COM_DETECTOR_INPUT PB2 //ЧТЕНИЕ

//ЗАПИСЬ
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

#define desired_speed 0.5
#define k 0.013
#define dt 122
#define tubrik

volatile uint8_t cmd = 0;
volatile bool left_ob = 1;
volatile bool right_ob = 1;
volatile uint8_t detector = 0;
volatile uint8_t cnt0 = 0;
volatile uint8_t cnt1 = 0;
volatile uint8_t cnt2 = 0;
volatile bool left_engine_run = false;
volatile bool right_engine_run = false;

volatile uint8_t speed_cnt_left = 0;
volatile uint8_t speed_cnt_right = 0;
volatile uint8_t timer_cnt = 0;

volatile uint8_t speed_left = 0;
volatile uint8_t speed_right = 0;

volatile int pwm_left = 0;
volatile int pwm_right = 0;

volatile bool remove_counter = 0;

volatile double desired_speed_left = desired_speed;
volatile double desired_speed_right = desired_speed;

void
configure_pins()
{
	DETECTOR_DIR |= (_BV(COM_DETECTOR_OUTPUT_LEFT) | _BV(COM_DETECTOR_OUTPUT_RIGHT)); //0 - НАСТРОЙКА ПИНА НА ЗАПИСЬ
	DETECTOR_DIR &= ~_BV(COM_DETECTOR_INPUT); //1 - НАСТРОЙКА ПИНА НА ЧТЕНИЕ

	DETECTOR_PORT &= ~(_BV(COM_DETECTOR_OUTPUT_LEFT) | _BV(COM_DETECTOR_OUTPUT_RIGHT)); //ЗАПИСЫВАЕМ В ПОРТ НУЛИ
	//DETECTOR_PIN &= ~(_BV(COM_DETECTOR_INPUT)); //ЗАПИЫВАЕМ В ПИН НОЛЬ
	ENGINE_DIR |= _BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT);
	ENGINE_PORT &= ~(_BV(ENGINE_LEFT) | _BV(ENGINE_RIGHT));
}

void
configure_timer()
{
	TCCR0 = _BV(CS02);
	TIMSK |= _BV(TOIE0);
}

bool
counter(uint8_t target, uint8_t max = 100) //ФУНКЦИЯ ОБНУЛЕНИЯ СЧЁТЧИКА
{
	/*cnt0++;
	if (cnt0 == 10) cnt0 = 0;
	return cnt0;*/

	if (cnt0 > max) cnt0 = 0;
	return cnt0++ > target;
}

bool
delay_counter(uint8_t target) //ФУНКЦИЯ ОБНУЛЕНИЯ СЧЁТЧИКА
{
	/*cnt2++;
	if (cnt2 == 92) cnt2 = 0;
	return cnt2;*/

	/*if (cnt2 > max) cnt2 = 0;
	return cnt2++ > target;*/

		if (cnt0 > target){
			cnt0 = 0;
			return true;
		}

		cnt0++;

		return false;
}

bool
pwm_counter_left(uint8_t target, uint8_t max = 30)
{
	if (cnt1 > max) cnt1 = 0;
	return cnt1++ > target;
}

bool
pwm_counter_right(uint8_t target, uint8_t max = 10)
{
	if (cnt2 > max) cnt2 = 0;
	return cnt2++ > target;
}

ISR(INT0_vect)
{
	speed_cnt_left++;
}

ISR(INT1_vect)
{
	speed_cnt_right++;
}

ISR(TIMER0_OVF_vect)
{
	/*speed_left = speed_cnt_left * dt;
	speed_right = speed_cnt_right * dt;

	speed_cnt_left = 0;
	speed_cnt_right = 0;

	if (desired_speed_left - (speed_left*k) < 0) pwm_left--;
	if (desired_speed_left - (speed_left*k) > 0) pwm_left++;
	if(desired_speed_left == 0) pwm_left = 0;

	if (desired_speed_right - (speed_right*k) < 0) pwm_right--;
	if (desired_speed_right - (speed_right*k) > 0) pwm_right++;
	if(desired_speed_right == 0) pwm_right = 0;

	pwm_right == desired_speed_left - k * speed_right;*/

	uint8_t c;

	switch(cmd) {
	case 0: //ВКЛЮЧЕНИЕ ЛЕВОГО ДАТЧИКА

		DETECTOR_PORT |= _BV(COM_DETECTOR_OUTPUT_LEFT);

		pwm_left = 10;
		pwm_right = 10;

		right_ob = false;
		left_ob = false;

		cmd = 1;

		break;

	case 1: //НИЧЕГО НЕ ДЕЛАТЬ

		if (delay_counter(8)) cmd = 2;

		break;

	case 2: // СЧИТЫВАЕНИЕ ИЗ ЛЕВОГО ДАТЧИКA

		c = 0;
		for (int i = 0; i < 10; ++i) {
			if (DETECTOR_PIN & _BV(COM_DETECTOR_INPUT)) c++;
		}

		if(c > 7) right_ob = true;

		cmd = 3;

		break;

	case 3: //ВЫКЛЮЧЕНИЕ ЛЕВОГО ДАТЧИКА
		DETECTOR_PORT &= ~(_BV(COM_DETECTOR_OUTPUT_LEFT));
		cmd = 4;
		break;

	case 4:
		DETECTOR_PORT |= _BV(COM_DETECTOR_OUTPUT_RIGHT);
		cmd = 5;
		break;

	case 5:
		if(delay_counter(8)) cmd = 6;
		break;

	case 6:
		c = 0;
		for (int i = 0; i < 10; ++i) {
			if (DETECTOR_PIN & _BV(COM_DETECTOR_INPUT)) c++;
		}

		if(c > 7) left_ob = true;

		cmd = 7;

		break;

	case 7:
		DETECTOR_PORT &= ~(_BV(COM_DETECTOR_OUTPUT_RIGHT));
		cmd = 8;
		break;

	case 8:
		if(!right_ob && left_ob) {
			pwm_left = 10;
			pwm_right = 0;

			/*desired_speed_left = 1;
			desired_speed_right = 0;*/
		}

		if(right_ob && !left_ob) {
			pwm_left = 0;
			pwm_right = 10;

			/*desired_speed_left = 0;
			desired_speed_right = 1;*/
		}

		if(!right_ob && !left_ob) {
			pwm_left = 0;
			pwm_right = 0;

			/*desired_speed_left = 0;
			desired_speed_right = 0;*/
		}

		if(right_ob && left_ob) {
			pwm_left = 5;
			pwm_right = 5;

			/*desired_speed_left = 0.5;
			desired_speed_right = 0.5;*/
		}

		cmd = 9;

		break;

	case 9:

		if (!pwm_counter_left(pwm_left)) ENGINE_PORT |= _BV(ENGINE_LEFT);
		else ENGINE_PORT &= ~_BV(ENGINE_LEFT);

		if (!pwm_counter_right(pwm_right)) ENGINE_PORT |= _BV(ENGINE_RIGHT);
		else ENGINE_PORT &= ~_BV(ENGINE_RIGHT);

		if(delay_counter(100) && (!right_ob || !left_ob)) cmd = 0;
		else cmd = 0;

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
		detector = DETECTOR_PIN;
		detector &= 0b00000100;
	}
	return 0;
}
