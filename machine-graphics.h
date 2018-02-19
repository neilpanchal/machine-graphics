#ifndef __MACHINE_GRAPHICS__
#define __MACHINE_GRAPHICS__

#include <Arduino.h>
#include <Print.h>
#include <SPI.h>

class MG {
protected:
    int pin_cs;   // Chip select
    int pin_dc;   // DC
    int pin_rst;  // Reset

    // Display properties:
    static const int cs_enable = 0;
    static const int cs_disable = 1;
    static const int post_chip_enable_wait_ns = 20;
    static const int pre_chip_disable_wait_ns = 10;
    static const int reset_pulse_width_ms = 100;
    static const int post_reset_wait_ms = 100;
    static const int sda_setup_time_ns = 50;
    static const int sck_pulse_width_ns = 50;
    static const int sck_clock_hz = 10000000UL;
    static const int spi_mode = 0;
    static const int i2c_bus_clock_100kHz = 4;
    static const int data_setup_time_ns = 40;
    static const int write_pulse_width_ns = 150;
    static const int tile_width = 16;
    static const int tile_height = 8;
    static const int default_x_offset = 0;
    static const int x_offset = 0;
    static const int flipmode_x_offset = 0;
    static const int pixel_width = 128;
    static const int pixel_height = 64;

    // Buffer
    static const int tile_buf_height = 8;

    int *buf;  // Use malloc per display memory
    int *tile_buf_ptr;
    int tile_curr_row;
    int page_cnt;  // 8 for some reason?
    int draw_color;
    int is_auto_page_clear;

    // Dimensions
    int buf_x0;
    int buf_x1;
    int buf_y0;
    int buf_y1;
    int pixel_buf_height;
    int pixel_curr_row;
    int height;
    int width;

    SPISettings spi_settings;

public:
	MG(int _pin_cs, int _pin_dc, int _pin_rst);
    // Initialization methods
    void updateDimensions();
    void setBufferCurrTileRow(int arg);
    void clearBuffer();
    void clearFirstPage();
    void drawTile(int x_pos, int y_pos, int cnt, int* tile_ptr);
    void sendTileRow(int src_tile_row, int dest_tile_row);
    void sendBuffer();
    int clearNextPage();
    void clearDisplay();
    void begin();

    void resetDisplay();
	void initDisplay();

	void setCS(int level);
	void setDC(int level);

	void startTransfer(int arg);
	void endTransfer(int arg);
	void sendBytes(int cnt, int* data);
	void sendByte(int data);
	void sendData(int cnt, int* data);
	void sendCmd(int cmd);
	void sendArg(int arg);

	void drawHVLine(int x, int y, int len, int dir);
};

#endif

// class U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI : public U8G2 {
//         public:
//     U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI(const u8g2_cb_t *rotation, uint8_t
//     cs, uint8_t dc,
//                                            uint8_t reset = U8X8_PIN_NONE)
//         : U8G2() {
//         u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, rotation,
//         u8x8_byte_arduino_hw_spi,
//                                            u8x8_gpio_and_delay_arduino);
//         u8x8_SetPin_4Wire_HW_SPI(getU8x8(), cs, dc, reset);
//     }
// };
