To build and upload with the [ardunio-cli](https://arduino.github.io/arduino-cli/0.34/getting-started/) tool
```bash
# Specify the board. This uses the Serial+MIDI4 option (which lets usbMIDI work)
$ arduino-cli board attach -b teensy:avr:teensy41:usb=serialmidi4 dmx

# Compile and upload to the given port
arduino-cli compile -u -p /dev/tty.usbmodem101 dmx

# Monitor the serial output
cat /dev/cu.usbmodem101
```

## Libraries
 * ArduinoJson
 * ArduinoSTL
