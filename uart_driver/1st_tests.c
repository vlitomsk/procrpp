#define F_CPU 8000000UL 
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define BAUDRATE 19200
#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 8UL)) / ((uint32_t)(baudRate) * 16UL) - 1)
#define UART_BAUD_HIGH	UBRRH
#define UART_BAUD_LOW	UBRRL
#define UART_STATUS	UCSRA
#define UART_TXREADY	UDRE
#define UART_RXREADY	RXC
#define UART_DOUBLE	U2X
#define UART_CTRL	UCSRB
#define UART_CTRL_DATA	((1<<TXEN) | (1<<RXEN))
#define UART_CTRL2	UCSRC
#define UART_CTRL2_DATA	((1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0))
#define UART_DATA	UDR

#define SC SendChar

uint8_t val;
uint8_t blink = 1;
uint8_t nBoot = 1;
char mask[20];
uint8_t iter;
uint8_t data[6];
uint8_t adress;

uint8_t i;

static uint8_t toHex(uint8_t data) {
	uint8_t hex;
	if(data < 'A')
		hex = data - 48;
	else
		hex = data - 55;
	return hex;
}

static void SendChar(uint8_t data) {
	while (!(UART_STATUS & (1<<UART_TXREADY)));
	UART_DATA = data;
}

/*	 servo 
	PORTB &= ~(1<<2);
	_delay_ms(20);		
	
	MOTOR
	
	PORTB &= ~(1<<2);
	_delay_ms(6);		
*/
ISR(USART_RXC_vect) {
	val = UART_DATA;
	if(val=='1'){
		SC('S'); // Right
		PORTB |= (1<<2);
		_delay_ms(1.2);
		PORTB &= ~(1<<2);
		_delay_ms(6);		
	}
	if(val=='2') {

		SC('M'); // Left
		PORTB |= (1<<2);
		_delay_ms(2);
		PORTB &= ~(1<<2);
		_delay_ms(6);		
	 }
	if(val=='3') {

		SC('N'); // F
		PORTB |= (1<<2);
		_delay_ms(1.6);
		PORTB &= ~(1<<2);
		_delay_ms(6);		
	}
		
}


int main(void){
	/* ***********************************
	                UART SET UP
	*************************************/
	UART_BAUD_HIGH = (UART_CALC_BAUDRATE(BAUDRATE)>>8) & 0xFF; //set baudrate
	UART_BAUD_LOW = (UART_CALC_BAUDRATE(BAUDRATE) & 0xFF);
	UART_CTRL = UART_CTRL_DATA; //set enable UART
	UART_CTRL2 = UART_CTRL2_DATA; //set byte and other features
	UCSRB |= (1 << RXCIE); // enable interrupts
	
	sei(); // allow really all interrupts
	
	
	/* **********************************
	 *               PORTS ASSIGNMENT
	 ***********************************/
	DDRB |= (1 << 2); //LED
	
	DDRC |= (1 << 2); 
	DDRC |= (1 << 4); 
	/*
	_delay_ms(2000);
	SC(' '); //first terminal byte
	SC('R');
	SC('E');
	SC('A');
	SC('D');
	SC('Y');
	/***********************************
	 *             THE MAIN LOOP
	 * ********************************/
	while (1)
	{
	
	SC('S');
	SC('T');
	SC('A');
	SC('R');
	SC('T');
	SC(' ');
	_delay_ms(1000);
	};
	return 0;
}