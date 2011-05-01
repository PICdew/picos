/*
 *	LCD interface header file
 *	See lcd.c for more info
 */

#define LCD_WIDTH 8
#define LCD_EOF 16


// Keeps track of the position on the lcd.
// Zero indexed mod LCD_WIDTH
char lcd_pos;

/**
 * Write data to the lcd using 4-bit mode
 * Note: LCD_RS needs to be set/cleared before
 * calling lcd_write
 */
extern void lcd_write(unsigned char);

/**
 * Clear the LCD and return to home position (lcd_pos = 0)
 */
extern void lcd_clear(void);

/**
 * Writes a string of characters to the LCD 
 */
extern void lcd_puts(const char * s);

/**
 * Go to the specified position.
 * Top left most character is 0
 * Buttom left most character is 0x40
 */
extern void lcd_goto(unsigned char pos);
	
/**
 * Intializes the LCD
 * MUST be called  before anything else!!
 */
extern void lcd_init(void);

/**
 * Write a character to the lcd. Writing
 * is done circularly. So, a write to 
 * position LCD_EOF + 1 will wrap around 
 * to the beginning.
 */
extern void lcd_putch(char);

/**
 * Set the cursor position 
 */
#define	lcd_cursor(x)	lcd_write(((x)&0x7F)|0x80)

