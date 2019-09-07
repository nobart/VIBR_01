/*
ATTINY13A:
3. PB4 - MOTOR_PIN_EN
4. GND
8. VCC
L: 0x69; H: 0xFF
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define WAKE_TIME_VA (int)1 //1*8 seconds
#define MOTOR_ON_TIME_MS (int)250
#define MOTOR_PIN_EN PB4
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)

volatile uint8_t wakeUpCnt = 0;
volatile uint8_t wakeUpFlag;

static void turnMotorOn(int timeValMs)
{
  PORTB |= (1 << MOTOR_PIN_EN);//HI
  _delay_ms(timeValMs);
  PORTB &= ~(1 << MOTOR_PIN_EN);//LO
}

int main(void)
{
  DDRB = 0xFF;
  DDRB |= (1 << MOTOR_PIN_EN);
  PORTB &= ~(1 << MOTOR_PIN_EN); //reset

  WDTCR |= (1 << WDCE) | (1 << WDE); //prescaler value change possibility
  WDTCR = (1 << WDP3) | (1 << WDP0); // watchdog counter 8 seconds
  WDTCR |= (1 << WDTIE); //watchdog int

  sei();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  adc_disable();

  while (1)
    {
      if (wakeUpFlag)
        {
          turnMotorOn(MOTOR_ON_TIME_MS);
          set_sleep_mode(SLEEP_MODE_PWR_DOWN);
          wakeUpFlag = 0;
        }
      sleep_mode();
    }
}

ISR(WDT_vect)
{
  wakeUpCnt++;
  if (wakeUpCnt >= WAKE_TIME_VA)
    {
      wakeUpFlag = 1;
      wakeUpCnt = 0;
    }
  wdt_reset();
}
