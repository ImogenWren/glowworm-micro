<img width="693" height="161" alt="glowworm-logo-export" src="https://github.com/user-attachments/assets/c39ae390-12af-45af-bcf6-94ba85d72106" />
# glowworm-micro
Project to build a small battery powered handheld lighting controller for creative macro photography


## Project Due Data: 25/12/25

## Desired Features
- Arduino IoT33 Controller (could be ported to ESP32)
- 1S Lipo battery pack with charger
- Power on and off device using external interrupts
- Rotary Encoder for changing values<img width="693" height="161" alt="glowworm-logo-export" src="https://github.com/user-attachments/assets/c90c7176-4a7a-428a-82e8-9ccf69c4ba16" />

- Buttons to select parameter
- Detachable WS2821 Led Strip with 3D printed diffuser options
- 3D printed & ergonomic case
- fastLED library

- 2 Channel Lighting, each channel will have independent
    - Hue
    - Saturation
    - Brightness

Additional control ->  blend mode
- Unblended -> Colours remain seperate
- Direct Blend ->  blend will mix colours directly  
- Colour wheel short blend -> two channels will blend via the shortest route around the colour wheel between them
- colour wheel long blend -> two channels will blend via the longest path around the colour wheel


## Strech Goals
- Save & Recall current settings on shutdown/powerup
- OLED screen displaying all parameters
- highlight currently selected parameter
- Battery indicator
- PCB for control system
- PCB for LED Strip
- Advanced colour palettes

