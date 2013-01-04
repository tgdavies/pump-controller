
typedef union 
{ 
  struct 
  { 
    unsigned char b0:1; 
    unsigned char b1:1; 
    unsigned char b2:1; 
    unsigned char b3:1; 
    unsigned char b4:1; 
    unsigned char b5:1; 
    unsigned char b6:1; 
    unsigned char b7:1; 
  }; 
  unsigned char val; 
} byte_t;

#define PORTA_byte (*((volatile byte_t*) &PORTA)) 
#define PORTB_byte (*((volatile byte_t*) &PORTB)) 
#define DDRA_byte (*((volatile byte_t*) &DDRA)) 
#define DDRB_byte (*((volatile byte_t*) &DDRB)) 
#define PINA_byte (*((volatile byte_t*) &PINA)) 
#define PINB_byte (*((volatile byte_t*) &PINB)) 

// defines for each port
#define A	(1)
#define B	(2)

#define readCNT1(X)         X = TCNT1L; \
        					X += TCNT1H << 8;
        					
#define setBit(port, bit) if (port == A) { PORTA |= (1 << bit); } else { PORTB |= (1 << bit); } 
#define clearBit(port, bit) if (port == A) { PORTA &= ~(1 << bit); } else { PORTB &= ~(1 << bit); } 
#define testBit(var, port, bit) if (port == A) { var = PINA & (1 << bit); } else { var = PINB & (1 << bit); } 

#define setDDRBit(port, bit) if (port == A) { DDRA |= (1 << bit); } else { DDRB |= (1 << bit); } 
#define clearDDRBit(port, bit) if (port == A) { DDRA &= ~(1 << bit); } else { DDRB &= ~(1 << bit); }

#define enablePCINT(port, bit) if (port == A) { PCMSK0 |= (1 << bit); } else { PCMSK1 |= (1 << bit); } 

#define LED1_DD (DDRA_byte.b1)
#define LED1_P (PORTA_byte.b1)
#define LED2_DD (DDRA_byte.b0)
#define LED2_P (PORTA_byte.b0)

#define setup_leds() {LED1_DD = 1; LED2_DD = 1;}

#define red(on) LED1_P = ~(on)
 
#define green(on) LED2_P = ~(on)
	
#define timer_diff(start, end) (start > end) ? (0xffff - start + end) : (end - start)
