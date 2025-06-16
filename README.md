# haptic-joystick-system
A handheld controller that uses air pressure to send haptic feedback.

**Created by:** Ruzanna Gaboyan and Philip Golczak

**Date:** June 7, 2025

## Overview

The **Haptic Joystick System** is a *wireless controller that lets you wirelessly control robot arms in XYR directions* using your hand movement and pressure. It includes a joystick module for motion control in XY directions, a rotary knob for base rotation and a small squeezable air bulb that sends and receives air pressure signals. The controller *reads how hard you're squeezing using an air pressure sensor and can also send pressure back* into the buib to communicate resistance.

**Why we made it**: By this project we wanted to make controlling robots more natural by *"feeling what they feel"* in addition to just moving them. This is useful especially in robotics tasks where pressure precision applied by the robot matters: like gripping and squeezing fragile objects. 

## Features
- **Pneumatic haptic feedback system:** Squeeze a small air bulb to send signals to a robot arm.
- **Joystick control**: Move the robot arm in XYR *(R-Rotation)* directions.
- **Two-way haptic feedback communication:** Air pump also sends pressure back into the bulb when robot grips something.
- **Wireless communication:** ESP32 hamdles communication between the controller and the robot over Wi-Fi.
- **Custom PCB and 3D case:** Designed and built from scratch for this project.

### Materials and Budget

| Part Number | Part Name                     | Description                       | Unit Price | Total Price |
| :---------- | :---------------------------: | :-------------------------------: | :--------: | ----------: |
| U3          | ESP32                         | [Wi-Fi Microprocessor][1]         | $10        | $10         |
| BT1         | Urgenex Lipo Battery          | [Battery for the Controller][2]   | $27        | $27         |
| U1          | Buck Converter 4mm (2x)       | [Buck Converter for 7.4-5v][10]   | $8         | $8          |
| R1          | 10k Resistor (100x)           | [Resistor for voltage divider][3] | $6         | $6          |
| R2          | 15k Resistor (100x)           | [Resistor for voltage divider][4] | $6         | $6          |
| J2          | ESP32 Joystick Module (2x)    | [XY control joystick module][5]   | $7         | $7          |
| U4          | MPX5010DP sensor              | [Pressure sensor][6]              | $25        | $25         |
| RV1         | Rotary Potentiometer (5x)     | [Potentiometer for R movement][7] | $10        | $10         |
| J1          | Air Pump                      | [DC mini air/vacuum pump][8]      | $8         | $8          |
| Q2          | MOSFET Transistor (10x)       | [IRLZ44NPBF Logic Level Mosfet][9]| $10        | $10         |
| D1          | Diode (100x)                  | [1N5819 Schottky Diodes][11]      | $3         | $3          |
| PCB         | PCB                           | [Fabricated PCB][12]              | $5         | $5          |
| HDR         | Header Pins (20x)             | [Header Pins to mount joystick/sensor/ESP32][13]| $9 | $9    |	        
| CN1         | Battery Connector             | [Plug in for LiPo battery][14]    | $8         | $8          |
|AB1|	Pipet bulb	|[Air bulb for pressure sensor][15]|	$8|	$8	|																				
|	TUBE|		Tubes	|	[Tube to connect the sensor, pump and air bulb][16]|		$8|		$8|																						
|	CN2	|	Tube T-Connectors	|[Connectors for the tube, air bulb, sensor and pump][17]|		$6|		$6	|																					


[1]: https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z/ref=sr_1_7?crid=3215NDM97THDZ&dib=eyJ2IjoiMSJ9.XBINg-sjhfF_gUtnMiKGjhlE-f5AuPRamTr33nRxSXkGLv_o48kwC8Ijeis6JInJV0KyHBRH7xGJQ-1txsZL4_5QVucvaXvokYACu1kJYTispfjw86LMs4pUaEb3QBf2tCHnMbfhxKmN1GqPyMwCe0JXg3RkQWr3XzxjTqvRC2Vi1yxUnR4MgBHJZC4l4B3sckUL9U6HKhcOjM0hclVNu3VH8A-i1EauSL7KfuJcGiQ.nLKf6GH-ppYlGRm2A-K7bqKU-ETPi6PvqRFLruHjBsE&dib_tag=se&keywords=esp+32&qid=1749497283&sprefix=esp+%2Caps%2C96&sr=8-7

