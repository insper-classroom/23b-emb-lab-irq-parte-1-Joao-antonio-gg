#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"



#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define LED_PIO           PIOC               
#define LED_PIO_ID        ID_PIOC                
#define LED_PIO_IDX       30         
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)   



#define BUT1_PIO          PIOD
#define BUT1_PIO_ID       ID_PIOD
#define BUT1_PIO_IDX      28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX) 

#define BUT2_PIO          PIOC
#define BUT2_PIO_ID       ID_PIOC
#define BUT2_PIO_IDX      31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define BUT3_PIO          PIOA
#define BUT3_PIO_ID       ID_PIOA
#define BUT3_PIO_IDX      19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX) 


volatile char but1_flag;

volatile char but2_flag;

volatile char but3_flag;


void but1_callback(void)
{
	but1_flag = 1;
}

void but2_callback(void)
{
	but2_flag = 1;
}

void but3_callback(void)
{
	but3_flag = 1;
}


void pisca_led(int n, float delay){
	for (int i=0;i<n;i++){
		pio_clear(LED_PIO, LED_PIO_IDX_MASK);
		for(int Q=1;Q<=127;Q++){
			gfx_mono_draw_rect(i, 5, 0, 10, GFX_PIXEL_SET);
			delay_ms(delay/126.0);
		}
		pio_set(LED_PIO, LED_PIO_IDX_MASK);
		for(int k=127;k>=1;k--){
			gfx_mono_draw_rect(k, 5, 0, 10, GFX_PIXEL_CLR);
			delay_ms(delay/126.0);
		}
	}
}


void init(void)
{

	// Configura led
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIO_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);


	// com pull-up
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_PIO_IDX_MASK, 60);

	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_PIO_IDX_MASK, 100);

	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_PIO_IDX_MASK, 60);

	pio_handler_set(BUT1_PIO,
	BUT1_PIO_ID,
	BUT1_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but1_callback);

	pio_handler_set(BUT2_PIO,
	BUT2_PIO_ID,
	BUT2_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but2_callback);

	pio_handler_set(BUT3_PIO,
	BUT3_PIO_ID,
	BUT3_PIO_IDX_MASK,
	PIO_IT_EDGE,
	but3_callback);

	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT2_PIO);

	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT3_PIO);

	pio_enable_interrupt(LED_PIO, LED_PIO_IDX_MASK);
	pio_get_interrupt_status(LED_PIO);

	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4); 

	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4); 

	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 4); 
}



int main (void)
{
	int delay = 1000;
	board_init();
	sysclk_init();
	delay_init();


	WDT->WDT_MR = WDT_MR_WDDIS;

	init();

	
	gfx_mono_ssd1306_init();

	gfx_mono_draw_string("Freq:", 10,16, &sysfont);
	gfx_mono_draw_string("Hz", 110,16, &sysfont);

	while(1) {
		blink_flag = 1;
		float freq = 1 / (delay/1000);
		char str[128]; 
		sprintf(str, "%.2f", freq);
		gfx_mono_draw_string(str, 60, 16, &sysfont);
		if (but1_flag) {
			blink_flag = 1;
			delay_ms(1000);
			if (!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)) {
				
					delay += 100;
				}
				
				else{
				
					delay -= 100;
				}
				but1_flag = 0;
			}
			
		

		if (but2_flag) {
			blink_flag = 0;
			but2_flag = 0;
		}

		if (but3_flag) {
			blink_flag = 1;
			if (delay >= 4900) {
				delay = 5000;
				} else {
				delay += 100;
			}
			but3_flag = 0;
		}


		if (blink_flag) {
			pisca_led(1, delay);
		}

	}
}