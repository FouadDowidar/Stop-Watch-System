/******************************************************************************
 *
 * Module: Stop Watch System
 *
 * File Name: mini.c
 *
 * Description: Source file for Stop Watch System Project
 *
 * Author: Fouad Dowidar
 *
 *******************************************************************************/

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

unsigned char sec=0;/*COUNTER FOR SECONDS*/
unsigned char min=0;/*COUNTER FOR MINUTES*/
unsigned char hr=0; /*COUNTER FOR HOURS*/

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_COMPA_vect)
{
	sec++;
	if(sec==60)
	{
		sec=0;
		min++;
	}
	if(min==60)
	{
		min=0;
		hr++;
	}
	if(hr==24)
	{
		sec=0;
		min=0;
		hr=0;
	}
}

ISR(INT0_vect)
{
	sec=0;
	min=0;
	hr=0;
}

ISR(INT1_vect)
{
	/*No clock source (Timer/Counter stopped)*/
	TCCR1B &= (~(1<<CS12)) & (~(1<<CS11)) & (~(1<<CS10));
}

ISR(INT2_vect)
{
	/*Set Timer/Counter Mode of Operation CTC*/
	/*WGM12 = 1*/
	/*Set Prescaler = 64*/
	/*CS10 = 1 , CS11= 1*/
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void TIMER1_CTC(void)
{
	/* Set initial value = 0 in Timer/Counter Register 1 */
	TCNT1  = 0;

	/* Set Compare value = 15625 in Output Compare Register 1A */
	OCR1A  = 15625;

	/*Non-PWM FOC1A = 1*/
	TCCR1A = (1<<FOC1A);

	/*Set Timer/Counter Mode of Operation CTC*/
	/*WGM12 = 1*/
	/*Set Prescaler = 64*/
	/*CS10 = 1 , CS11= 1*/
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);

	/*Set Enable GLOBAL I-BIT = 1*/
	/*Set Output Compare Match Interrupt enable from Timer interrupt mask */
	SREG  |= (1<<7);
	TIMSK |= (1<<OCIE1A);

}

void INT0_Reset(void)
{
	/*Set the I-BIT to enable Interrupts*/
	SREG  |= (1<<7);

	/*Set General Interrupt Control Register to enable external interrupts*/
	GICR  |= (1<<INT0);

	/*Set the MCU Control Register for edge trigger*/
	/*External Interrupt INT0 with falling edge*/
	MCUCR |= (1<<ISC01);
	MCUCR &= (~(1<<ISC00));

	/*PD2/INT0 push button with
	  internal pull-up resistor*/
	DDRD  &= (~(1<<PD2));
	PORTD |= (1<<PD2);

}

void INT1_Pause(void)
{
	/*Set the I-BIT to enable Interrupts*/
	SREG  |= (1<<7);

	/*Set General Interrupt Control Register to enable external interrupts*/
	GICR  |= (1<<INT1);

	/*Set the MCU Control Register for edge trigger*/
	/*External Interrupt INT1 with raising edge*/
	MCUCR |= (1<<ISC11) | (1<<ISC10);

    /*PD3/INT1 push button with the
	external pull-down resistor*/
	DDRD &= (~(1<<PD3));

}

void INT2_Resume(void)
{
	/*Set the I-BIT to enable Interrupts*/
	SREG   |= (1<<7);

	/*Set General Interrupt Control Register to enable external interrupts*/
	GICR   |= (1<<INT2);

	/*Set the MCU Control and Status Register for edge trigger*/
	/*External Interrupt INT2 with falling edge*/
	MCUCSR &= (~(1<<ISC2));

	/*PB2/INT2 push button with
	  internal pull-up resistor*/
	DDRB  &= (~(1<<PB2));
	PORTB |= (1<<PB2);

}

int main(void)
{
	DDRC  |= 0x0f;/*set the first 4-pins in PORTC as output*/
	PORTC &= 0xf0;/*Six 7-seg off*/

	DDRA  |= 0x3f;/*set the first 6-pins in PORTA as the enable/disable pins*/
	PORTA |= 0x3f;

	INT2_Resume();
	INT1_Pause();
	INT0_Reset();
	TIMER1_CTC();
	while(1)
	{
		/*SECONDS*/
		PORTA = (1<<0);
		PORTC = sec % 10;
		_delay_ms(2);
		PORTA = (1<<1);
		PORTC = sec / 10;
		_delay_ms(2);

		/*MINUTES*/
		PORTA = (1<<2);
		PORTC = min % 10;
		_delay_ms(2);
		PORTA = (1<<3);
		PORTC = min / 10;
		_delay_ms(2);

		/*HOURS*/
		PORTA = (1<<4);
		PORTC = hr % 10;
		_delay_ms(2);
		PORTA = (1<<5);
		PORTC = hr / 10;
		_delay_ms(2);

	}
}
