To build and upload with the [ardunio-cli](https://arduino.github.io/arduino-cli/0.34/getting-started/) tool
```bash
# Specify the board. This uses the Serial+MIDI4 option (which lets usbMIDI work)
arduino-cli board attach -b teensy:avr:teensy41:usb=serialmidi4

# Compile and upload to the given port
arduino-cli compile -u -p /dev/tty.usbmodem101

# Monitor the serial output (may require a different port)
arduino-cli monitor -p /dev/tty.usbmodem101
```

## PlatformIO
My current plan is to put all teensy specific code in `src/` - these interact with FreeRTOS or other teensy specific hardware calls. Supporting  libraries will be in `lib/<name>` - these don't have specific teensy calls and can be tested locally.
```bash
# Test using gtest on the native arch
pio test -e native

# Build for teensy target
pio run -e teensy41
```

## NanoPB
Protobuf messages are generated with protoc. Install with:
```bash
pip3 install --upgrade protobuf nanopb
```

And then generate with:
```bash
protoc \
    -Ilib/config \
    --plugin=protoc-gen-nanopb=$(python3 -c "import nanopb; print(nanopb.__path__[0])")/generator/protoc-gen-nanopb \
    --nanopb_out=lib/config \
    $(find lib/config -name "*.proto")
```

## Libraries
 * ArduinoJson
 * ArduinoSTL

## Lights
These are the lights I have and the associated manuals:
 * [Missyee36](https://images-na.ssl-images-amazon.com/images/I/B1TmOuEgH2S.pdf) RGB light
 * [Electro86](https://cdb.s3.amazonaws.com/ItemRelatedFiles/13224/electro86.pdf) (Maybe called litake?) RGB light 
    - The DIP switch settings seem correct according to that manual
    - The DMX channels seem wrong. I found:
        0: 0-127 brightness 128-255 strobe
        1: red
        2: green
        3: blue
 * [Betopper](https://cdn.shopify.com/s/files/1/0084/5230/9047/files/BETOPPER_Moving_Head_Stage_Led_Light_LM70_LM70S_Standard_User_Manual.pdf?v=1630045250) Moving Light
 * [Washbar112](https://m.media-amazon.com/images/I/C1CE5BB5CrL.pdf) Bar light (not sure of the name)
