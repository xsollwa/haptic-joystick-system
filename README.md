# haptic-joystick-system
A handheld controller that uses air pressure to send haptic feedback.

**Created by:** Ruzanna Gaboyan and Philip Golczak

**Date:** June 7, 2025

## Overview

The **Haptic Joystick System** is a *wireless controller that lets you wirelessly control robot arms in XYR directions* using your hand movement and pressure. It includes a two dual axis joystick modules for motion control in XYR directions, and a small squeezable air bulb that sends and receives air pressure signals. The controller *reads how hard you're squeezing using an air pressure sensor and can also send pressure back* into the bulb to communicate resistance.

**Why we made it**: By this project we wanted to make controlling robots more natural by *"feeling what they feel"* in addition to just moving them. This is useful especially in robotics tasks where pressure precision applied by the robot matters: like gripping and squeezing fragile objects. 

//////Note: we removed the screen feature to bring the cost of the project down.

## Features
- **Pneumatic haptic feedback system:** Squeeze a small air bulb to send signals to a robot arm.
- **Joystick control**: Move the robot arm in XYR *(R-Rotation)* directions.
- **Two-way haptic feedback communication:** Air pumping piston mechanism also sends pressure back into the bulb when robot grips something.
- **Wireless communication:** ESP32 hamdles communication between the controller and the robot over Wi-Fi.
- **Custom PCB and 3D case:** Designed and built from scratch for this project.
- **OLED Display:** A little screen shows live data like grip pressure, feedback intensity and other info.
- **Ergonomic handle options:** In addition to the default casing, we CAD-ed an alternate ergonomic grip design for more comfort that can be added on the controller.

Ergonomic handle options: In addition to the default casing, we CAD-modeled an alternate ergonomic grip design for added comfort and modularity when holding the controller.

### Materials and Budget

## Bill of Materials

| Part Number | Part Name           | Description                                      | Unit Price | Total Price | Link |
|-------------|---------------------|--------------------------------------------------|------------|-------------|------|
| MP1         | ESP32               | Wi-Fi Microprocessor                             | $5.99      | $5.99       | [Amazon](https://www.amazon.com/gp/product/B0DDPJQX3X/ref=ox_sc_act_image_11?smid=A1VTL661FOEJB1&th=1) |
| J1, J2      | Joystick Module (2x)| KY-023 control joystick module                   | $6.79      | $6.79       | [Amazon](https://www.amazon.com/Joystick-Sensor-Module-Controller-KY-023/dp/B08681KFSD/) |
| S1          | AITRIP Pressure Sensor | Barometric Pressure Sensor Module              | $9.99      | $9.99       | [Amazon](https://www.amazon.com/AITRIP-Pressure-Digital-Barometric-Controller/dp/B09KXXBCYX/) |
| PCB1        | PCB                 | Fabricated PCB                                   | $5.00      | $5.00       | [PCBWay](https://pcbway.com/) |
| BT1         | Battery Connector   | Plug in for LiPo battery                         | $3.77      | $3.77       | [Amazon](https://www.amazon.com/gp/product/B0DY5GRSQW/ref=ox_sc_act_title_7?smid=AH55IWDCGXER3&psc=1) |
| BULB        | Pipet bulb          | 4ml Air bulb for pressure sensor                 | $7.98      | $7.98       | [Amazon](https://www.amazon.com/gp/product/B01LZHMLQA/ref=ox_sc_act_title_6?smid=A29JIFH4ZXPNL8&psc=1) |
| TUBE        | Tubes               | Tube to connect the sensor, pump and air bulb    | $7.49      | $7.49       | [Amazon](https://www.amazon.com/uxcell-Silicone-Tubing-Rubber-Transfer/dp/B085N17TDB/) |
| CN1         | Tube T-Connectors   | Connectors for the tube, air bulb, sensor, pump | $11.99     | $11.99      | [Amazon](https://www.amazon.com/gp/product/B06XGCM2RX/ref=ox_sc_act_title_5?smid=A1THAZDOWP300U&psc=1) |
| SYRINGE     | Syringe             | 10ml Syringe for inflating and deflating         | $5.99      | $5.99       | [Amazon](https://www.amazon.com/Plastic-Syringe-Measurement-Scientific-Measuring/dp/B09MYPTD1P/) |
| AT1         | Actuator            | Mini Linear Actuator to push air                 | $19.99     | $19.99      | [Amazon](https://www.amazon.com/Actuator-Electric-Actuators-Robotics-Automation/dp/B0D17H1HGV/) |
| MD1         | Motor Driver        | Stepper Motor Driver Module                      | $6.98      | $6.98       | [Amazon](https://www.amazon.com/WWZMDiB-L298N-H-Bridge-Controller-Raspberry/dp/B0CR6BX5QL/) |
| **TOT**     | **Total (excl. taxes & shipping)** |                                              |            | **$91.96**  |      |
|             | **Total WITH taxes & shipping**   |                                              |            | **$114.50** |      |


## Notes on Purchasing and Shipping

We want to purchase most of the parts from Amazon. We chose Amazon as our primary supplier because we have access to Amazon Prime, which has faster shipping. The prime shipping makes the packs cheaper than the singular alternatives at other sources (like AliExpress). Also, we own some of the parts, like the battery and boost converter, so their cost is put as $0. 

//Note: The difference between Aliexpress sourcing and Amazon sourcing for this project is ~$7.00

## How it Works
1) Joystick modules send movement data wirelessly through ESP32 to the robot arm,
2) When you squeeze the air bulb the pressure sensor reads the force,
3) ESP32 sends that signal to the robot (like our [Remote Assist Hand](https://github.com/xsollwa/remote-assist-hand), but the controller can potentially be used on other robots as well),
4) If the robot grips something, it sends resistance signals back,
5) Air pumping mechanism activates and sends pressure into the bulb,
6) You feel it!
7) OLED screen displays pressure data, joystick values, connection status and more. Pressing either joystick down switched between display modes.

## Pictures 
**PCB**

<p align="center">
  <img src="Progress Images/PCB SchematicV2.png" width="400">
  <img src="Progress Images/PCB RoutingV2.png" width="400">
  <img src="Progress Images/PCB 3D FrontV2.png" width="400">
  <img src="Progress Images/PCB 3D BackV2.png" width="400">
</p>



**3D model**

<p align="center">
  <img src="./CAD/images/CAD Isometric.png" alt="Back with Lid" width="400">
  <br>
  <img src="./CAD/images/CAD Back.png" alt="Back" width="400">
  <br>
  <img src="./CAD/images/CAD Front.png" alt="Front" width="400">
</p>



## Building Process
1) Brainstorming and sketches
2) PCB design in KiCAD
3) CAD modeling in OnShape
4) Coding
5) Revision and Optimization
6) Assembly and testing

## Journal
Our full development journal can be found [here](./JOURNAL.md)

## Contact
For any questions or suggestions please reach out to us at *gaboyanruzanna@gmail.com* or *ph.golczak@gmail.com* .




