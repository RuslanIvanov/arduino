#ifndef iarduino_RF433_h
#define iarduino_RF433_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class iarduino_RF433{
	public:
	uint16_t i433_func_CRC16(const uint8_t*, uint8_t, uint8_t k=0); // Создаём циклически избыточный код	(указатель на массив, количество байт, [первый байт])
};

#endif