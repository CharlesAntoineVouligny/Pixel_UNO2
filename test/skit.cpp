counter = map(bright, 0, 255, 0, 25);
          while(!shortpress) {
          click();
          bright = constrain(map(counter, 0, 25, 0 , 255), 0, 255);
          Serial.print("Brightness: ");
          Serial.println(bright);
          settingDisplay();
          delay(25);