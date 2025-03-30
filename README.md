# ESPHome Somfy cover remote

## Required hardware
- ESP32
- CC1101 RF module

## Setup
```
esphome:
  name: somfycontroller
  libraries:
    - SPI
    - EEPROM
    - SmartRC-CC1101-Driver-Lib
    - Somfy_Remote_Lib

external_components:
  - source: github://HarmEllis/esphome-somfy-cover-remote@main
    components: [ somfy_cover ]

esp32:
  board: esp32dev
  framework:
    type: arduino

# Enable logging
logger:

# Enable Home Assistant API
api:

ota:
  platform: esphome
  password: !secret ota_password

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

  Enable fallback hotspot (captive portal) in case wifi connection fails
  ap:
    ssid: "Somfycontroller Fallback Hotspot"
    password: !secret fallback_hotspot

button:
  - platform: template
    id: "program_livingroom"
    name: "Program livingroom"
  - platform: template
    id: "program_kitchen"
    name: "Program kitchen"
  - platform: template
    id: "program_study"
    name: "Program study"
  - platform: template
    id: "program_bathroom"
    name: "Program bathroom"

cover:
  - platform: somfy_cover
    id: "livingroom"
    name: "Livingroom cover"
    device_class: shutter
    open_duration: 18s
    close_duration: 17s
    cover_remote_code: 0x6b2a03
    cover_prog_button: "program_livingroom"
  
  - platform: somfy_cover
    id: "kitchen"
    name: "Kitchen cover"
    device_class: shutter
    open_duration: 26s
    close_duration: 25s
    cover_remote_code: 0x0bf93b
    cover_prog_button: "program_kitchen"
  
  - platform: somfy_cover
    id: "study"
    name: "Study cover"
    device_class: shutter
    open_duration: 18s
    close_duration: 17s
    cover_remote_code: 0x09a1c3
    cover_prog_button: "program_study"

  - platform: somfy_cover
    id: "bathroom"
    name: "Bathroom Cover"
    device_class: shutter
    open_duration: 18s
    close_duration: 17s
    cover_remote_code: 0x449677
    cover_prog_button: "program_bathroom"

```
