
__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}


#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
typedef int            bool;

#define true 1
#define false 0

#endif 

#ifndef _KEYB_H_
#define _KEYB_H_


/* GPIOD */

#define GPIOD            0x40020C00
#define GPIOD_MODER_LOW  (GPIOD)
#define GPIOD_MODER_HIGH (GPIOD +  2) // adding to the correct port
#define GPIOD_OTYPER     (GPIOD +  4)
#define GPIOD_OSPEEDR    (GPIOD +  8)
#define GPIOD_PUPDR      (GPIOD + 12)
#define GPIOD_IDR        (GPIOD + 16)
#define GPIOD_ODR        (GPIOD + 20)
#define GPIOD_BSRR       (GPIOD + 24)


/* GPIOE */

#define GPIOE            0x40021000
#define GPIOE_MODER_LOW  (GPIOE)
#define GPIOE_MODER_HIGH (GPIOE +  2)
#define GPIOE_OTYPER     (GPIOE +  4)
#define GPIOE_OSPEEDR    (GPIOE +  8)
#define GPIOE_PUPDR      (GPIOE + 12)
#define GPIOE_IDR        (GPIOE + 16)
#define GPIOE_ODR        (GPIOE + 20)
#define GPIOE_BSRR       (GPIOE + 24)


#endif // _KEYB_H_  
#define IS_DEBUG 0   
                     


void app_init(void)
{
    *(volatile ulong*)0x40023830 = 0x18;
    *(volatile uint*)0x40020C08 = 0x55555555;

    *(volatile ushort*) GPIOD_MODER_LOW  = 0x5555;
    *(volatile ushort*) GPIOD_MODER_HIGH = 0x5500;
    *(volatile ushort*) GPIOD_OTYPER    &= 0x00FF;
    *(volatile uint*)   GPIOD_PUPDR     &= 0x0000FFFF;
    *(volatile uint*)   GPIOD_PUPDR     |= 0x00AA0000;
}



const uchar KEYCODE[4][4] =
{ {  1, 2,  3, 10 }
, {  4, 5,  6, 11 }
, {  7, 8,  9, 12 }
, { 14, 0, 15, 13 }
};

const char KEYCODE_INTO_7SEG[] =
{ 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
  0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};


// =============================================================================
// FUNCTIONS

void activate_row(uint);      // Activates a specific row.
int read_column();            // Reads each column and returns the index of the
                              // key being pressed, if that.
uchar key_value(uint, uint);  // Return the value of the key being pressed.



uchar keyb(void)
{
    for (uint row = 1; row <= 4; row++)
    {
        activate_row(row);
        uint column = read_column();
        if (column)
            return key_value(row, column);
    }

    return (uchar)0xFF;
}



void activate_row(uint row)
{
    volatile uchar *gpiod_odr_high = (volatile uchar*)(GPIOD_ODR + 1);

    switch (row)
    {
    case 1:
        *gpiod_odr_high = 0x10; break;

    case 2:
        *gpiod_odr_high = 0x20; break;

    case 3:
        *gpiod_odr_high = 0x40; break;

    case 4:
        *gpiod_odr_high = 0x80; break;

    default: break;
        // NAUGHTY MOVE
    }
}



int read_column()
{
    // Get the high-byte of IDR.
    uchar c = *(volatile uchar*)(GPIOD_IDR + 1);

    if ( c & 0b1000 )
        return 4;

    if ( c & 0b0100 )
        return 3;

    if ( c & 0b0010 )
        return 2;

    if ( c & 0b0001 )
        return 1;

    return 0;
}

void out_7_seg(uchar keycode)
{
    volatile uchar *gpiod_odr_low = (volatile uchar*)GPIOD_ODR;

    if ( 0 <= keycode && keycode <= 15 )
        *gpiod_odr_low = KEYCODE_INTO_7SEG[keycode];
    else
        *gpiod_odr_low = 0;
}



uchar key_value(uint row, uint col)
{
    row--;
    col--;

    return KEYCODE[row][col];
}



void main(void)
{
    app_init();
    *(volatile uchar*)GPIOD_ODR = 0x02;

    while (true)
    {
        out_7_seg( keyb() );
#if IS_DEBUG
        while (true) {}
#endif
    }
}