[2]: https://www.amazon.com/URGENEX-Battery-1800mAh-Rechargeable-Campatibal/dp/B0924MM61Z/ref=sxin_17_sbv_search_btf?content-id=amzn1.sym.8aea4788-5372-43c5-bde7-3d239eb02a51%3Aamzn1.sym.8aea4788-5372-43c5-bde7-3d239eb02a51&crid=3Q08CKQ0G2YS0&cv_ct_cx=2s+lipo&keywords=2s+lipo&pd_rd_i=B0924MM61Z&pd_rd_r=2fee5f48-be65-4bef-8665-56cf135858b5&pd_rd_w=GqGDQ&pd_rd_wg=o6vhg&pf_rd_p=8aea4788-5372-43c5-bde7-3d239eb02a51&pf_rd_r=TBJT25WAE9YMH6TNV576&qid=1748989214&sbo=RZvfv%2F%2FHxDF%2BO5021pAnSA%3D%3D&sprefix=2s+lipo%2Caps%2C112&sr=1-1-5190daf0-67e3-427c-bea6-c72c1df98776

[3]: https://www.amazon.com/EDGELEC-Resistor-Tolerance-Multiple-Resistance/dp/B07QJB31M7/ref=sr_1_1_sspa?crid=1J60H25QWSLCW&dib=eyJ2IjoiMSJ9.6ytsDc_SLGT7AJp0OfWBjoCDAB6gzLHDZDoKGFVzEQ3THOTbt6Kn3kNaqJKRXq-WAuH3uBKmSpWpA_eLq0MN49kLD38zZawAxMxdJtvrQH-h_xg7Ril56gv9u4-WCurHKP4k6gTRCtYAzrr2x_YNlo6O0CWq3usIlwcWLa9WMxkt6x7Ul3r87SC1oi9lLShR1Dsx9siEIohWAxbhgnCfJzTfFmEcOolCSy9I76z_j8g.xz6Zq9xZLntUonBRxvcPnr3LZcTGwvfQX8ipuXdn6FI&dib_tag=se&keywords=10k+resistor&qid=1749847715&sprefix=10k+resistor%2Caps%2C156&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1

[4]: https://www.amazon.com/EDGELEC-Resistor-Tolerance-Multiple-Resistance/dp/B07QDXVNKB/ref=sr_1_1_sspa?adgrpid=1335907190004933&dib=eyJ2IjoiMSJ9.AEIvbSpSTKjw2d_Xki0BqM2SICP4fnA3l8w8e6nxSU7hobRuvXszTi7k8K7JMIoO9bnjNX0lF72c91WkX154QY6ih6YURgyWoSA1fo1nC2YqRmgqiifn9DOvVKdweN4CUshzJJOQU-tSch0CJ5yCltXq58r8oV1vWC1A26J9L1q6ArJNtF7N8nzxXaDi31I9wpOhU7Qdp1wyMQ3QVkd1g5PJurRrfeDO13_7uBuGtQQ.jpwVLhLxL2reMRVw02RJy3JefsRsav8FOH1tJC_fhZo&dib_tag=se&hvadid=83494586628907&hvbmt=bp&hvdev=c&hvlocphy=100589&hvnetw=o&hvqmt=p&hvtargid=kwd-83494538114778%3Aloc-190&hydadcr=24663_13770022&keywords=15k+resistor&mcid=55195f8f3548323f9e6ed45eee7913ad&msclkid=89373a78c36c1c4211a952344e22bf25&qid=1749847514&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1

