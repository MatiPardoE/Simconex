#include "neo_pixel.h"

NeoPixel::NeoPixel(int pin, int numPixels) : pin(pin), numPixels(numPixels)
{
    pixels = Adafruit_NeoPixel(numPixels, pin);
}

NeoPixel::~NeoPixel()
{
    // Destructor implementation
}

void NeoPixel::begin()
{
    pixels.begin();
    pixels.clear();
    for (int i = 0; i < numPixels; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 255, 255)); // White
    }
    pixels.show();
}

void NeoPixel::setState(neoPixelState state)
{
    switch (state)
    {
    case OFF:
        pixels.clear();
        break;
    case MANUAL_MODE:
        for (int i = 0; i < numPixels; i++)
            pixels.setPixelColor(i, pixels.Color(0, 0, 255)); // Blue
        break;
    case CYCLE_RUNNING:
        for (int i = 0; i < numPixels; i++)
            pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green
        break;
    case CYCLE_PAUSED:
        for (int i = 0; i < numPixels; i++)
            pixels.setPixelColor(i, pixels.Color(255, 255, 0)); // yellow
        break;
    case CYCLE_COMPLETED:
        for (int i = 0; i < numPixels; i++)
            pixels.setPixelColor(i, pixels.Color(255, 0, 255)); // Example color
        break;
    case CYCLE_ERROR:
        for (int i = 0; i < numPixels; i++)
            pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red color for error
        break;
    case CALIB_PH:
        for (int i = 0; i < numPixels; i++)
            pixels.setPixelColor(i, pixels.Color(148, 3, 252)); // Example color
        break;
    case CALIB_OD:
        for (int i = 0; i < numPixels; i++)
            pixels.setPixelColor(i, pixels.Color(148, 3, 252)); // Example color
        break;
    case NEW_INTERVAL:
        for (int i = 0; i < numPixels; i++)
            pixels.setPixelColor(i, pixels.Color(255, 128, 0)); // Example color
        break;
    default:
        break;
    }
    pixels.show();
}

void NeoPixel::initFun()
{
    for (int j = 0; j < 256; j++) { // Cycle through the color wheel
        for (int i = 0; i < numPixels; i++) {
            pixels.setPixelColor(i, pixels.ColorHSV((i * 256 / numPixels + j) & 255, 255, 255));
        }
        pixels.show();
        delay(20); // Adjust delay for speed of rainbow effect
    }
    pixels.show();
}
