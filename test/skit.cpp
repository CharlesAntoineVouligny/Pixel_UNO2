while(!init_setflag) {
      click();
      if (counter < 0) {
        counter = 0;
      }
      timeset = counter;
      
      pixel.setPixelColor(timeset, color);
      pixel.show();
      Serial.print(timeset);
      Serial.println(" minutes");
      if (shortpress) {
        init_setflag = true;
        shortpress = false;
        longpress = false; // error-prevention line
        timeset *= 60;
        counter = 0;
        second = 0;
        minute = 0;
        Serial.println("Time set!");
      }
      
    }