[5]: https://www.amazon.com/Joystick-Module-Arduino-ESP8266-Raspberry/dp/B0DQ37P5RQ/ref=sr_1_3?crid=2JUAF2KO6R53F&dib=eyJ2IjoiMSJ9.mxYyaN9Eh3MHAteUAIIoRFTMdexnkqWQuV6fEshbfpGqWAGYqJbU2R5gAKJavs47GBinRmE4sAOT-agQFbL-HPhIDq9QNPEHfzQ_4Bby4tnaMMFp_kOsCFXrSj2-84zUFDZPk9qvLgpj_AfHyzxO7SddFhhruTHbK6HLsV39vjA1VkxS4To5EECYtGaJ-L5t-MGLIdCwrD76rltU1GjdBNZUZkxbNQeKppsOQ4qs5EA.o--HpLHc9wBcPfOxlwW_qdXJQXqmDO_XSrM6u_-KG04&dib_tag=se&keywords=esp32+joystick+module&qid=1749869520&sprefix=esp32+joystick+module%2Caps%2C124&sr=8-3

[6]: https://www.amazon.com/RELAND-SUN-MPX5010DP-MPX5010-Sensor/dp/B0BMQTGB11/ref=sr_1_1?crid=1UG25SS9GOS1P&dib=eyJ2IjoiMSJ9.7BbDlWZbiNaPCjKpBCDUTwiTJPUJjONDXSM3DyfRaeSxZVkHaFVSJWDP8XtTk4B53pLDLXG54JXKSr2283jYM7SDOfhc0BX4YLTUhx8m-Pu_iTmmowqDtETrcjzsIwOMQdsoxBaIuLxf1G-xAHUnnVuYKmAbfDA4X7zqF_6X8a-9rd5UNS65Fy5cTI2RtBKunYLWG1Yn0pGeFOgAEUEJV9gC_-b-qnkoGByzqzYS81I.A-ozTt2B-eQT_L2nkFAdmplhc2VljjvNh-b-vMEzqEE&dib_tag=se&keywords=MPX5010dp&qid=1749869864&sprefix=mpx5010dp%2Caps%2C144&sr=8-1

[7]: https://www.amazon.com/EPLZON-Linear-Potentiometer-XH2-54-3-Connector/dp/B0D2991CBF/ref=sr_1_1_sspa?crid=3FI8H5RTE846F&dib=eyJ2IjoiMSJ9.siZn0QONktRoKUQM-07MANws1n3l3lKv7NhcBagnbdD4KAQCJni_opNyuMxpLgry7JkFN66bzZM5edvg35IVK1uXlCtUE_sxa9gT84ht3QY5DQRCo-auJUKO1E4hkznKS-KJMwNQ8LSZiYx13RlqsZhvaF8u7Zwm5U_DOlCDmfN8sVotI4ZWwTNpG3HD4VjnHSkTH5UFcrs1te8kLVnsnBA0ix9zyTdWG3eApKTKMqg.kZNohqIJSwTyFz0b8lyrSe3VIhlQrLglQ2E_4V4Cjy0&dib_tag=se&keywords=rotary%2Bpotentiometer&qid=1749870028&sprefix=rotary%2Bpoten%2Caps%2C137&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&th=1

[8]: https://www.amazon.com/WSDMAVIS-0-25A-Motor-Vacuum-Self-Priming/dp/B08PFLBX82?source=ps-sl-shoppingads-lpcontext&ref_=fplfs&psc=1&smid=A2S7F3O8N742PI

[9]: https://www.amazon.com/ALLECIN-IRLZ44N-Transistors-IRLZ44NPBF-Mosfets/dp/B0CBKH4XGL?source=ps-sl-shoppingads-lpcontext&ref_=fplfs&smid=A3FX7C4A9P37IQ&th=1

