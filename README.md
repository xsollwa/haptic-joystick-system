# haptic-joystick-system
A handheld controller that uses air pressure to send haptic feedback.

**Created by:** Ruzanna Gaboyan and Philip Golczak

**Date:** June 7, 2025

## Overview

The **Haptic Joystick System** is a *wireless controller that lets you wirelessly control robot arms in XYR directions* using your hand movement and pressure. It includes a joystick module for motion control in XY directions, a rotary knob for base rotation and a small squeezable air bulb that sends and receives air pressure signals. The controller *reads how hard you're squeezing using an air pressure sensor and can also send pressure back* into the buib to communicate resistance.

The main idea of this project is to make controlling robots more natural by *"feeling what they feel"* in addition to just moving them. This is useful especially in robotics tasks where pressure precision applied by the robot matters: like gripping and squeezing fragile objects. 

## Features
- **Pneumatic haptic feedback system:** Squeeze a small air bulb to send signals to a robot arm.
- **Joystick control**: Move the robot arm in XYR *(R-Rotation)* directions.
- **Two-way haptic feedback communication:** Air pump also sends pressure back into the bulb when robot grips something.
- **Wireless communication:** ESP32 hamdles communication between the controller and the robot over Wi-Fi.
- **Custom PCB and 3D case:** Designed and built from scratch for this project.

## Components
| Part        | Description            |
|-------------|-------------------------|
|ESP32|Microcontroller for communication through Wi-Fi|
|MPX5010DP Pressure Sensor|Measures how hard the bulb is squeezed|
|Joystick Module|Controls X and Y motion|
|Rotary Potentiometer|Controls R motion|
|PCB|Handles power delivery and connections|
|LiPo Battery|Powers the controller|
|3D printed parts|Joystick case and lid|
|Buck converter|Maintain correct voltage|
|Air Pump|Pushes air back into the bulb for feedback|
|MOSFET Transistor|Switch for turning the air pump on/off|

You can also view our full Bill of Materials [here](./MATERIALS-AND-BUDGET.md)

## How it Works
1) Joystick module and knob send movement data wirelessly through ESP32 to the robot arm,
2) When you squeeze the air bulb the pressure sensor reads the force,
3) ESP32 sends that signal to the robot (like our [Remote Assist Hand](https://github.com/xsollwa/remote-assist-hand), but the controller can potentially be used on other robots as well),
4) If the robot grips something, it sends resistance signals back,
5) Air pump activates and sends pressure into the bulb,
6) You feel it!

## Building Process
1) Brainstorming and sketches
2) PCB design in KiCAD
3) CAD modeling in OnShape
4) Revision and Optimization
5) Assembly and testing

## Journal
Our full development journal can be found [here](./JOURNAL.md)

## Contact
For any questions or suggestions please reach out to us at *gaboyanruzanna@gmail.com* or *ph.golczak@gmail.com* .




