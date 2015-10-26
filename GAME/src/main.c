#include <asf.h>
#include <string.h>

#define SERIAL_CONNECTION	&USARTC0

int main (void)
{
	// Initialize all components
	board_init();
	sysclk_init();
	gfx_mono_init();
	
	// USART options.
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 9600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};

	// Initialize usart driver in RS232 mode
	usart_init_rs232(SERIAL_CONNECTION, &USART_SERIAL_OPTIONS);

	// Enable display backlight
	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	
	// Define game options
	const int MAX_LEVEL = 10;
	const char LEVELS[10][10] = {"6", "64", "647", "6479", "64791", "647912", "6479125", "64791250", "647912502", "6479125026"};

	while(true)
	{
		// Define initial game variables
		uint8_t current_level = 0;
		
		// Print rules and wait for enter
		gfx_mono_draw_string("Welcome!", 1, 0, &sysfont);
		gfx_mono_draw_string("Press enter...", 1, 20, &sysfont);
		
		// Wait for enter
		char input = usart_getchar(SERIAL_CONNECTION);
		while(input != '\r');
		
		while(current_level < MAX_LEVEL)
		{
			// Declare variables
			uint8_t entered_char = '\0';
			uint8_t entered_string[10] = {'\0'};
						
			// Show first level for a moment then clear screen
			gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
			gfx_mono_draw_string(LEVELS[current_level], 1, 0, &sysfont);
			delay_ms(250);
			gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
				
			// Accept numbers
			gfx_mono_draw_string("Enter the numbers...", 1, 0, &sysfont);
				
			uint8_t i = 0;
			while(entered_char != '\r')
			{
				entered_char = usart_getchar(SERIAL_CONNECTION);
				entered_string[i] = entered_char;
				i++;
			}
			
			// Clear screen
			gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
			
			// Check for correctness
			if(strncmp(LEVELS[current_level], entered_string, current_level + 1) == 0)
			{
				// Correct 
				current_level++;
				
				if(current_level == MAX_LEVEL)
				{
					// Final win screen
					gfx_mono_draw_string("You won the game!", 1, 0, &sysfont);
					
					// Restart the game
					current_level = 0;
				}
				else
				{
					// Continue to next level
					gfx_mono_draw_string("Correct, press", 1, 0, &sysfont);
					gfx_mono_draw_string("enter to continue!", 1, 10, &sysfont);	
				}
			}
			else
			{
				// Incorrect answer
				gfx_mono_draw_string("Incorrect, game", 1, 0, &sysfont);
				gfx_mono_draw_string("restarted!", 1, 10, &sysfont);
				gfx_mono_draw_string("Press enter...", 1, 20, &sysfont);
				
				// Clear level stuff
				current_level = 0;
			}
			
			// Wait for enter
			input = usart_getchar(SERIAL_CONNECTION);
			while(input != '\r');
		}
	}
}
