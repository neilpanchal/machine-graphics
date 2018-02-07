#ifndef __MACHINE_GRAPHICS__
#define __MACHINE_GRAPHICS__

#include <Arduino.h>
#include <Print.h>

class MG {
protected:
	int* buffer;
	int pin_cs;
	int pin_dc;
	int pin_rst;

public:

	void initDisplay();
	void clearDisplay();

	void begin();

	void clearBuffer();
	void sendBuffer();

	// Graphics methods
	void drawPixel(int x, int y);
	void setupDisplay();
	void setupBuffer();
	void resetDisplay();
	void ioReset();
	void ioDelay();
}



#endif


// class U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI : public U8G2 {
//         public:
//     U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI(const u8g2_cb_t *rotation, uint8_t cs, uint8_t dc,
//                                            uint8_t reset = U8X8_PIN_NONE)
//         : U8G2() {
//         u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, rotation, u8x8_byte_arduino_hw_spi,
//                                            u8x8_gpio_and_delay_arduino);
//         u8x8_SetPin_4Wire_HW_SPI(getU8x8(), cs, dc, reset);
//     }
// };
