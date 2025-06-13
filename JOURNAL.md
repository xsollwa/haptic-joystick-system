---
Title: "Haptic Joystick System"
Authors: "Ruzanna Gaboyan and Philip Golczak"
Description: "Haptic Joystick System is a controller that uses air pressure to send haptic feedback. It has an air bulb that you squeeze, a tube connected to pressure sensor and joystick-like controls for movement in the X Y and R (rotation) directions."
Created_At: "2025-06-07"
---

### June 6, 2025 - 2 hours

**What we did:**

- Today we brainstormed ideas for a project that would allow us to incorporate a two-way haptic feedback system and potentially wirelessly interact with robotic builds that have a functional haptic feedback system.
  
- We decided to build a wireless haptic controller that can interact with a robotic system through pressure-based feedback and multi-axis (XYR) motion input.
  
- The controller works by letting you control a robot or device using your hand. When you squeeze the air bulb, it sends air to a pressure sensor, which tells the microcontroller how hard you’re squeezing. That information is sent wirelessly to either a robot hand or a balloon that gets bigger or smaller depending on your squeeze. The joystick lets you move the robot left, right, forward, and back, and the knob lets you rotate it.
  
- We also will use a small air pump connected to the bulb to let you feel what the robot is feeling. E.g., if the robot grips something hard, the system pushes air back into your bulb, making it harder to squeeze. This creates two-way feedback because you're not just sending commands buy feeling what’s happening on the other end.
  
### June 7, 2025 - 2 hours

**What we did:**

*Ruzanna*

- Today I created the GitHub repository for this project with its necessary files, and submitted a pull request to Highway project gallery.
  
- Philip and I had a discussion about all materials we would need for this control pad. After having a good idea of how the controller will function I attempted to create the first detailed design sketch of it. This initial sketch would serve as a visual aid to help us conceptualize how the various components will fit and interact within the final assembly. Here is the first sketch with the labeled components!

![unnamed](https://github.com/user-attachments/assets/40ea8b7f-d950-4abc-8050-621e3feae92d)

After showing the sketch to Philip, we figured that although this control pad would be functional, it would be quite uncomfortable to use. Instead of this design, we decided to incorporate a design that would be fitted to the hand (like an Xbox controller), where the index finger and the thumb of the right hand would pinch the small air bulb, and the left hand would be used for XYR controls. (Also I was missing an air pump in my sketch, which needs to be used in the two-way feedback system)


### June 10, 2025 - 3 hours

**What we did:**

*Ruzanna*

- Following the aforementioned feedback I proceeded to create the 3d design of the controller using OnShape. The holes for the electronics are made considering the sizes of the parts that we found online and planned to obtain.
  Here are some screenshots: (This is my second ever 3d design!)

![Screenshot 2025-06-10 212356](https://github.com/user-attachments/assets/f41a8b4f-3ec3-4baf-9466-5352c035c6ab)

![Screenshot 2025-06-10 212418](https://github.com/user-attachments/assets/f4fa1d7f-e369-4021-b05e-f19a5888ffa3)

![Screenshot 2025-06-10 212432](https://github.com/user-attachments/assets/d88df2e6-3c5f-4a72-8750-7fc613d34f4f)

- I sent the 3d design over to Philip for review, revisions and fixing if needed.

### June 11, 2025 - 4 hours

*Ruzanna*

- Today I created the first schematic for the controller pcb. It was my second time using KiCad after solder so it took A WHILE... but I finally got all the connections in place. I used an ESP32 module as the main microcontroller and wired up a joystick, pressure sensor, potentiometer, and MOSFET-controlled air pump. Getting the voltage regulator set up correctly for the ESP32 was a bit confusing, and figuring out how to wire the MOSFET without frying anything took a few tries. I also added a flyback diode for the air pump to prevent voltage spikes. Laying out all the component symbols and making sure each one had the right number of pins connected was tricky, especially with the ESP32. But by the end, I had a complete schematic I felt pretty good about. Here is a picture of it:

![Screenshot 2025-06-12 125250](https://github.com/user-attachments/assets/6372711e-21f1-404e-a737-27b28564a4bc)

- I sent the schematic to Philip for a double check and some feedback, and it turned out that I was using the wrong ESP32 model and batteries the whole time (I had selected the ESP32-WROOM-32 instead of the correct ESP-WROOM-32, and I was designing around a 4.7V battery instead of the required 7.4V). Philip also also recommended replacing the step-up converter with a buck converter to provide a more stable 5V output for both the microcontroller and the sensor. After reviewing these suggestions, I went back and revised the design. Following the corrections, I created the second (and correct!) version of the schematic:

![Screenshot 2025-06-13 144101](https://github.com/user-attachments/assets/5bfd7266-f5f5-4579-9cd3-cca158d246d0)

### June 12, 2025 - 2 hours

*Ruzanna*

- Today I completed the final PCB layout for the controller. One of the biggest challenges I faced was routing all the connections — with so many components in a compact space, it quickly became difficult to find clean paths for every signal. Eventually, I learnt about vias and used them to complete the pcb!

![Screenshot 2025-06-13 144023](https://github.com/user-attachments/assets/df339fd2-4320-41bc-b257-b6e95aef9cd5)
![Screenshot 2025-06-13 162339](https://github.com/user-attachments/assets/5d22a96b-e25a-418c-88ed-5debb6bfae24) 
//nothing super interesting on the back of it

- We also familiarised ourselves with the process of ordering pcbs through PCBWay, since it would be our first time using it.



