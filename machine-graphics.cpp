#include "machine-graphics.h"
#include <Arduino.h>
#include <SPI.h>

// main.cpp

// D1 = new MG(CS, DC, RST);
// D1.resetDisplay();
// D1.begin();

MG::MG(int _pin_cs, int _pin_dc, int _pin_rst) {
    pin_cs = _pin_cs;
    pin_dc = _pin_dc;
    pin_rst = _pin_rst;
    buf = (int*)malloc(1024);
    draw_color = 1;
    is_auto_page_clear = 1;
    tile_buf_ptr = buf;
    updateDimensions();
    SPISettings spi_settings(sck_clock_hz, SPI_MSBFIRST, spi_mode);
}

// Draw methods

void MG::updateDimensions() {
    int t = tile_buf_height;
    t *= 8;
    pixel_buf_height = t;
    t = tile_width;
    if (t >= 32) t = 31;
    t *= 8;
    pixel_curr_row = t;
    t = tile_buf_height;
    /* handle the case, where the buffer is larger than the (remaining) part of
     * the display */
    if (t + tile_curr_row > tile_height) {
        t = tile_height - tile_curr_row;
    }
    t *= 8;
    buf_y0 = pixel_curr_row;
    buf_y1 = buf_y0;
    buf_y1 += t;

    height = pixel_curr_row;
    width = pixel_height;
}
void MG::setBufferCurrTileRow(int arg) {
    tile_curr_row = arg;
    updateDimensions();
}
void MG::clearBuffer() {
    int cnt = tile_width;
    cnt *= tile_buf_height;
    cnt *= 8;
    memset(tile_buf_ptr, 0, cnt);
}
void MG::clearFirstPage() {
    int cnt = tile_width;
    cnt *= tile_buf_height;
    cnt *= 8;
    memset(tile_buf_ptr, 0, cnt);
    setBufferCurrTileRow(0);
}

void MG::drawTile(int x_pos, int y_pos, int cnt, int* tile_ptr) {
	int x, c;
	int *ptr;
	startTransfer(0);
	x = x_pos;
	x *= 8;
	x += x_offset; // Check fonts, for now set to 0

	sendCmd(0x040); /* set line offset to 0 */
	sendCmd(0x010 | (x >> 4));
	sendArg(0x000 | ((x & 15))); /* probably wrong, should be SendCmd */
	sendArg( 0x0b0 | y_pos); /* probably wrong, should be SendCmd */

	do {
	    c = cnt;
	    ptr = tile_ptr;
	    sendData(c*8, ptr);
	    /* note: SendData can not handle more than 255 bytes */
	    cnt--;
	} while (cnt > 0);

	endTransfer(0);
}
void MG::sendTileRow(int src_tile_row, int dest_tile_row) {
    int* ptr;
    int offset;
    int w;

    w = tile_width;
    offset = src_tile_row;
    ptr = tile_buf_ptr;
    offset *= w;
    offset *= 8;
    ptr += offset;
    drawTile(0, dest_tile_row, w, ptr);
}

