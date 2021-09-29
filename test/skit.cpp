for(uint8_t i = 0; i < 28; i++) {
          pixel.setPixelColor(i, elem); // Draw new pixel
          pixel.setPixelColor(i-4, 0); // Erase pixel a few steps back
          pixel.show();
          delay(25);
        }