[10]: https://www.amazon.com/Maxmoral-Converter-Adjustable-Step-Down-Regulator/dp/B07MKQXNWG/ref=sr_1_8?crid=2O8Z0895WHHXP&dib=eyJ2IjoiMSJ9.Mn8RJg1NDqimTO87a_6HSzNLUgHkiPZ-VHvhyk_sEEX959uBLCpsnr1INDbSWpDKHzBUeE-YTWH4jJPt8yN_66qJGG0LMLLCtm2hUaRa0Z7giRo_BbYZ43KzK8a0ZkmFgm1_7exLw-T5nEHN_xsRLts72GEUkHbbKW-mQhQgRSSwipKjFBaNXV0R6c9MMWTGuIFDt9opV19PQrMrfOmi0de15Fu1fMhiF38ZXDObWWI.BWYEDRp61inwdzt085fWsmer4meqyVfUpvTSosCXECM&dib_tag=se&keywords=buck+converter+7.4v+to+5v&qid=1749758952&sprefix=buck+converter+7.4%2Caps%2C125&sr=8-8

[11]: https://www.amazon.com/LORESO-1N5819-Diode-Pack-100/dp/B07SPG9XFC?

[12]: https://www.pcbway.com/orderonline.aspx

[13]: https://www.amazon.com/Female-Header-ESP-WROOM-32-ESP32-DevKitC-Breakout/dp/B0CFDYMRK2

[14]: https://www.amazon.com/ZHOFONET-Connector-Adapter-Silicone-Battery/dp/B0DCFQPNFS/ref=sr_1_1_sspa?crid=3M1VK0ORPYIYJ&dib=eyJ2IjoiMSJ9.oGT8dUcPEt_-3myYRWftZ3DoP_hxXUXtj8rHjO44IODWJaF8r3qpxb3_LdaLFcHkSAlY6sRuTfXAtT4CJWVglcS8zBZgHf2INMpEWXcBu1VdJbOZB9d2uzsxLUbZVMqoylkjEW456rAB0vO9DUD9paL6Vtii-Hc4eGX3QDFHX630xEYBLuF5YEvH23S-KkzFMohth_k1CdhRRfUMT6oUCA_inkOBmX093ZZYt9TzGMmac0ODrnMp3IhxnIiXGf0n1mp5ki927JosQLKR-94rneH-L2sNrhOhUfYmVXyiR58.MXuIkj12e3vScXQKY50ODYn6wxcbGM-8giCcSQOPTRw&dib_tag=se&keywords=lipo%2Bbattery%2Bconnector&qid=1749872934&s=electronics&sprefix=lipo%2Bbattery%2Bconnector%2Celectronics%2C104&sr=1-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&th=1

[15]: https://www.amazon.com/Laboratory-Suction-Pipette-Dropper-Pipettes/dp/B07P86QNB8
[16]: https://www.amazon.com/uxcell-Silicone-Tubing-Rubber-Transfer/dp/B085N17TDB
[17]: https://www.amazon.com/uxcell-30Pcs-Plastic-Joiner-Connector/dp/B06XFDTLG2


## How it Works
1) Joystick module and knob send movement data wirelessly through ESP32 to the robot arm,
2) When you squeeze the air bulb the pressure sensor reads the force,
3) ESP32 sends that signal to the robot (like our [Remote Assist Hand](https://github.com/xsollwa/remote-assist-hand), but the controller can potentially be used on other robots as well),
4) If the robot grips something, it sends resistance signals back,
5) Air pump activates and sends pressure into the bulb,
6) You feel it!

## Pictures 
**PCB**
![Screenshot 2025-06-13 144101](https://github.com/user-attachments/assets/dcc8c826-610b-46b6-85fa-66b040815ea5)
![Screenshot 2025-06-13 144023](https://github.com/user-attachments/assets/46b11eda-23d2-4e46-9c2e-fdec899698c1)
![Screenshot 2025-06-13 162339](https://github.com/user-attachments/assets/94c4793a-4583-4b61-a52c-b57f91ec8223)

**3D model**



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