void MG::sendBuffer() {
    int src_row;
    int src_max;
    int dest_row;
    int dest_max;

    src_row = 0;
    src_max = tile_buf_height;
    dest_row = tile_curr_row;
    dest_max = tile_height;

    do {
        sendTileRow(src_row, dest_row);
        src_row++;
        dest_row++;
    } while (src_row < src_max && dest_row < dest_max);
}
int MG::clearNextPage() {
    int row;
    sendBuffer();
    row = tile_curr_row;
    row += tile_buf_height;
    if (row >= tile_height) {
        return 0;
    }
    clearBuffer();
    setBufferCurrTileRow(row);
    return 1;
}
void MG::clearDisplay() {
    clearFirstPage();
    do {
    } while (clearNextPage());
    setBufferCurrTileRow(0);
}
void MG::begin() {
    // GPIO init
    pinMode(pin_cs, OUTPUT);
    pinMode(pin_dc, OUTPUT);
    pinMode(pin_rst, OUTPUT);
    setCS(cs_disable);
    SPI.begin();  // you could here define the SPI bus and pins, TODO
    initDisplay();
    clearDisplay();
}
void MG::resetDisplay() {
    // Set High
    digitalWrite(pin_rst, 1);
    delay(reset_pulse_width_ms);

    // Pull line low for reset
    digitalWrite(pin_rst, 0);
    delay(reset_pulse_width_ms);

    // Set High
    digitalWrite(pin_rst, 1);
    delay(reset_pulse_width_ms);
}
void MG::initDisplay() {
    // enable chip, delay is part of the transfer start
    startTransfer(0);
    // display off
    sendCmd(0x0ae);
    // clock divide ratio (0x00=1) and oscillator frequency (0x8)
    sendCmd(0x0d5);
    sendArg(0x080);
    // multiplex ratio
    sendCmd(0x0a8);
    sendArg(0x03f);
    // display offset
    sendCmd(0x0d3);
    sendArg(0x000);
    // set display start line to 0
    sendCmd(0x040);
    // [2] charge pump setting (p62): 0x014 enable, 0x010 disable, SSD1306 only,
    // should be removed for SH1106
    sendCmd(0x08d);
    sendArg(0x014);
    //  page addressing mode
    sendCmd(0x020);
    sendArg(0x000);
    // segment remap a0/a1*/
    sendCmd(0x0a1);
    // c0: scan dir normal, c8: reverse
    // Flipmode
    // U8X8_C(0x0a0),				/* segment remap a0/a1*/
    // U8X8_C(0x0c0),				/* c0: scan dir normal, c8: reverse
    sendCmd(0x0c8);
    // com pin HW config, sequential com pin config (bit 4), disable left/right
    // remap (bit 5)
    sendCmd(0x0da);
    sendArg(0x012);
    // [2] set contrast control */
    sendCmd(0x081);
    sendArg(0x0cf);
    // [2] pre-charge period 0x022/f1*/
    sendCmd(0x0d9);
    sendArg(0x0F1);
    /* vcomh deselect level */
    sendCmd(0x0db);
    sendArg(0x040);
    // Deactivate scroll
    sendCmd(0x02e);
    // output ram to display
    sendCmd(0x0a4);
    // none inverted normal display mode
    sendCmd(0x0a6);
    // disable chip
    endTransfer(0);
}
void MG::setCS(int level) {
    // Chip select
    digitalWrite(pin_cs, level);
}
void MG::setDC(int level) {
    // byte_cb(u8x8, U8X8_MSG_BYTE_SET_DC, arg, NULL);
    digitalWrite(pin_dc, level);
}
void MG::startTransfer(int arg) {
    SPI.beginTransaction(spi_settings);
    setCS(cs_enable);
    delayMicroseconds(1);  // post_chip_enable_wait_ns = 10 ns but wait for 1 ms
}
void MG::endTransfer(int arg) {
    delayMicroseconds(1);  // pre_chip_disable_wait_ns
    setCS(cs_disable);
    SPI.endTransaction();
    // byte_cb(u8x8, U8X8_MSG_BYTE_END_TRANSFER, 0, NULL);
}
void MG::sendBytes(int cnt, int* data) {
    // u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_SEND, cnt, (void *)data);
    int* data_ptr = data;
    while (cnt > 0) {
        SPI.transfer(*data_ptr);
        data_ptr++;
        cnt--;
    }
}
void MG::sendByte(int data) {
    // Send 1 byte
    sendBytes(1, &data);
}
void MG::sendData(int cnt, int* data) {
    // TBD
    setDC(1);
    sendBytes(cnt, data);

}
void MG::sendCmd(int cmd) {
    setDC(0);
    sendByte(cmd);
}
void MG::sendArg(int arg) {
    setDC(0);
    sendByte(arg);
}

void MG::drawHVLine(int x, int y, int len, int dir) {

}
