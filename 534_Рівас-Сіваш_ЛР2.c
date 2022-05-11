/*
    Застосування МП у БМЗ
    Студент: Рівас Сіваш Максим
    Група: 534

    Завдання - Варіянт 2:
    «Система керування двигуном»
    Написати програму для системи керування двигуном, яка має кнопки 0-2,
    під’єднані до виводів порту B0–B2. Натиснення кнопки 0 умикає двигун,
    повторне натиснення – вимикає. Двигун має 4 швидкості обертання. Натиснення
    кнопки 1 підвищує швидкість обертання двигуна вдвічі, кнопки 2 – зменшує
    вдвічі. Передбачити обмеження швидкості обертання двигуна.
    Кроковий двигун з 4 обмотками, під’єднано до виводів порту B4-B7.
    Допустимі часові затримки при керуванні двигуном – від 1 до 100 мс.
*/

// Частота
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

// Умикач двигуна
#define ENGINE_BUTTON 0b1

// Кнопка збільшення швидкости
#define SPEED_UP_BUTTON 0b10

// Кнопка зниження швидкости
#define SPEED_DOWN_BUTTON 0b100

// Визначають швидкість обертання
#define MIN_DELAY 10
#define MAX_DELAY 80

// Кількість обмоток
#define COILS_NUMBER 4

// Переходить до наступної обмотки
uint8_t nextCoil(uint8_t i);

// Швидкості двигуна
enum engineSpeed
{
    One = 1,
    Two,
    Three,
    Four
};

// Збільшує швидкість враховуючи обмеження
enum engineSpeed increaseSpeed(enum engineSpeed speed);

// Зменшує швидкість враховуючи обмеження
enum engineSpeed decreaseSpeed(enum engineSpeed speed);

// Робить затримку в залежності від обраної швидкости
void wait(enum engineSpeed speed);

void main(void)
{

#pragma optsize -
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPCE) | (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

#ifdef _OPTIMIZE_SIZE_
#pragma optsize +
#endif

    // Налаштування
    DDRB = 0xF0;
    PORTB = ENGINE_BUTTON | SPEED_DOWN_BUTTON | SPEED_UP_BUTTON;

    uint8_t coil = 0;
    enum engineSpeed speed = One;

    bool engineOn = false;

    bool engineButtonHeld = false;
    bool speedDownButtonHeld = false;
    bool speedUpButtonHeld = false;

    while (true)
    {
        bool engineButtonPressed = PINB & ENGINE_BUTTON == 0;
        bool speedUpButtonPressed = PINB & SPEED_UP_BUTTON == 0;
        bool speedDownButtonPressed = PINB & SPEED_DOWN_BUTTON == 0;

        // Перевірка кнопки двигуна на утримання
        if (engineButtonPressed && !engineButtonHeld)
        {
            engineOn = !engineOn;
            engineButtonHeld = true;
        }
        else
            engineButtonHeld = false;

        // Перевірка кнопки збільшення швидкости на утримання
        if (speedUpButtonPressed && !speedUpButtonHeld)
        {
            speed = increaseSpeed(speed);
            speedUpButtonHeld = true;
        }
        else
            speedUpButtonHeld = false;

        // Перевірка кнопки зменшення швидкости на утримання
        if (speedDownButtonPressed && !speedDownButtonHeld)
        {
            speed = decreaseSpeed(speed);
            speedDownButtonPressed = true;
        }
        else
            speedDownButtonPressed = false;

        if (engineOn)
        {
            PORTB = ((1 << coil) << 4); // | ENGINE_BUTTON | SPEED_DOWN_BUTTON | SPEED_UP_BUTTON;

            wait(speed);

            coil = nextCoil(coil);
        }
    }
}

enum engineSpeed increaseSpeed(enum engineSpeed speed)
{
    if (speed < Four)
        ++speed;

    return speed;
}

enum engineSpeed decreaseSpeed(enum engineSpeed speed)
{
    if (speed > One)
        --speed;

    return speed;
}

uint8_t nextCoil(uint8_t i)
{
    if (i < (COILS_NUMBER - 1))
        ++i;

    return i;
}

void wait(enum engineSpeed speed)
{
    switch (speed)
    {
    case One:
        _delay_ms(10);
        break;
    case Two:
        _delay_ms(20);
        break;
    case Three:
        _delay_ms(40);
        break;
    case Four:
        _delay_ms(80);
        break;
    }
}