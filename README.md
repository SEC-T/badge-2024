# SEC-T 0x10sion Music Synthesizer Badge kit
![photo of SEC-T 0x10sion Music Synthesizer Badge](https://github.com/user-attachments/assets/5560923a-0038-4800-beb4-cfb76b8eb46d)

The SEC-T 0x10sion Music Synthesizer Badge is a kit that attendees of SEC-T 0x10sion 2024 must solder together.
It is an Arduino-compatible music synthesizer kit with:
* a touch keyboard
* built-in speaker/amp
* audio output jack (for high-quality audio)
* 5 programmable RGB LEDs

It is designed to be perfect for total beginners to learn to solder and complete successfully.

The SEC-T Synth Badge comes with a pre-programmed music synthesizer that makes way cool sounds and music and noise.
Once soldering is completed, the SEC-T Synth Badge is a really nice, portable, performing musical instrument!

The SEC-T Synth Badge kit is based on the ArduTouch music synthesizer kit by Mitch Altman / Cornfield Electronics,
which is fully Open Hardware, with a CC BY-SA 4.0 license.

The SEC-T Synth Badge makes use of a modified ArduTouch library, which is available for
programming in more super nice synthesizer features, and for making your own synthesizers.
The library comes with examples (with more to come) that serve as a nice tutorial
on how to create your own SEC-T Synth Badge synthesizers (which are Arduino sketches).

The SEC-T Synth Badge is really just an Arduino Uno clone
with the features listed above, a couple of extra buttons, and a four of pots (for playing music and controlling the sounds).
The ArduTouch Arduino Library is where the real magic comes from!

For those who want to learn more, the documentation teaches the basics of Digital Signal Processing for audio generation.

The SEC-T Synth Badge kit is fully Open Hardware, with a CC BY-SA 4.0 license.

![Open Hardware logo](https://github.com/user-attachments/assets/ffe5899f-a1be-437b-99b8-e0a93b4a2112)
  
  
  
---
## Assembly Instructions (with re-programming how-to)

_Click on the image below to open a PDF file with detailed instructions on how to assemble your SEC-T 0x10sion Music Synthesizer Badge (which also includes a how-to for re-programming your Synth Badge with a new Arduino sketch):_  
  
### Part 1 of 2 parts:
<a href="https://github.com/user-attachments/files/16896373/SEC-T_0x10sion_badge_assembly_part_1of2.pdf"><img src="https://github.com/user-attachments/assets/f0d42357-683e-4664-b6a4-c0766932c63d" align="center" width="400"></a>  
  
### Part 2 of 2 parts:
<a href="https://github.com/user-attachments/files/16896082/SEC-T_0x10sion_badge_assembly_part_2of2.pdf"><img src="https://github.com/user-attachments/assets/e167f560-7fb3-4250-949e-37c5ca6ce853" align="center" width="400"></a>  
  
  
---
## "Quadrant" Synth Demo Videos

The Synth Badge comes pre-programmed with a synth called __Quadrant__.  
Quadrant is somewhat like a looper.  Every time you play a note it goes into the loop.  You can control the speed of the loop, and you can also make the sound warp (detune) every time through the loop.  
  
There is the default patch for Quadrant, plus 4 presets -- all 5 of these have very different sounds!  
In addition to changing the loop speed and changing the warp-factor, there are many parameters that you can play with, some of which change the sounds in subtle ways, others which change the sounds a lot.  

These demo videos give you a very small taste of the various sounds Quadrant can make. 

___NOTE__:  The demo videos use the prototype version of the SEC-T 0x10sion Music Synthesizer Badge kit._  
There are some differences between the prototype version and the badges at the event.  In particular, the two red buttons are reversed (left and right).  

__Basic Functionality of Quadrant__:  
* At power-up (or after Reset), Quadrant is in the Default Patch
* Single-Click the right red button to go up an octave
* Single-Click the left red button to go down an octave
* Double-Click the right red button and then hit one of the first four lower touch-keys to choose a preset
  * The left-most touch-key is "C" -- Scaffold
  * The next touch-key is "D" -- Farsy
  * "E" -- Teleprompt
  * "F" -- Glacial
  * "G" -- Blur
* Tap the Reset button to get the Default Patch
* The Top Pot controls the Warp-factor
* The Bottom Pot controls the speed of the loop
* Pot A controls tremelo speed
* Pot B controls tremelo depth
* Press-and-hold on the left and right red buttons will make the Red and Blue LEDs light or blink, indicating that the four Pots will now control different parameters.  
  
See the next section, "Quadrant Synth User Guide", for a complete description of all that the Quadrant Synth can do.  
  
### Default Patch (at power-up and after Reset) _-- click on the image below to play the video_
[![Thumb for Default Patch demo](https://img.youtube.com/vi/8XAPtq4mnFQ/0.jpg)](https://www.youtube.com/watch?v=8XAPtq4mnFQ)  

### Preset "C" -- Scaffold _-- click on the image below to play the video_
[![Thumb for Preset "C" demo](https://img.youtube.com/vi/vg959F6WOIo/0.jpg)](https://www.youtube.com/watch?v=vg959F6WOIo)  

### Preset "D" -- Farsy _-- click on the image below to play the video_
[![Thumb for Preset "D" demo](https://img.youtube.com/vi/kQ33xrg7Zvw/0.jpg)](https://www.youtube.com/watch?v=kQ33xrg7Zvw)  

### Preset "E" -- Teleprompt _-- click on the image below to play the video_
[![Thumb for Preset "E" demo](https://img.youtube.com/vi/aHm4yFb8vfQ/0.jpg)](https://www.youtube.com/watch?v=aHm4yFb8vfQ)  

### Preset "F" -- Glacial _-- click on the image below to play the video_
[![Thumb for Preset "F" demo](https://img.youtube.com/vi/Ehx3G3c8V5c/0.jpg)](https://www.youtube.com/watch?v=Ehx3G3c8V5c)  

### Preset "G" -- Blur _-- click on the image below to play the video_
[![Thumb for Preset "G" demo](https://img.youtube.com/vi/jeHlcZ8XYc8/0.jpg)](https://www.youtube.com/watch?v=jeHlcZ8XYc8)  
  
  
---
## Quadrant Synth User Guide

The Synth Badge comes pre-programmed with a synth called __Quadrant__.  Quadrant is a very powerful performing musical instrument.  
_Click on the image below to learn how to play with it:_  
<a href="https://github.com/user-attachments/files/16901048/Quadrant_Synth_User_Guide.pdf"><img src="https://github.com/user-attachments/assets/9ba05948-aa36-4cce-a8c9-c9a45c3a8a30" align="center" width="400"></a>  
  
  
---
## Re-programming the Synth Badge

The Synth Badge can be re-programmed using the free Arduino software (on Windows, Linux, and MacOS).  
The end pages of the [Synth Assembly Instructions](https://github.com/user-attachments/files/16896082/SEC-T_0x10sion_badge_assembly_part_2of2.pdf) explain how to do this.  
To re-program your Synth Badge you will need to use a USB-Serial TTL adapter.  
  
The Synth Badge comes pre-programmed with the __Quadrant__ synth.  
  
There are 8 other synths that were developed for the open hardware_ArduTouch_ project (which the Synth Badge is based on), all of which will work on the SEC-T Synth Badge.  But you should be aware of the following:  
* The Synth Badge has 5 programmable RGB LEDs that the _ArduTouch_ boards do not have.  
* The __Quadrant__ synth sketch (from the Synth Badge's Github) has code for controlling the RGB LEDs, but the _ArduTouch synth sketches do not -- though this functionality can be added by you or others.  
* The Synth Badge does not have touch-keys for "A, A#, or B).
  
All of the documentation for [ArduTouch](https://cornfieldelectronics.com/cfe/projects.php#ardutouch), including the synth sketches, is available on the Cornfield Electronics website:  
https://cornfieldelectronics.com/cfe/projects.php#ardutouch  
  
Here is a list of ArduTouch synths that you can program into your Synth Badge:
* __Thick__: 4-voice sawtooth synth
* __Arpology__: Arpegiating synth
* __Beatitude__: drum sequencer with bass synth
* __Dronetic__: drone-making synth
* __Hocus__: Hammond organ (with Leslie) emulator synth
* __Mantra__: Indian-like percusion/bass/drone/melody synth
* __Xoid__: Digital-noise synth
* __ZapShot__: 8-bit video-game-sound synth
  
  
---
## A Note on the (Horrible) User Interface for the Quadrant Synth

As noted in the [Quadrant Synth User Guide](https://github.com/user-attachments/files/16901048/Quadrant_Synth_User_Guide.pdf) (see the "Quadrant Synth User Guide", above), the User Interface is really annoying.  

The following is quoted from the Quadrant Synth User Guide:  
  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;_You will be annoyed at the User Interface for this synth.  Sorry about that.  The reason that it is so annoying  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;is that there are so few Input/Output pins available on the microcontroller.  There were only enough I/O pins  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;for 2 LED indicators, 2 Buttons, and 4 Pots._  
  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;_I had to come up with a way to control all of the functionality of the Quadrant synth with only the above inputs  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;and outputs.  The result is not ideal.  Yet, with some practice, you will find that your Quadrant synth is quite  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;usable as a really nice performing musical instrument._  
  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;_FYI:  I chose this microcontroller to be the same as the one used in an Arduino Uno:  ATmega328.  This was so  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;that the project would be easy to use for beginners, easy to learn for beginners, easy to hack, and also as  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;inexpensive as possible._  

  
---
## Schematic Diagram

_Click on the thumbnail below to to see the PDF file:_  
<a href="https://github.com/user-attachments/files/16918384/SEC-T_0x10sion_Badge_schematic.pdf"><img src="https://github.com/user-attachments/assets/cf107937-fef4-4f8a-adfe-5f01dac5e2ce" align="center" width="400"></a>  
  
  
  
