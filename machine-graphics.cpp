#include "machine-graphics.h"

MG::MG(int _pin_cs, int _pin_dc, int _pin_res) :
		page_cnt(8),
		pin_cs(_pin_cs),
		pin_dc(_pin_dc),
		pin_res(_pin_rst) {}

// Draw methods

void MG::initDisplay() {}
void MG::clearDisplay() {}

void MG::begin() {}
void MG::clearBuffer() {}
void MG::sendBuffer() {}

// Graphics methods
void MG::drawPixel(int x, int y) {}
void MG::setupDisplay() {

}
void MG::setupBuffer() {
	buffer = malloc(1024)
}

void MG::resetDisplay()	{
	// Set High
	ioReset(1);
	ioDelay(reset_pulse_width_ms);

	// Pull line low for reset
	ioReset(0);
	ioDelay(reset_pulse_width_ms);

	// Set High
	ioReset(1);
	ioDelay(reset_pulse_width_ms);
}

void MG::ioReset() {
	// Do reset stuff
}

void MG::ioDelay() {
	// Do delay stuff
}
