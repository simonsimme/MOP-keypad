
#include <stdint.h>

unsigned char readColumn();
unsigned char keyb(void);
#define GPIOD_BASE 0x40020C00
#define GPIOD_CRL  (*(volatile uint32_t *)(GPIOD_BASE))
#define GPIOD_CRH  (*(volatile uint32_t *)(GPIOD_BASE + 0x04))
#define GPIOD_IDR  (*(volatile uint32_t *)(GPIOD_BASE + 0x08))
#define GPIOD_ODR  (*(volatile uint32_t *)(GPIOD_BASE + 0x0C))

void main(void){
upInint();
while (1)
{
    out7seg(keyb());
}

    
}
void startUp(void){

}
void upInint(void){
    *((unsigned short *) 0x40020c00) = 0x0055;
}
unsigned char keyb(void){

    for (int row = 0; row < 4; row++) {
        activateRow(row);
        unsigned char column = readColumn();
        if (column != 0) {
            // Find the pressed column (assuming active high)
            for (int col = 0; col < 4; col++) {
                if (column & (1 << (col + 4))) { // Columns on PD4-7
                    printf("row is: %d\n and col is %d\n", row,col);
                    return (row * 4) + col + 1; // Key encoding (1-16)
                }
            }
        }
    }
    return 0xFF; // No key pressed
    
}
void activateRow(int row) {
    // Implement the logic to activate the specified row
    // Example: Set the corresponding bit in the output register
    *((unsigned short *) 0x40020c00) = (1 << row);
}
void out7seg(unsigned char c){
    const unsigned char seg7[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
    };

    *((unsigned char *)0x4002080C) = (c == 0xFF) ? 0x00 : seg7[c % 16];

}

unsigned char readColumn() {
    // Implement the logic to read the column value from PD0-7
    // Example: Read the input register and return the value
    return (GPIOD_IDR >> 4) & 0x0F;
}