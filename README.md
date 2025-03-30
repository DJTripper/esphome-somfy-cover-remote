# ESPHome Somfy cover remote

## Required hardware
- ESP32
- CC1101 RF module

## Setup
```
esphome:
  name: somfycontroller

external_components:
  - source: github://HarmEllis/esphome-somfy-cover-remote.git
    components: [somfy_cover]

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

cover:
  - platform: somfy_cover
    id: "livingroom"
    name: "Livingroom cover"
    device_class: shutter
    open_duration: 18s
    close_duration: 17s
    cover_remote_code: 0x6b2a03
    cover_prog_button: 
      name: Program livingroom
  
  - platform: somfy_cover
    id: "kitchen"
    name: "Kitchen cover"
    device_class: shutter
    open_duration: 26s
    close_duration: 25s
    cover_remote_code: 0x0bf93b
    cover_prog_button: 
      name: Program kitchen
  
  - platform: somfy_cover
    id: "study"
    name: "Study cover"
    device_class: shutter
    open_duration: 18s
    close_duration: 17s
    cover_remote_code: 0x09a1c3
    cover_prog_button: 
      name: Program study

  - platform: somfy_cover
    id: "bathroom"
    name: "Bathroom Cover"
    device_class: shutter
    open_duration: 18s
    close_duration: 17s
    cover_remote_code: 0x449677
    cover_prog_button: 
      name: Program bathroom

```