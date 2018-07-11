#ifndef HEADER_H_

//===============Дальномеры=================
#define DETECTOR_DIR DDRB

#define DETECTOR_PORT PORTB
#define DETECTOR_PIN PINB

//Нога на которую подаётся питание (оба датчика подключены к 1 ноге)
#define COM_DETECTOR_INPUT PB2

//Ноги с которых считываем сигнал
#define COM_DETECTOR_OUTPUT_LEFT PB4
#define COM_DETECTOR_OUTPUT_RIGHT PB1
//==========================================

//===============Двигатели==================
#define ENGINE_DIR DDRD

#define ENGINE_PORT PORTD

#define ENGINE_LEFT PD7
#define ENGINE_RIGHT PD5

//Запуск двигателей
#define PUSK_LEFT ENGINE_PORT |= _BV(ENGINE_LEFT)
#define PUSK_RIGHT ENGINE_PORT |= _BV(ENGINE_RIGHT)
//-----------------

//Остановка двигателей
#define STOP_LEFT ENGINE_PORT &= ~_BV(ENGINE_LEFT)
#define STOP_RIGHT ENGINE_PORT &= ~_BV(ENGINE_RIGHT)
//--------------------
//==========================================

//Макросы, которые используются в ПИД-регуляторе для преобразования входной скорости в значения, которые использует ШИМ
#define _MIN(x,y) ((x < y) ? x : y)
#define _MAX(x,y) ((x > y) ? x : y)
//---------------------------------------------------------------------------------------------------------------------

//Всё что ниже можно удалить, но пусть пока что будет
//#define INPUT_PIN PINA //непонятный пин, нигде не используется
//1 метр в секунду - максимальная скорость машинки
/*
#define desired_speed 0.5
#define desired_speed_pwm desired_speed/(0.026)
#define k 0.013
#define p 61
*/

#endif /* HEADER_H_ */
