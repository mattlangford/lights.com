To build and upload with the [ardunio-cli](https://arduino.github.io/arduino-cli/0.34/getting-started/) tool
```bash
# Specify the board. This uses the Serial+MIDI4 option (which lets usbMIDI work)
arduino-cli board attach -b teensy:avr:teensy41:usb=serialmidi4

# Compile and upload to the given port
arduino-cli compile -u -p /dev/tty.usbmodem101

# Monitor the serial output (may require a different port)
arduino-cli monitor -p /dev/tty.usbmodem101
```

## Libraries
 * ArduinoJson
 * ArduinoSTL

## Lights
These are the lights I have and the associated manuals:
 * [Missyee36](https://images-na.ssl-images-amazon.com/images/I/B1TmOuEgH2S.pdf) RGB light
 * [Electro86](https://cdb.s3.amazonaws.com/ItemRelatedFiles/13224/electro86.pdf) (Maybe called litake?) RGB light 
 * [Betopper](https://cdn.shopify.com/s/files/1/0084/5230/9047/files/BETOPPER_Moving_Head_Stage_Led_Light_LM70_LM70S_Standard_User_Manual.pdf?v=1630045250) Moving Light
 * [Washbar112](https://m.media-amazon.com/images/I/C1CE5BB5CrL.pdf) Bar light (not sure of the name)