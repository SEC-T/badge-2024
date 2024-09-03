//
//  Quadrant.ino
//
//  A partly echoing, partly arpeggiating, possibly warped synth.
//
//  To get the most out of Quadrant be sure to read these header comments 
//  first. 
//
//  Target:   SEC-T 0x10sion Music Synthesizer Badge kit v0.2
//
//  This file is modified from the Quadrant.ino synth, which was created for the ArduTouch music synthesizer kit.
// 
//  ---------------------------------------------------------------------------
//
//           ==== Audio/Control Flow Diagram for the Quadrant Synth ====
//
//
//                ------------  
//                |  Tropes  | --------.
//                ------------         |
//                                     |    
//                                     v    
//                              ----------------
//               .----------->  |   Echotron   |
//               |              ----------------
//               |                  |     |
//               |------------------|-----|------------------. 
//               |                  |     |                  |
//               |                  v     v                  v
//          ------------        ---------------        ------------
//          |  Lead 0  |  --->  |    Mixer    |  <---  |  Lead 1  |
//          ------------        ---------------        ------------
//                                  |     |
//                                  |     |
//                                  v     v
//
//                               [ Audio out ]
//
//  
//  Lead 0 is duplicated in the opposite audio channel by Lead 1 (which may be
//  transposed and/or detuned from Lead 0). The note information for Lead 0 is 
//  also fed into the Echotron. At the same time, a "trope" (an arpeggiation 
//  pattern) can be applied to the echoed note. The output from the Echotron is 
//  internally panned in stereo and then fed into a mixer along with the audio 
//  output from Lead 0 and Lead 1.
//
//  ---------------------------------------------------------------------------
//
//                         ====  The Echotron Unit ====
//
//  The Echotron is a stereo synthesizer in its own right. 
//
//  At the heart of the Echotron is a carousel of "slots", each of which holds
//  an instance of an echoing note:
//
//       ---------------------------------------
//       |                                     |  <--- WARP FACTOR
//       |            Note Carousel            |  
//       |                                     |  <--- DELAY TIME
//       |  Slot 0 < Slot 1 < Slot 2 < Slot 3  |  
//       |                                     |  <--- FEEDBACK
//       ---------------------------------------
//             |
//             |
//             v
//          note out
//
//  The carousel is constantly "revolving", outputting the note held in the
//  current slot (the left-most slot in the above diagram), then shifting 
//  the slots. 
//
//  The speed at which the carousel revolves is controlled by the DELAY TIME. 
//
//  Each time a note echoes its volume will decrease by an amount controlled
//  by the FEEDBACK setting. If FEEDBACk is set to its highest value (255) 
//  then the note will echo forever, or until all the slots in the carousel
//  have been used up, and a new incoming note "overwrites" it. 
//
//  Each time a note echoes its pitch can be raised or lowered by the 
//  current WARP FACTOR.
//
//  The number of slots in the carousel is configurable and can be set 
//  interactively via the Parameter Menu (see below) to between 1 and 4.
//
//                            ----- Tropes ------
//
//  The above description of the note carousel is a simplification. In fact,
//  each slot can contain not just a note, but also a pointer to a "trope" (an
//  arpeggiation pattern) in ROM. Each time the carousel revolves it will play
//  the next note in the arpeggiation pattern for that slot.
//
//                           ----- Note HOLD ------
//
//  You can instruct the Echotron to HOLD the next note played (see the
//  Parameter Menu below). When a note is on HOLD it will not decrease in 
//  volume as it echoes, and it will not be overwritten by incoming notes 
//  when the carousel is full. This is a very useful feature for setting
//  up a constant bass pulse and jamming over it. Better yet, if a trope is 
//  active when you hold the note, then you can set up a whole bass line or 
//  texture to be played over. And you can hold more than one note :)
//
//                         ----- SAMPLE & HOLD ------
//
//  You can globally freeze whatever sound loop the Echotron is producing by 
//  enabling SAMPLE & HOLD (toggled via the Parameter Menu, see below). Then 
//  you can jam over it freely.
//
//                         -----   ISO-TEMPO  -------
//
//  This feature allows the Echotron to maintain a constant tempo, regardless
//  of the number of notes in the carousel. So if there are 4 notes currently
//  echoing in the carousel and ISO-TEMPO is on, then the delay time will be
//  1/4 of what it would be if there were only 1 note in the carousel. The 
//  "Telepromt" and "Frara Jaqua" presets use ISO-TEMPO. You can toggle
//  ISO_TEMPO via the Parameter Menu (see below).
//
//                          -----   ENVELOPE  ------
//
//  A master envelope dynamically controls the amplitude of the notes produced by
//  the Echotron. This envelope is a traditional ADSR (Attack/Decay/Sustain/Release)
//  but with a twist: a sustain time parameter is added. When the envelope for an 
//  echoing note reaches the sustain stage it will automatically release after the 
//  amount of time specified by the sustain time parameter. The various stages of
//  the master envelope can be controlled via the pots (see below).
//
//                        -----   PULSE WIDTH  ------
//
//  The notes produced by the Echotron use square wave oscillators. The PULSE WIDTH
//  of these oscillators can be modified via a pot (see below). The pulse width 
//  controls the ratio between the positive and negative edges of the square wave 
//  and can radically alter the timbre of the tones.
//
//                          -----  AUTO-WAH  ------
//
//  The Echotron has a pair of internal autowahs (oscillating low-pass filters)
//  which are applied to the note output before it is panned. Three parameters
//  control the autowah behaviour: CUTOFF frequency, oscillation FREQ, and 
//  oscillation DEPTH. All of these can be modified via the pots (see below).
//
//                        -----  PAN CONTROL  ------
//
//  A PAN control dynamically rotates the note output in the stereo field. The
//  DEPTH and FREQ of the panning can be controlled via the pots (see below).
//
//  ---------------------------------------------------------------------------
//
//                     Overview Diagram of the Echotron
//
//  Now that we have covered each component, here is an overall view:
//
//
//                                     |
//                     [HOLD]    notes | in        [SAMPLE & HOLD]
//                           \         |             
//                            \        v
//                             -----------------   < WARP >
//             Trope in --->   | Note Carousel |   < DELAY >      [ISO-TEMPO]
//                             -----------------   < FEEDBACK >
//     -------------                    |          < # SLOTS >
//     | ENVELOPE  |              notes | out
//     | <Attack>  |                    |
//     | <Decay>   |                    v
//     | <Sustain> |     -----------------------------
//     | <SusTime> |     |          AutoWahs         |
//     | <Release> |     | <CUTOFF>  <FREQ>  <DEPTH> |
//     -------------     -----------------------------
//                             |               |
//                             v               v
//                       -----------------------------
//                       |  <---- Pan Control ---->  |  
//                       |      <FREQ>  <DEPTH>      |
//                       -----------------------------
//                           |     audio out     |
//                           v                   v
//
//
//  ---------------------------------------------------------------------------
//
//                            PLAYING THE KEYBOARD
//
//  ---------------------------------------------------------------------------
//
//  The Quadrant keyboard is a mono-touch instrument: you can play one note at 
//  a time.
//
//  To raise the keyboard by an octave, tap the right button once.
//  To lower the keyboard by an octave, tap the left button once.
//
//  ---------------------------------------------------------------------------
//
//                     USING THE POTS TO CONTROL PARAMETERS
//
//  ---------------------------------------------------------------------------
//
//  The pots can be used to control 18 different parameters.
//
//  The ArduTouch board has two pots.
//     You can choose two parameters to control with the left and right pots.
//  The SEC-T Synth Badge board has four pots.
//     You can choose four paramaters to control with the left and right pots.
//
//  By pressing (not tapping) the left and right buttons you can change which
//     parameters the pots control. Each time a button is pressed, its
//     associated LED (red LED for left button, blue LED for right button) will
//     change state (from OFF to ON to BLINKING, back to OFF etc.)
//
//              For the ArduTouch board:
//              -----------------------
//  Below is a chart showing which two parameters the two pots control when
//  the LEDs are in a given state:
//
//     red LED    blue LED         top POT                  bot POT
//     -------    --------   -------------------      -------------------
//       OFF         OFF          echo WARP             echo DELAY time
//        ON         OFF        echo FEEDBACK          lead/echo BALANCE
//     BLINK         OFF        panning FREQ             panning DEPTH
//       OFF          ON        autowah FREQ             autowah DEPTH
//        ON          ON        autowah CUTOFF          echo PULSE WIDTH
//     BLINK          ON         lead 1 DETUNE**         lead 1 TRANSPOSE***
//       OFF       BLINK       envelope ATTACK*          envelope DECAY*
//        ON       BLINK       envelope SUSTAIN*        envelope RELEASE*
//     BLINK       BLINK         echo SUSTIME            lead PORTAMENTO
//
//    * these pots can be configured via the Parameter Menu (see next section)
//      to control:
//
//       1) only the LEAD voices
//       2) only the ECHO voices
//       3) both the LEAD and ECHO voices
//
//    ** detunes lead1 from lead 0
//
//    ** transposes lead1 from lead0 (range is from -2 to +2 octaves)
//
//
//              For the SEC-T Synth Badge:
//              -------------------------
//  Below is a chart showing which four parameters the four pots control when
//  the LEDs are in a given state:
//
//     red LED    blue LED         Top POT                Bottom POT              Adjust A POT             Adjust B POT
//     -------    --------   -------------------      -------------------      -------------------      -------------------
//       OFF         OFF          echo WARP             echo DELAY time           autowah FREQ             autowah DEPTH
//        ON         OFF        echo FEEDBACK          lead/echo BALANCE          autowah CUTOFF          echo PULSE WIDTH
//     BLINK         OFF        panning FREQ             panning DEPTH             lead 1 DETUNE**         lead 1 TRANSPOSE***
//       OFF          ON        autowah FREQ             autowah DEPTH           envelope ATTACK*          envelope DECAY* 
//        ON          ON        autowah CUTOFF          echo PULSE WIDTH         envelope SUSTAIN*        envelope RELEASE*
//     BLINK          ON         lead 1 DETUNE**         lead 1 TRANSPOSE***       echo SUSTIME            lead PORTAMENTO
//       OFF       BLINK       envelope ATTACK*          envelope DECAY*                --                       --
//        ON       BLINK       envelope SUSTAIN*        envelope RELEASE*               --                       --
//     BLINK       BLINK         echo SUSTIME            lead PORTAMENTO            (weirdness)                  --
//
//    * these pots can be configured via the Parameter Menu (see next section)
//      to control:
//
//       1) only the LEAD voices
//       2) only the ECHO voices
//       3) both the LEAD and ECHO voices
//
//    ** detunes lead1 from lead 0
//
//    ** transposes lead1 from lead0 (range is from -2 to +2 octaves)
//
//  ---------------------------------------------------------------------------
//
//                           USING THE PARAMETER MENU
//
//  ---------------------------------------------------------------------------
//
//  You can control additional parameters by using the Parameter Menu.
//
//  To access the Parameter Menu double-tap the left button. The LEDs will now
//  start blinking on and off in alternation: this tells you that the Parameter 
//  Menu is active. When the Parammeter Menu is active the keys will no longer 
//  play notes but each key will select and modify a particular parameter as 
//  described below. Depending on the parameter, the pot positions may also be
//  relevant, as described below. The Parameter Menu is active until you press 
//  a key: then the parameter associated with that key is modified, and the 
//  Parameter Menu is automatically exited (restoring the prior user interface 
//  state).
//
//  You can exit the Parameter Menu without modifying any parameters by double-
//  tapping the left button a second time.
//  
//  The list below describes what actions are taken when a key is pressed:
//
//      Key   Action   
//      ---   -----------------------------------------------------------
//
//       C    toggle Sample-and-Hold on/off
//       C#   turn Sample-and-Hold off, and unhold all held notes
//       D    hold next note played
//       D#   [ reserved ]
//       E    toggle lead voices on/off 
//       F    set # of slots in carousel based on pot 0 position *
//       F#   toggle isoTempo or/off 
//       G    select trope based on pot 1 position **
//       G#   [ reserved ] 
//       A    ADSR pots control envelopes of both lead and echo voices
//       A#   ADSR pots control envelopes of lead voices only 
//       B    ADSR pots control envelopes of echo voices only
//
//   * turn pot 0 to desired position (all the way left = 1, all the way right = 4) 
//     then press F 
//
//  ** turn pot 1 to desired position (all the way left = "no trope" ) then press G 
//
//  ---------------------------------------------------------------------------
//                         Guide to Quadrant's Presets 
//  ---------------------------------------------------------------------------
//
//   Load presets by double-tapping the right button and then pressing any 
//   "white" key. 
//
//   The presets are as follows:
//
//      Key   Name        Performance Notes   
//      ---   ----        ----------------------------------------------------
//       C    Scaffold    lead voices off (and should stay off)
//       D    Farsy       
//       E    Teleprompt  lead voices off (turn them on once 2 slots are playing)  
//       F    Glacial     play very slowly 
//       G    Blur      
//       A    ToneJack    ** 
//       B    FraraJaqua  lead voices off (turn them on once 4 slots are playing) ** 
//
//       ** FraraJaqua and ToneJack are only available with ArduTouch board
//
//  ---------------------------------------------------------------------------
//                           About Quadrant's Tuning 
//  ---------------------------------------------------------------------------
//
//   When you start playing notes on the keyboard you will probably notice that 
//   the 'C' key plays an 'F'. And if you have a really good ear you may also 
//   notice that the interval between C# and G# is not exactly a 5th.
//
//   Why is this?
//
//   To explain the 2nd observation first, Quadrant uses Harmonic Tuning (also
//   known as Just Temperament or "Helmholtz's scale") instead of the usual
//   Equal Temperament (which is the system default). 
//
//   Harmonic Tuning is actually more "natural" than Equal Temperament (which
//   was created as a compromise so that keyboard instruments could play in
//   all keys). In Harmonic Tuning the relative frequencies of all notes 
//   compared to the tonic ('C' in our case) are rational numbers:
//
//              Unison         1/1  
//              Minor Second  25/24  
//              Major Second   9/8  
//              Minor Third    6/5  
//              Major Third    5/4   
//              Fourth         4/3  
//              Diminish 5th  45/32  
//              Fifth          3/2  
//              Minor Sixth    8/5  
//              Major Sixth    5/3  
//              Minor Seventh  9/5  
//              Major Seventh 15/8  
//              Octave         2/1  
//
//   The Echotron uses quantum square wave oscillators which can only be set to 
//   frequencies which have integral wavelengths. The frequencies produced by 
//   these oscillators conform more closely to a scale in Harmonic Tuning than 
//   one in Equal Temperament.
//
//   But why does the 'C' key play an 'F'?
//
//   The short answer is that on the ArduTouch, whose audio rate is ~15.6 kHz,
//   'F' (in octave 1) has a period of 360 samples. 360 is a number favored by the
//   ancients because it is divisible by a large number of integers: 1, 2, 3, 4, 
//   5, 6, 8, 9, 10, 12, 15, 18, 20, 24, etc. It so happens that by using a tonic
//   frequency whose period is 360, we are able to use quantum oscillators to
//   closely approximate the Harmonic Tuning scale over several octaves. Low 'F', 
//   so to speak, is the natural tonic frequency of the ArduTouch.*
//
//   Since Quadrant's tuning is only an approximation to an 'F' Harmonic tuning, 
//   certain musical scales (such as E Major, F# Major on the ArduTouch keyboard) 
//   will sound a bit sour. C Major / D Minor work best.
//   
//   *actually, on the ArduTouch a waveform with a period of 360 samples generates 
//   a tone with a frequency of 43.578 Hz. In the equal temperament scale, 'F' in
//   octave 1 has a frequency of 43.654 Hz.  
//
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2024, Mitch Altman
//  Copyright (C) 2019, Cornfield Electronics, Inc.
// 
//  This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
//  Unported License.
// 
//  To view a copy of this license, visit
//  http://creativecommons.org/licenses/by-sa/3.0/
// 
//  Created by Bill Alessi & Mitch Altman.
//
//  Slightly modified for SEC-T 0x10sion Music Synthesizer Badge kit by Mitch Altman
// 
//  ---------------------------------------------------------------------------/*

//  ---------------------------------------------------------------------------
//
//  The firmware for controlling the RGB LEDs on the SEC-T 0x10sion Music Synthesizer Badge
//  was taken from firmware substantially written by Josh Levine.
//  The original firmware is available at:
//  https://github.com/bigjosh/SimpleNeoPixelDemo
//  Copyright (c) 2015 Josh Levine
//  It has an MIT License:
//  https://github.com/bigjosh/SimpleNeoPixelDemo/blob/master/licence.txt
//  Mitch Altman modified it slightly for use of the SEC-T 0x10sion Music Synthesizer Badge kit.
//
//  ---------------------------------------------------------------------------


#include "ArduTouch.h"                       

about_program( Quadrant, 1.74c )       

// If you've got your hacker shoes on and want to alter this sketch, uncommenting
// the following line will help you move between __FULLHOST__ and __STNDLONE__
// runtime models without generating compiler errors.

//#define DEVELOPER                       // for developers only!

// The __STNDLONE__ runtime model is necessary for the full version of Quadrant

#ifndef DEVELOPER
#ifndef __STNDLONE__
   #error This sketch requires the __STNDLONE__ runtime model (Model.h)
#endif
#endif

// AUDIT_LEAD is for developers who want to listen to the full lead voices while 
// running in the __FULLHOST__ model (otherwise, only sine waves are used, because 
// there is not enough ROM to fit the complex wavetables, nested oscillators and 
// console code at the same time). When AUDIT_LEAD is defined and the runtime model
// is __FULLHOST__ the Echotron unit is replaced by a null-audio StereoInstrument 
// (again, not enough space).

// #define AUDIT_LEAD                      // for developers only!

#ifdef __FULLHOST__
   #ifndef AUDIT_LEAD 
      #define AUDIT_ECHO
   #endif
#else
   #define AUDIT_ECHO
   #define AUDIT_LEAD
#endif

#ifdef SECT_BADGE_PROJ
/**********************************************
    * Definitions for programmable RGB LEDs
   ***********************************************/
#define NUM_PIXELS 5  // Number of pixels in the LED string
#define BRIGHTNESS 6  // Shift R, G, and B values right by this integer \
                      // BRIGHTNESS of 0 gives full brightness \
                      // BRIGHTNESS of 1 gives half brightness \
                      // BRIGHTNESS of 2 gives quarter brightness \
                      // BRIGHTNESS of 3 gives eighth brightness \
                      // BRIGHTNESS of 4 gives sixteenth of the brightness \
                      // BRIGHTNESS of 5 gives thirty-secondth of the brightness \
                      // etc
// The following values are for PD2 on the ATmega328P chip (which is Arduino Pin 2):
#define PIXEL_PORT PORTD  // Port of the pin the pixels are connected to
#define PIXEL_DDR DDRD    // Port of the pin the pixels are connected to
#define PIXEL_BIT 2       // Bit of the pin the pixels are connected to
// The following are the timing constraints taken mostly from the WS2812 datasheets,
// chosen to be conservative and avoid problems rather than for maximum throughput:
#define T1H 900  // Width of a 1-bit High time in ns
#define T1L 600  // Width of a 1-bit Low time in ns
#define T0H 400  // Width of a 0-bit High time in ns
#define T0L 900  // Width of a 0-bit Low time in ns
// Width of the low gap between bits to cause a frame to time out and latch,
// and thus display the pixel data most recently sent to the LED string
#define RES 250000  // For the SEC-T Synth Badge, we will ignore this, since there will always be enough time after sending pixel data to latch the pixel data
// Here are some convience defines for using nanoseconds specs to generate actual CPU delays
#define NS_PER_SEC (1000000000L)  // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)
#endif   // SECT_BADGE_PROJ


/******************************************************************************
 *
 *                               Presets
 *
 ******************************************************************************/

define_preset( Empty,      "" )               

define_preset( Blur,       "!0g25\\`"
                           #ifdef AUDIT_LEAD
                           "X1v120\\Or1.5\\m23\\d23\\D.75\\<``"
                           #endif
                           "b200\\"
                           #ifdef AUDIT_ECHO
                           "Ew1\\f220\\d.1\\n2\\Ma20\\t50\\``" 
                           #endif
                           )               

define_preset( Farsy,      "!Ma60\\s200\\r180\\`1x5\\`"
                           #ifdef AUDIT_ECHO
                           "Et3\\d.32\\f190\\Ma10\\r165\\t180\\`"
                           "0P31\\ac123\\lf3.2\\d89\\```"
                           "1P30\\ac130\\lf2.4\\d83\\````"
                           #endif
                           "s1\\0g36\\`1g38\\`" )               

define_preset( Glacial,    "!b50\\Ma160\\d200\\s170\\r210\\`"
                           #ifdef AUDIT_ECHO
                           "Et5\\d2.7\\f255\\Pf.4\\d64\\`"
                           "Ma130\\d200\\s0\\r0\\t0\\`"
                           "0P64\\ac160\\lf1.2\\d51\\```"
                           "1P117\\ac140\\lf.8\\d51\\```"
                           #endif
                           )

define_preset( Scaffold,   "!0.`1.`"
                           #ifdef AUDIT_ECHO
                           "Et2\\d.037\\f255\\w4\\"
                           "Pf5\\d128\\``"
                           #endif
                           "" )  
                                        
define_preset( ToneJack,   "!"
                           "!b225\\0v80\\x-15\\Ag<g40\\```"
                           #ifdef AUDIT_LEAD
                           "X1Om126\\r.377\\D.75\\<``"
                           #endif
                           #ifdef AUDIT_ECHO
                           "Ed.2\\f255\\w30\\"
                           #endif
                           "" )  
                                        
define_preset( FraraJaqua, "!0.v120\\x-36\\ed0\\r255\\`Agg40\\```"
                           "X1.ed50\\s200\\r145\\`O<r.236\\`Agg1.5\\```"
                           "b130\\"
                           #ifdef AUDIT_ECHO
                           "Ed.45\\"
                           "0P96\\ac180\\lf6\\d77\\```"
                           "1P32\\alf1.65\\d109\\```"
                           "it\\t9\\f255\\Ma13\\d56\\s70\\t25\\``" 
                           #endif
                           )               

define_preset( Teleprompt, "!0.v100\\x-12\\ea1\\d1\\s52\\r240\\`Ag<g12\\```"
                           "X1.v160\\x-12\\ea0\\d10\\s60\\r220\\`d32\\O<r2\\m116\\``"
                           "b220\\"
                           #ifdef AUDIT_ECHO
                           "Eit\\t8\\n2\\d.171\\f255\\0P96\\`"
                           "Md7\\s36\\r36\\t5\\``" 
                           #endif
                           )


begin_bank( myPresets )                   

   _preset( Scaffold )                    // C
   _preset( Empty )                       // C#
   _preset( Farsy )                       // D
   _preset( Empty )                       // D#
   _preset( Teleprompt )                  // E
   _preset( Glacial )                     // F
   _preset( Empty )                       // F#
   _preset( Blur )                        // G
   _preset( Empty )                       // G#
   _preset( ToneJack )                    // A
   _preset( Empty )                       // A#
   _preset( FraraJaqua )                  // B

end_bank()

/******************************************************************************
 *       
 *                               Tropes 
 *
 *  Tropes are preset arpeggiation patterns in ROM. 
 *
 *  Each element in a trope array specifies a semi-tone offset to be applied to 
 *  subsequent echoes of a note. 
 *
 *  The end of a trope is signified by an offset of EOT.
 *
 ******************************************************************************/

#define NUM_TROPES 10            // number of preset tropes in ROM

#define EOT        -128          // "end-of-trope"

PROGMEM const char trope00[] = { EOT };             // preset 0 always null

PROGMEM const char trope01[] = { 0, 7, 12, 7, EOT };
PROGMEM const char trope02[] = { 0, 5, 7, EOT };
PROGMEM const char trope03[] = { 0, 0, 7, 5, EOT };
PROGMEM const char trope04[] = { 0, 0, 0, 7, EOT };
PROGMEM const char trope05[] = { 0, -5, EOT };
PROGMEM const char trope06[] = { 0, -4, EOT };
PROGMEM const char trope07[] = { 0, 0, -3, -1, -5, -3, EOT };

// "Teleprompt"

PROGMEM const char trope08[] = { 9, 0, 5, 0, 12, 0, 9, 11, 0, 7, 0, 14, EOT };

// "Frara Jaqua"

PROGMEM const char trope09[] = { 0, 2, 4, 0, 0, 2, 4, 0,
                                 4, 5, 7, 7, 4, 5, 7, 7,
                                 7, 5, 4, 0, 7, 5, 4, 0,
                                 0, -5, 0, 0, 0, -5, 0, 0, EOT };

PROGMEM const char* const tropes[] =   // an array of ptrs to preset tropes
{
   &trope00[0],
   &trope01[0],
   &trope02[0],
   &trope03[0],
   &trope04[0],
   &trope05[0],
   &trope06[0],
   &trope07[0],
   &trope08[0], 
   &trope09[0]
} ;

/*----------------------------------------------------------------------------*
 *                           TweekedHarmonicTuning
 *----------------------------------------------------------------------------*/      

// This tuning compensates for a particularly out-of-tune 'A' in octave 3
// for quantum oscillators. By using this tuning, oscillators with non-quantum 
// periods (such as those used by the lead voices) will not be noticably out of 
// tune with their quantum counterparts.

class TweekedHarmonicTuning : public HarmonicTuning
{
   typedef HarmonicTuning super;          // superclass is HarmonicTuning

   public:

   TweekedHarmonicTuning()
   {
      tonic0 = 43.578;                    // low C will play an 'F'
   }

   double pitch( key k )                  // return frequency for a given key
   {
      if ( k.octave() == 3 && k.position() == 9 ) // tweek A3
         return 582.25;
      else
         return super::pitch( k );
   }

} ;

/*----------------------------------------------------------------------------*
 *                               NotePeriods
 *----------------------------------------------------------------------------*/      

// Thus is a lookup table of quantum periods for notes

const word NotePeriods[] PROGMEM = {  

   // octave 0

   360, 346, 320, 300, 288, 270, 256, 240, 225, 214, 200, 192,

   // octave 1

   180, 173, 160, 150, 144, 135, 128, 120, 113, 107, 100, 96,

   // octave 2
   
   90, 86, 80, 75, 72, 67, 64, 60, 56, 54, 50, 48,

   // octave 3
   
   45, 43, 40, 38, 36, 34, 32, 30, 28, 27, 25, 24

} ;

word notePeriod( byte noteNum )           // given a note #, return its period
{
   #define NUMPERIODS 48

   while ( noteNum >= NUMPERIODS )        // insure note falls within table
      noteNum -= 12;
      
   // calc address of period in ROM

   byte idx    = noteNum << 1;            
   word elePtr = (word )&NotePeriods[0] + idx;    

   return pgm_read_word_near( elePtr );
}

/******************************************************************************
 *
 *                              OscStack 
 *
 ******************************************************************************/

#define NoNote 255                     // note # for no note

#define NUMOSC 2                       // # of oscillators in OscStack 

#define MAXAMP 63                      // max oscillator amplitude
#define MINAMP 18                      // min oscillator amplitude

#define MAXPERIOD 360                  // max allowable period for an osc
#define MINPERIOD 4                    // min allowable period for an osc

class OscStack : public QuantumOsc 
{
   typedef QuantumOsc super;           // superclass is QuantumOsc

   protected:

   byte  curOsc;                       // current osc #

   byte amp[NUMOSC];                   // amplitude of oscillators

   byte edgIdx[NUMOSC];                // index into current edge 
   byte edgeDC[NUMOSC];                // downcounter to next edge transition

   byte edgLen[NUMOSC][2];             // edge lengths
   char edgVal[NUMOSC][2];             // edge values

   byte note[NUMOSC];                  // current note being played (per osc)

   byte pendNote[NUMOSC];              // pending note (per osc)
   int  pendWarp[NUMOSC];              // pending warp (per osc)

   byte pW;                            // pulse width: 1 to 128 (128=50%)

   public:

   AutoWah  *autowah;                  // built-in autowah for stack
   AutoADSR *env[NUMOSC];              // osc envelopes

   OscStack()
   {
      autowah  = new AutoWah();
      env[0]   = new AutoADSR();
      env[1]   = new AutoADSR();
   }

   void calcEdges( byte t, word period )
   {
      Word posSteps; 
      Word negSteps;

      if ( period <= 1 )
      {
         setSteps( t, 0, 0 );
      }

      posSteps.val  = period;
      posSteps.val *= pW;
      posSteps.val  = posSteps._.msb;

      if ( posSteps.val == 0 ) 
         posSteps.val = 1;

      negSteps.val = period - posSteps.val;

      if ( negSteps.val > 255 )
      {
         posSteps.val += ( negSteps.val - 255 );
         negSteps.val = 255;
      }

      setSteps( t, posSteps._.lsb, negSteps._.lsb );
   }

   boolean charEv( char code )    
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE

         case '0':                     // set current osc
         case '1':

            curOsc = code - '0';
            break;

         case 'a':                     // push autowah

            console.pushMode( autowah );
            break;

         case 'n':                     // set note  
         {
            byte inp;
            if ( console.getByte( CONSTR("note"), &inp ) )
               trigNote( curOsc, inp );
            break;
         }

         case 'p':                     // set phase  
         {
            int inp;
            if ( console.getInt( CONSTR("phase"), &inp ) && inp >= 0 )
               setPhase( curOsc, inp );
            break;
         }

         case 'P':                     // set pulse width
         {
            byte inp;
            if ( console.getByte( CONSTR("PW"), &inp ) )
               setPW( inp );
            break;
         }
  
         #endif

         #ifdef CONSOLE_OUTPUT

         case chrInfo:                 // display object info to console

            super::charEv( chrInfo );
            console.newlntab();
            console.infoByte( CONSTR("PW"), pW );
            autowah->brief();
            break;

         #endif

         case '!':                     // perform a reset

            super::charEv( code );  

            tracking  = false;
            pW        = 128;
            curOsc    = 0;

            for ( byte i = 0; i < NUMOSC; i++ )
            {
               edgIdx[i]    = 0;
               edgVal[i][0] = 0;
               edgVal[i][1] = 0;
               amp[i]       = 0;
               note[i]      = NoNote;
               pendNote[i]  = NoNote;
            }

            autowah->reset();
            autowah->execute( PSTR("<c215\\lf.125\\d45\\") );  // set cutoff,freq,depth

            break;

         default:

            return super::charEv( code );  

      }
   }

   void dynamics()
   {
      autowah->dynamics();

      for ( byte i = 0; i < NUMOSC; i++ )
      {
         if ( pendNote[i] == NoNote )
            env[i]->dynamics();
         else
         {
            setNote( i, pendNote[i], pendWarp[i] );
            env[i]->trigger();
            pendNote[i] = NoNote;
         }

         if ( note[i] != NoNote )
         {
            byte edgAmp = amp[i] * env[i]->value;
            if ( amp[i] < MINAMP )
               edgAmp = 0;
            edgVal[i][0] = edgAmp;
            edgVal[i][1] = -edgAmp;
         }
      }
   }

   void newNote( byte noteNum, byte vol, int warp = 0 )
   {
      // calculate amplitude for new note

      Word regAmp;
      regAmp.val  = vol;
      regAmp.val += 1;
      regAmp.val *= MAXAMP;

      amp[curOsc] = regAmp._.msb;

      trigNote( curOsc, noteNum, warp );
      curOsc ^= 1;                     // flip current osc
   }


   word oscPeriod( byte nth )
   {
      return edgLen[nth][0]+edgLen[nth][1];
   }

   void output( char *buf )
   {
      char *bufstart = buf;            // save buffer addr 
      byte icnt = audioBufSz;          // write this many ticks of output

      while ( icnt-- )                 // for each byte in the buffer
      {
         char outVal = 0;
         for ( byte t = 0; t < NUMOSC; t++ )
         {
            if ( --edgeDC[t] == 0 )       // handle edge transition, if any
            {
               edgIdx[t] ^= 1;            // toggle idx between 1 and 0
               edgeDC[t] = edgLen[t][edgIdx[t]];
            }
            outVal += edgVal[t][edgIdx[t]];
         }
         *buf++ = outVal;
      }

      if ( ! autowah->muted() )
         autowah->process( bufstart );
   }

   double realFreq()                   // return the real frequency
   {
      return periodHz( oscPeriod(0) );
   }

   void setNote( byte t, byte noteNum, int warp = 0 )
   {
      note[t] = noteNum;

      if ( noteNum == NoNote )
      {
         setSteps( t, 0, 0 );
         return;
      }

      // calculate period

      int period = notePeriod( noteNum ) + warp;

      if ( period > MAXPERIOD )
         period = MAXPERIOD;
      else if ( period < MINPERIOD )
         period = MINPERIOD;

      // compute edge lengths
      
      calcEdges( t, period );

      /* phase lock with any identical osc */

      for ( byte i = 0; i < NUMOSC; i++ )
      {
         if ( t == i ) continue;
         if ( period != oscPeriod(i) ) continue;
         
         edgIdx[t] = edgIdx[i];
         edgeDC[t] = edgeDC[i];
      }

   }

   word getPhase( byte t )
   {
      word phase;
      phase  = edgIdx[t] ? 0 : edgLen[t][0];
      phase += edgLen[t][edgIdx[t]] - edgeDC[t];
      return phase;
   }

   #ifdef DEBUG
   void infoNotes()
   {
      console.newlntab();
      for ( byte i = 0; i < NUMOSC; i++ )
      {
         console.infoByte( CONSTR("note"), note[i] );
         console.infoByte( CONSTR("amp"), amp[i] );
      }
   }
   #endif

   void setPhase( byte t, word phase )
   {
      word period = oscPeriod(t);
      while ( phase >= period )
         phase -= period;

      if ( phase < edgLen[t][0] )
      {
         edgIdx[t] = 0;
         edgeDC[t] = edgLen[t][0]-phase;
      }
      else
      {
         edgIdx[t] = 1;
         edgeDC[t] = period-phase;
      }
   }

   void setPW( byte x )
   {
      if ( x < 1 ) 
          x = 1;
      if ( x > 128)
         x = 128;
      pW = x;

      for ( byte i = 0; i < NUMOSC; i++ )
         calcEdges( i, oscPeriod(i) );
   }

   void setSteps( byte t, byte n0, byte n1 )
   {
      edgLen[t][0] = n0;
      edgLen[t][1] = n1;

      char val = ( n0 && n1 ) ? amp[t] * env[t]->value : 0;

      edgVal[t][0] = val;
      edgVal[t][1] = -val;
   }

   void trigNote( byte oscNum, byte noteNum, int warp = 0 )
   {
      pendNote[oscNum] = noteNum;
      pendWarp[oscNum] = warp;
   }

   PROMPT_STR( oscStack ) 

} ;

/******************************************************************************
 *
 *                                 Echotron 
 *
 ******************************************************************************/

#define MINVOL (word)(((MINAMP*256) / MAXAMP) - 1)  

struct Slot           // data structure of a carousel slot 
{
   byte  note;        // current note to echo
   byte  vol;         // volume of note ( 255 == "1.0" )
   bool  hold;        // skip this slot when inserting new notes in carousel
   int   warp;        // accumulated warp
   const char *table; // ptr to table of semi-tone offsets in ROM (a "trope")
   char  idx;         // indexes table (index of current semi-tone offset)
} ;

#define NUMSLOTS 4    // number of slots in the carousel 

//                      ---- panControl ----
//
// panPos determines the static pan position for Echotron::side[0.1] 
//
//    panPos == 0   means side[0] is panned completely to the left 
//    panPos == 128 means side[0] is centered in the stereo field
//    panPos == 255 means side[0] is panned completely to the right
//
// The pan position for side[1] is the complement of that for side[0].

byte        panPos;                       // static pan position for voices  
PanControl  panControl( &panPos );        // dynamic controller of panPos

class Echotron : public StereoInstrument 
{
   typedef StereoInstrument super;        // superclass is StereoInstrument

   public:
                                           
   OscStack *side[2];                     // 1 osc stack per side

   byte curSide;                          // index of side to route next note to

   MasterAutoADSR  env;                   // master envelope controls osc envelopes

   // carousel state variables

   Slot   carousel[ NUMSLOTS ];           // round of notes to echo
   byte   maxNotes;                       // max number of notes permitted in carousel 
   byte   numNotes;                       // number of notes currently in carousel
   byte   insSlot;                        // index into carousel for next note insertion
   byte   echoSlot;                       // index into carousel for next note to echo

   byte   feedback;                       // feedback amount ( 255 == echo forever )

   double delay;                          // delay time between echoed notes 
   word   delayRL;                        // # of dynamic updates between echoed notes
   word   delayDC;                        // # of dynamic updates until next echo 

   bool   isoTempo;                       // delayRL is inversely proportional to numNotes
   bool   hold;                           // hold the next note in carousel 
   bool   samplehold;                     // sample-and-hold is engaged

   char   warp;                           // warp echo this much per trigger
   byte   trope;                          // trope # to use with next inserted note

   byte   prevPW;                         // previous PW value via pot

   Echotron()
   {
      side[0] = new OscStack();
      side[1] = new OscStack();

      env.setNumSlaves(4);

      env.setSlave( 0, side[0]->env[0] );
      env.setSlave( 1, side[0]->env[1] );
      env.setSlave( 2, side[1]->env[0] );
      env.setSlave( 3, side[1]->env[1] );
   }

   boolean charEv( char code )    
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE            // compile cases needed by macros

         case '0':                        // push a side
         case '1':

            console.pushMode( side[code - '0'] );
            break;

         case 'd':                        // set delay time
         {
            double input;
            if ( console.getDouble( CONSTR("delay"), &input ) )
               setDelay( input );
            break;
         }   

         case 'M':                        // push master envelope

            console.pushMode( &this->env );
            break;   

         case 'f':                        // set echo feedback 

            console.getByte( CONSTR("feedback"), &this->feedback );
            break;

         case 'h':                        // hold next noteOn 

            hold = true;
            console.newprompt();
            break;

         case 'i':                        // set isoTempo 
         {
            bool val;
            if ( console.getBool( CONSTR("isoTempo"), &val ) )
               setIsoTempo( val );
            break;
         }

         case 'n':                        // set number of notes in carousel
         {
            byte input;
            if ( console.getByte( CONSTR("notes"), &input ) )
               setMaxNotes( input );
            break;
         }   

         case 'N':                        // execute a noteOn 
         {
            byte noteNum;
            byte octave;
            if ( console.getByte( CONSTR("noteNum"), &noteNum ) )
            {
               octave = 0;
               while ( noteNum >= 12 )
               {
                  ++octave;
                  noteNum -= 12;
               }
               noteOn( key( noteNum, octave ) );
            }
            break;
         }

         case 'P':                        // push the panning control
         
            console.pushMode( &panControl );
            break;   

         #endif

         case 's':                        // set sample-and-hold 

            console.getBool( CONSTR("samphold"), &this->samplehold );
            break;

         case 't':                        // set trope 
         {
            byte val;
            if ( console.getByte( CONSTR("trope"), &val ) )
               setTrope( val );
            break;
         }

         case 'U':                        // unhold all notes

            for ( byte i = 0; i < NUMSLOTS; i++ )
               carousel[i].hold = false;
            hold = false;
            break;

         case 'w':                        // set warp 
         {
            char val;
            if ( console.getSByte( CONSTR("warp"), &val ) )
               setWarp( val );
            break;
         }

         #ifdef CONSOLE_OUTPUT            // compile cases that display to console 

         case chrInfo:

            super::charEv( chrInfo );
            console.newlntab();
            panControl.brief();
            env.brief();
            console.newlntab();
            console.infoBool( CONSTR("samphold"), samplehold );
            console.infoBool( CONSTR("hold"), hold );
            console.newlntab();
            console.infoByte( CONSTR("notes"), maxNotes );
            console.infoBool( CONSTR("isoTempo"), isoTempo );
            console.newlntab();
            console.infoByte( CONSTR("trope"), trope );
            console.infoInt( CONSTR("warp"), warp );
            console.newlntab();
            console.infoDouble( CONSTR("delay"), delay );
            console.infoByte( CONSTR("feedback"), feedback );
            break;

         #endif

         case '!':                        // perform a reset

            super::charEv( code );  

            curSide = 0;
            warp    = 0;
            trope   = 0;
            prevPW  = 0;

            side[0]->reset();          
            side[1]->reset();
            side[1]->autowah->lfo.setFreq( .165 );  // override autowah 1 freq

            // initialize pan control ( unmuted, panPos 0, freq .36, depth 50% )

            panControl.reset();
            execute( PSTR( "P<P0\\f.36\\d64\\" ) );

            // initialize carousel parameters

            numNotes   = 0;               // no notes in carousel
            hold       = false;           // do not hold next note
            samplehold = false;           // sample-and-hold not engaged
            isoTempo   = false;           // delay not based on numNotes

            for ( byte i = 0; i < NUMSLOTS; i++ )
               clearSlot(i);

            setMaxNotes(4);

            feedback = 175;
            delayDC  = 0;                  
            setDelay( .3 );
            //setDelay( .45 );

            // initialize master envelope

            env.reset();
            env.execute( PSTR( "<d42\\s100\\t6\\r150\\" ) );

            break;

         default:

            return super::charEv( code );  

      }
   }

   void clearSlot( byte i )
   {
      if ( i < NUMSLOTS ) 
      {
         carousel[i].note  = NoNote;
         carousel[i].idx   = 0;
         carousel[i].table = ptrPresetTrope(0);
      }
   }

   void dynamics()                        // perform a dynamic update
   {
      //           --- update component dynamics  ---

      side[0]->dynamics();
      side[1]->dynamics();

      panControl.dynamics();

      //       ---  determine whether it is time for the next echo  ---

      if ( ! numNotes )     return;       // carousel is empty
      if ( ! delayDC )      return;       // delay is turned off -- there is no echo
      if ( --delayDC != 0 ) return;       // delay is on but still counting down

      //               ---  it is time for the next echo  ---
      
      delayDC = delayRL;                  // reload delay downcounter

      // locate the next bona-fide note in the carousel to be echoed

      Word echoVol;                       // register for computing echo volume
      byte echoIdx = echoSlot;            // index of slot to check for an echoing note
      
      while ( true )                      // loop through slots 
      {
         if ( carousel[echoIdx].note != NoNote )
         {
            // if sample-and-hold is engaged, or note in slot is held, 
            // do not mitigate echo volume level

            if ( samplehold || carousel[echoIdx].hold )
            {
               echoVol._.msb = carousel[echoIdx].vol;
               break;
            }

            // compute new echo volume level 

            echoVol.val  = feedback;
            echoVol.val += 1;
            echoVol.val *= carousel[echoIdx].vol;

            // if level falls below minimum volume threshold recycle it as a NoNote

            if ( echoVol._.msb < MINVOL )
            {
               carousel[echoIdx].note = NoNote;
               setNumNotes( numNotes-1 );                      
            }
            else
               break;                     // note is bona-fide
         }
         echoIdx = nextSlot( echoIdx );

         if ( echoIdx == echoSlot )       // return if no bona-fide note in carousel
            return;
      }

      //        ---             echo note         ---

      carousel[echoIdx].vol = echoVol._.msb;

      // update warp  

      int nextWarp = carousel[echoIdx].warp + warp;
      if ( nextWarp > MAXPERIOD )
        nextWarp = MAXPERIOD;
      else if ( nextWarp < -MAXPERIOD )
        nextWarp = -MAXPERIOD;

      carousel[echoIdx].warp = nextWarp;

      // add trope offset to note

      char offset = 0;           // offset to apply to note

      // get trope offset from table

      offset = pgm_read_byte_near( carousel[echoIdx].table + carousel[echoIdx].idx );

      //console.infoByte( CONSTR("idx"), carousel[echoIdx].idx );

      if ( offset == EOT )       // end of trope
      {
         if ( carousel[echoIdx].idx )  // if non-null trope
         {
            offset = pgm_read_byte_near( carousel[echoIdx].table );
            carousel[echoIdx].idx = 1;
         }
         else
            offset = 0;            
      }
      else                       // bump trope idx (with wrap)
         carousel[echoIdx].idx += 1;


      int noteToSend = carousel[echoIdx].note + offset;

      while ( noteToSend < 0 )   // insure note + offset is positive
         noteToSend += 12;

      // send note to the osc stack 

      sendNote( noteToSend, carousel[echoIdx].vol, carousel[echoIdx].warp );

      // bump echo slot

      echoSlot = nextSlot( echoIdx );  
   }

   bool evHandler( obEvent e )               // event handler
   {
      switch ( e.type() )
      {     
         case POT0:                          // set feedback 

            setFeedbackViaPot( e.getPotVal() );
            break;

         case POT1:                          // set delay 
         
            setDelayViaPot( e.getPotVal() );
            break;

         default:       

            return super::evHandler(e);      // pass other events through
      }
      return true;
   }

   byte nextSlot( byte slot )
   {
      ++slot;
      if ( slot >= maxNotes )
         slot = 0;
      return slot;
   }

   void noteOn( key note )                // play a note
   {
      if ( samplehold ) return;

      // set insSlot to the first slot where a note can be inserted.
      // if there are no free slots (because all contain held notes), return.

      byte trySlot  = insSlot;
      
      while ( carousel[trySlot].note != NoNote && carousel[trySlot].hold )
      {
         trySlot = nextSlot( trySlot );
         if ( trySlot == insSlot )        // no free slot in carousel
            return;
      }
      insSlot = trySlot;

      // compute note # from note octave and position and any global transposition

      note.transpose( xpose ); 
      byte noteNum = note.octave() * 12 + note.position();

      // bump note count if insertion is occuring in a free slot

      if ( carousel[insSlot].note == NoNote )
         setNumNotes( numNotes+1 );                      

      // insert it into the carousel

      carousel[insSlot].note  = noteNum;
      carousel[insSlot].vol   = 255;
      carousel[insSlot].hold  = hold;
      carousel[insSlot].warp  = 0;
      carousel[insSlot].table = ptrPresetTrope( trope );
      carousel[insSlot].idx   = 0;

      insSlot = nextSlot( insSlot );      // update insertion slot
      
      if ( numNotes == 1 && delayRL )
         delayDC = 1;

      hold   = false;

   }

   void output( char *bufL, char *bufR )
   {
      if ( muted() )                      // output "silence"
      {
         for ( byte i = 0 ; i < audioBufSz; i++ )
         {
            bufL[i] = 0;
            bufR[i] = 0;
         }
         return;   
      }
      
      // output is not muted, proceed normally

      side[0]->output( bufL );
      side[1]->output( bufR );

      // mix side 0 & 1 between the two channels based on panPos

      word coPanning = 256 - panPos;         // complement of panPos

      // pan the sides such that:
      //
      //    bufL = (256-panPos)*side[0] + panPos*side[1]
      //    bufR = (256-panPos)*side[1] + panPos*side[0]
      //

      Int  sum;                              // sum of side 0 & 1 output
      Int  regL;                             // register for computing left buffer
      Int  regR;                             // register for computing rght buffer

      for ( byte i = 0 ; i < audioBufSz; i++ )
      {
         sum.val    = bufL[i] + bufR[i];

         regR.val   = panPos;
         regR.val  *= bufR[i];

         regL.val   = coPanning;
         regL.val  *= bufL[i];
         regL.val  += regR.val;

         bufL[i]    = regL._.msb;

         sum.val   -= regL._.msb;
         bufR[i]    = sum._.lsb;
      }

   }

   byte prevSlot( byte slot )
   {
      if ( slot == 0 || slot >= maxNotes )
         return maxNotes-1;
      else
         return --slot;
   }

   const char *ptrPresetTrope( byte ith )
   {
      if ( ith >= NUM_TROPES )
         ith = 0;

      Word presetAddr;
      word idxPresets  = (word )tropes + ( ith << 1 );
      presetAddr._.lsb = pgm_read_byte_near( idxPresets++ );
      presetAddr._.msb = pgm_read_byte_near( idxPresets );
      return (const char*) presetAddr.val;

      //return &trope00[0];
   }

   void sendNote( byte noteNum, byte noteVol, int warp = 0 )     
   {
      side[curSide]->newNote( noteNum, noteVol, warp );  
      curSide ^= 1;                                
   }
            
   void setDelay( double time )
   {
      if ( time >= 0.0 )
      {
         delay   = time;
         updateDelay();
      }
   }

   void setDelayViaPot( byte value )
   {
      #define LOG_BUCKET 16            // scale semi-logarithmically every 16 vals

      double delay = 0.0;
      double scale = .00125;           

      while ( value )                  
      {
         if ( value < LOG_BUCKET )
         {
            delay += value * scale;
            value = 0;
         }
         else
         {
            delay += LOG_BUCKET * scale;
            value -= LOG_BUCKET;
            scale *= 1.25;
         }
      }

      setDelay( delay );

      #undef LOG_BUCKET
   }

   void setFeedbackViaPot( byte value )
   {
      feedback = 128 + (value >> 1);
   }

   void setIsoTempo( bool val )
   {
      isoTempo = val;
   }

   void setNumNotes( byte num )
   {
      if ( num > maxNotes )
         num = maxNotes;

      if ( isoTempo && num != numNotes && num + numNotes != 1 )
      {
         numNotes = num;
         updateDelay();
      }   
      else
         numNotes = num;

   }

   void setMaxNotes( byte num )
   {
      // range-check num 

      if ( num < 1 )
         num = 1;
      if ( num > NUMSLOTS )
         num = NUMSLOTS;

      // set maxNotes

      maxNotes = num;

      // reset slot indexes

      echoSlot = 0;
      insSlot  = 0;

      // count number of notes within maxNote boundary, 
      // at the same time look for a cleared slot for insSlot

      byte count = 0;
      for ( byte i = 0; i < maxNotes; i++ )
         if ( carousel[i].note == NoNote ) 
            insSlot = i;
         else
            ++count;
      setNumNotes( count );

      // clear slots beyond maxNotes

      for ( byte i = maxNotes; i <= NUMSLOTS; i++ )
         clearSlot( i );

   }

   void setPwViaPot( byte val )
   {
      val >>= 1;
      if ( val != prevPW )
       {
          side[0]->setPW( val );
          side[1]->setPW( val );
          prevPW = val;
       }
   }

   void setTrope( byte nth )
   {
      if ( nth >= NUM_TROPES )
         nth = 0;
      trope = nth;
   }

   void setWarp( byte val )
   {
      // If the warp is changing directions, adjust the accumulated warp
      // of each note in the carousel so that when applied the resultant
      // period will not exceed MAXPERIOD or MINPERIOD. This insures that 
      // the next echo of the note will begin moving in the new direction.

      if ( val > 0 && warp <= 0 )
      {
         int minAccWarp;
         for ( byte i = 0; i < maxNotes; i++ )
         {
            minAccWarp = MINPERIOD - notePeriod( carousel[i].note );
            if ( carousel[i].warp < minAccWarp )
               carousel[i].warp = minAccWarp;
         }
      }
      else if ( val < 0 && warp >= 0 )
      {
         int maxAccWarp;
         for ( byte i = 0; i < maxNotes; i++ )
         {
            maxAccWarp = notePeriod( carousel[i].note ) - MAXPERIOD;
            if ( carousel[i].warp > maxAccWarp )
               carousel[i].warp = maxAccWarp;
         }
      }
      warp = val;
   }

   void setWarpViaPot( byte value )
   {
      bool positive;
      if ( value > 127 )
      {
         value -= 127;
         positive = false;
      }
      else
      {
         value = 127 - value;
         positive = true;
      }
      value >>= 3;                 // divide by 8;

      #define ZERO_SWATH 3

      // make "0" swath ZERO_SWATH times as large as other swaths

      if ( value > ZERO_SWATH )             
         value -= ZERO_SWATH;
      else if ( value > 0 )
         value = 0;

      setWarp( positive ? value : -value );
   }

   void updateDelay()
   {
      double reload = delay * dynaRate;

      if ( isoTempo && numNotes > 1 ) 
         reload /= numNotes;

      reload += 0.5;

      delayRL = reload;
      if ( delayRL == 0 )
         delayDC = 0;
      else if ( delayDC == 0 && numNotes > 0 )
         delayDC = delayRL;
   }

   PROMPT_STR( Echotron ) 

#ifdef AUDIT_ECHO

} echo ;

#else

} ;

StereoInstrument echo;

#endif

/******************************************************************************
 *
 *                                  XGain 
 *
 ******************************************************************************/

class XGain : public Factor         // a cheap gain that uses little ROM
{
   typedef Factor super;            // superclass is Factor

   protected:

   double gain;                     // gain amount (1.0 = parity)

   public:

   XGain()
   {
      shortcut = 'g';
   }

   boolean charEv( char code )      // process a character event
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE
         case 'g':                        // set gain
         {
            double userInp;
            if ( console.getDouble( CONSTR("gain"), &userInp ) )
               setGain( userInp );
            break;
         }
         #endif

         #ifdef CONSOLE_OUTPUT
         case chrInfo:
         {
            super::charEv( code );
            console.infoDouble( CONSTR("gain"), gain );
            break;
         }
         #endif

         case '.':                        // mute

            super::charEv( code );
            value = 1.0;
            break;

         case '<':                        // unmute

            super::charEv( code );
            value = gain;
            break;

         case '!':                        // reset

            super::charEv( code );
            setMute( false );
            setGain( 1.0 );
            break;

         default:
            return super::charEv( code );
      }
      return true;
   }

   void setGain( double g )
   {
      gain = g;
      if ( ! muted() )
         value = gain;
   }

   PROMPT_STR( gain ) 

} ;


/******************************************************************************
 *
 *                                  LeadVox0 
 *
 ******************************************************************************/

class LeadVox0 : public ADSRVoice 
{
   typedef ADSRVoice super;               // superclass is ADSRVoice

   public:

   WaveOsc *wave;
   XGain   *gain;

   LeadVox0()
   {
      wave = new WaveOsc();
      #ifdef AUDIT_LEAD
         wave->setTable( wavetable( Ether ) );
      #else
         wave->setTable( wavetable( Sine ) );
      #endif

      useOsc( wave );

      gain = new XGain();
      addAmpMod( gain );
   }

} leadVox0;

/******************************************************************************
 *
 *                                  LeadVox1 
 *
 ******************************************************************************/

class LeadVox1 : public LeadVox0 
{
   typedef LeadVox0 super;               // superclass is LeadVox0

   public:

   XorOsc *pair;

   LeadVox1()
   {
      #ifdef AUDIT_LEAD
         pair = new XorOsc( new Sine(), new Sine() );
      #endif
   }
   
   boolean charEv( char code )    
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE

         #endif

         case '!':                     // perform a reset

            super::charEv( code );
            #ifdef AUDIT_LEAD
               pair->reset();
            #endif
            break;

         default:

            return super::charEv( code );  

      }
   }

   void usePair()
   {
      #ifdef AUDIT_LEAD
         useOsc( pair );
         pair->setFreq( 0.0 );
      #endif
   }

} leadVox1;

/******************************************************************************
 *
 *                                ParmMenu
 *
 ******************************************************************************/

class ParmMenu : public Mode           // Runtime parameter menu
{
   typedef Mode super;                 // superclass is Mode

   public:

   bool charEv( char code )            // process a character event
   {
      switch ( code )
      {
         case focusPUSH:

            blinkLED(0);               // set up alternating blinking LEDs
            blinkLED(1, true);
            break;

         default:

            return super::charEv( code );
      }
      return true;
   }

   bool evHandler( obEvent ev );       // handle an onboard event

   byte slotNumViaPot0()               // return slot # based on pot 0 position
   {
      byte slot = readPot(0);
      slot >>= 6;                      // this works only if NUMSLOTS == 4!
      return slot;                    
   }

   PROMPT_STR( ParmMenu ) 

} parmMenu;

/******************************************************************************
 *
 *                              QuadrantSynth
 *
 ******************************************************************************/

ByteMenu presetMenu;                // keybrd menu for selecting presets

enum {  // these values enumerate bits in QuadrantSynth::envPotSend 

        ENVPOT_LEAD = 1,            // envelope pots control lead voice envelopes
        ENVPOT_ECHO = 2             // envelope pots control Echotron envelopes

     } ;

MasterADSR env;                     // master envelope for lead voices

class QuadrantSynth : public VoxSynth
{
   typedef VoxSynth super;          // superclass is VoxSynth

   public:

   // The audio balance between the echo and the lead voice is determined 
   // by the state var balance. It can range between the following values:
   //
   //    balance = 0   --> audio: 1/2 echo output + 1/2 lead output
   //    balamce = 255 --> audio: 3/4 echo output + 1/4 lead output

   byte   balance;                  // audio balance between echo and lead
   byte   scaleEcho;                // scaling coefficent for echo output
   byte   scaleLead;                // scaling coefficent for lead output               

   double lfoSkew;                  // amount to skew LFO frequencies in stereo

   byte envPotSend;                 // controls which envelopes are effected by pots  

   void config()
   {
      configVoices(2);

      setFrameDimensions( 2, 2 );   // dimensions of embedded user interface frame
      
      // configure master envelope to control envelopes of lead voices

      env.setNumSlaves(2);

      env.setSlave( 0, &leadVox0.envAmp );
      env.setSlave( 1, &leadVox1.envAmp );

      keybrd.setDefOct( 2 );        // start keyboard in octave 2
   }

   Tuning *tuning()
   {
      return new TweekedHarmonicTuning(); // use custom tuning
   }

   // Note: newOsc() will never be called (back) by the system because all 
   // oscillators in this sketch are allocated and registered via useOsc() 
   // within their respective voice constructors, and the system only calls 
   // newOsc() for voices that have a NULL osc ptr after newVox() for that
   // voice has been called. However, we override newOsc() here simply to 
   // avoid having the newOsc() method of the super class linked in -- 
   // which costs an extra 400 bytes of ROM!

   Osc *newOsc( byte nth )                // create nth osc -- NEVER CALLED
   {
      return NULL;                   
   }
      
   Voice *newVox( byte nth )              // create nth voice
   {
      if ( nth == 0 )
         return &leadVox0;
      else
         return &leadVox1;
   }
      
   bool charEv( char code )            // process a character event
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE         // compile cases needed by macros

         case 'b':                     // set balance between echo / lead
         {
            byte input;
            if ( console.getByte( CONSTR("balance"), &input ) )
               setBalance( input );
            break;
         }

         #ifdef DEBUG
         case 'i':                     // give info on notes in carousel
         
            echo.side[0]->infoNotes();
            echo.side[1]->infoNotes();
            console.newprompt();
            break;
         #endif

         case 'E':                     // push Echotron

            console.pushMode( &echo );
            break;

         case 'M':                     // push master envelope

            console.pushMode( &env );
            break;

         case 'l':

            console.getDouble( CONSTR("lfoSkew"), &lfoSkew );
            break;

         case 'P':                     // push parameter menu

            console.pushMode( &parmMenu );
            break;

         case 's':                     // set envPotSend

            console.getByte( CONSTR("sendEnv"), &envPotSend );
            break;

         case 'X':                     // use lead voice configuration X

            leadVox1.usePair();
            break;

         #endif

         #ifdef CONSOLE_OUTPUT

         case chrInfo:

            super::charEv( code );
            console.newlntab();
            echo.brief();
            env.brief();
            console.infoByte( CONSTR("sendEnv"), envPotSend );
            console.newlntab();
            console.infoByte( CONSTR("balance"), balance );
            console.infoDouble( CONSTR("lfoSkew"), lfoSkew );
            break;

         #endif

         case '!':                     // perform a reset

            super::charEv( code );

            enableFrame();             // use embedded user-interface frame
            
            envPotSend = ENVPOT_LEAD + ENVPOT_ECHO;

            echo.reset();
            lfoSkew = .667;

            env.reset();
            env.setMute( false );
            env.execute( PSTR( "ed50\\s36\\r110\\" ) );  

            execute( PSTR( "1d-26\\" ) );

            setBalance(95);
            break;

         default:

            return super::charEv( code );  
      }
      return true;
   }

   void dynamics()                           // perform a dynamic update
   {
      super::dynamics();
      echo.dynamics();
   }

   bool evHandler( obEvent e )               // event handler
   {

      if ( handlePots(e) ) return true;      // handle any pot events

      switch ( e.type() )
      {     
         case BUT0_DTAP:                     // push parameter menu

            console.pushMode( &parmMenu );
            break;

         case BUT1_DTAP:                     // run a preset 

            presetMenu.waitKey();            // user selects preset by pressing key
            runPreset( (const char *)presets.dataPtr( presetMenu.value ) );
            break;

         default:       

            return super::evHandler(e);      // pass other events through
      }
      return true;
   }

   bool handlePots( obEvent ev )             // handle pot events
   {
      if ( ! ev.amPot() )                    // return false if not a pot event
         return false;                       

      byte potVal = ev.getPotVal();          // cache pot value

      switch ( ev.type() )                   // execute case by pot#_frame#
      {
         #ifdef AUDIT_ECHO

         /*             FRAME 00             */

         case POT0_F00:                      // set warp 

            echo.setWarpViaPot(potVal);
            break;

         case POT1_F00:                      // set delay

            echo.setDelayViaPot(potVal);
            break;

         /*             FRAME 10             */

         case POT0_F10:                      // set feedback 

            echo.setFeedbackViaPot(potVal);
            break;

         case POT1_F10:                      // set balance 

            setBalance(potVal);
            break;

         /*             FRAME 20             */

         case POT0_F20:                      // pan freq 
         case POT1_F20:                      // pan depth 

            panControl.potEv(ev);
            break;

         /*             FRAME 01             */

         case POT0_F01:                      // autowah freq 

            echo.side[0]->autowah->lfo.potEv(ev);
            echo.side[1]->autowah->lfo.setFreq( echo.side[0]->autowah->lfo.getFreq() * lfoSkew );
            break;

         case POT1_F01:                      // autowah depth  

            echo.side[0]->autowah->lfo.potEv(ev);
            echo.side[1]->autowah->lfo.potEv(ev);
            break;

         /*             FRAME 11             */

         case POT0_F11:                      // autowah cutoff

            for ( byte i = 0; i <= 1; i++ )
               echo.side[i]->autowah->setCutoff( potVal );
            break;

         case POT1_F11:                      // echotron pulse width 

            echo.setPwViaPot( potVal );
            break;

         #endif // ifdef AUDI_ECHO

         /*             FRAME 21             */

         case POT0_F21:                      // leadVox1 detune

            leadVox1.osc->setDetune( potVal - 128 );
            break;

         case POT1_F21:                      // leadVox1 transpose

            {
               // transpose lead 1 by between -24 and 24 semitones

               int transpose  = potVal;
               if (transpose >= 254 ) transpose = 256; 
                   transpose -= 128;
                   transpose *= 48;
                   transpose >>= 8;

               leadVox1.xpose = transpose;
            }
            break;

         /*             FRAME 02             */

         case POT0_F02:                      // set attack for envelopes

            if ( envPotSend & ENVPOT_LEAD )
               env.setAttack( potVal );

            #ifdef AUDIT_ECHO
            if ( envPotSend & ENVPOT_ECHO )
               echo.env.setAttack( potVal );
            #endif

            break;
                                             // set decay for envelopes
         case POT1_F02:                       

            if ( envPotSend & ENVPOT_LEAD )
               env.setDecay( potVal );

            #ifdef AUDIT_ECHO
            if ( envPotSend & ENVPOT_ECHO )
               echo.env.setDecay( potVal );
            #endif

            break;

         /*             FRAME 12             */

         case POT0_F12:                      // set sustain for envelopes

            if ( envPotSend & ENVPOT_LEAD )
               env.setSustain( potVal );

            #ifdef AUDIT_ECHO
            if ( envPotSend & ENVPOT_ECHO )
               echo.env.setSustain( potVal );
            #endif

            break;
                                             // set release for envelopes
         case POT1_F12:                       

            if ( envPotSend & ENVPOT_LEAD )
               env.setRelease( potVal );

            #ifdef AUDIT_ECHO
            if ( envPotSend & ENVPOT_ECHO )
               echo.env.setRelease( potVal );
            #endif

            break;

         /*             FRAME 22             */

         case POT0_F22:                      // set sustime  

            #ifdef AUDIT_ECHO
            echo.env.setSusTime( potVal );
            #endif
            break; 

         case POT1_F22:                      // set lead portamento  

            leadVox0.setGlide( potVal );
            leadVox1.setGlide( potVal );
            break; 

      }
      return true;
   }

   void noteOn( key note )                   // turn a note on
   {
      super::noteOn( note );
      echo.noteOn( note );
   }

   void output( char *bufL, char *bufR )
   {
      char leadbuf[ audioBufSz ];           // temp buffer for lead vox output

      echo.output( bufL, bufR );

      // mix lead voices with echo (per channel)

      int scaledLead;
      Int sum;

      leadVox0.output( leadbuf );
      for ( byte i = 0 ; i < audioBufSz; i++ )
      {
         scaledLead = leadbuf[i] * scaleLead;

         sum.val = scaledLead + bufL[i] * scaleEcho;
         bufL[i] = sum._.msb;
      }

      leadVox1.output( leadbuf );
      for ( byte i = 0 ; i < audioBufSz; i++ )
      {
         scaledLead = leadbuf[i] * scaleLead;

         sum.val = scaledLead + bufR[i] * scaleEcho;
         bufR[i] = sum._.msb;
      }

      /*
      for ( byte i = 0 ; i < audioBufSz; i++ )
      {
         bufR[i] = 0;
      }
      */
   }

   // the higher the balance input value, the louder the echo vs lead

   void setBalance( byte val )
   {
      balance = val;

      word coeff = balance + 1;
      coeff >>= 2;
      coeff += 128;

      scaleEcho = coeff;

      coeff = 256 - scaleEcho;

      scaleLead = coeff;
   }

   void toggleLead()                         // toggle lead voices (mute/unmute)
   {
      bool status = !vox[0]->muted();
      vox[0]->setMute( status );
      vox[1]->setMute( status );
   }

   void welcome()
   {
      presets.load( myPresets );               

      // set up preset 0 

      // runPreset( (const char *)presets.dataPtr( 0 ) ); 

   }

} mySynth;


bool ParmMenu::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case KEY_DOWN:

         switch ( ev.getKey().position() )
         {
            #ifdef AUDIT_ECHO

            case 0:                 // 'C'  toggles sample-and-hold

               echo.samplehold = ! echo.samplehold;
               break;

            case 1:                 // 'C#' clears sample-and-hold and all held notes

               echo.samplehold = false;
               for ( byte i = 0; i < NUMSLOTS; i++ )
                  echo.carousel[i].hold = false;
               break;

            case 2:                 // 'D'  sets hold for next note played 

               echo.hold = true;
               break;

            #endif // AUDIT_ECHO

            case 3:                 // 'D#' 

               break;

            #ifdef AUDIT_ECHO

            case 4:                 // 'E'  toggle lead voices on/off 

               mySynth.toggleLead();
               break;

            case 5:                 // 'F' set number of slots

               echo.setMaxNotes( slotNumViaPot0()+1 );
               break;

            case 6:                 // 'F#'  toggle isoTempo on/off

               echo.isoTempo = ! echo.isoTempo;
               break;

            case 7:                 // 'G'  select trope via pot 1 position
            {
               Word nth;
               nth.val = readPot(1) * NUM_TROPES;
               echo.setTrope( nth._.msb );
               break;
            }
            case 8:                 // 'G#'

               break;

            #endif // AUDIT_ECHO

            case 9:                 // 'A'  envelope pots control both lead + echo

               mySynth.envPotSend = ENVPOT_LEAD | ENVPOT_ECHO;
               break;

            case 10:                // 'A#' envelope pots control lead only

               mySynth.envPotSend = ENVPOT_LEAD;
               break;

            case 11:                // 'B'  envelope pots control echo only

               mySynth.envPotSend = ENVPOT_ECHO;
               break;
         }

         break;
      
      case KEY_UP:

         console.popMode();
         break;

      default:                      // pass other events to superclass

         return super::evHandler(ev);      
   }

   return true;                 
}


#ifdef SECT_BADGE_PROJ
/*********************************************
*
* This section is for support for the programmable RGB LEDs (which are WS2812b, aka: Neopixels)
*
* This entire section was written by Josh Levine (credits and license given near the top of this file)
*
**********************************************/

/*********************
*
* sendBit
*
* Send a bit to the LED string. We must to drop to asm to enusre that the complier does
*    not reorder things and make it so the delay happens in the wrong place.
*
**********************/
inline void sendBit(bool bitVal) {

  if (bitVal) {  // 0-bit

    asm volatile(
      "sbi %[port], %[bit] \n\t"  // Set the output bit
      ".rept %[onCycles] \n\t"    // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"  // Clear the output bit
      ".rept %[offCycles] \n\t"   // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t" ::
        [port] "I"(_SFR_IO_ADDR(PIXEL_PORT)),
      [bit] "I"(PIXEL_BIT),
      [onCycles] "I"(NS_TO_CYCLES(T1H) - 2),  // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
      [offCycles] "I"(NS_TO_CYCLES(T1L) - 2)  // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness

    );

  } else {  // 1-bit

    // **************************************************************************
    // The timing for the 1-bit is really the only tight goldilocks timing in the whole program!
    // **************************************************************************
    asm volatile(
      "sbi %[port], %[bit] \n\t"  // Set the output bit
      ".rept %[onCycles] \n\t"    // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
      "nop \n\t"                  // Execute NOPs to delay exactly the specified number of cycles
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"  // Clear the output bit
      ".rept %[offCycles] \n\t"   // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t" ::
        [port] "I"(_SFR_IO_ADDR(PIXEL_PORT)),
      [bit] "I"(PIXEL_BIT),
      [onCycles] "I"(NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I"(NS_TO_CYCLES(T0L) - 2)

    );
  }

  // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is a long time)
  // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
  // This has thenice side effect of avoid glitches on very long strings
}


/*********************
*
* sendByte
*
* Send a byte to the LED string
*
**********************/
inline void sendByte(unsigned char byte) {

  for (unsigned char bit = 0; bit < 8; bit++) {
    sendBit(bitRead(byte, 7));  // Neopixel wants bit in highest-to-lowest order
                                // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
    byte <<= 1;                 // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
  }
}


/*

  The following three functions are the public API:
  
  ledSetup() - set up the pin that is connected to the string. Call once at the begining of the program.  
  sendPixel(r, g, b) - send a single pixel to the string. Call this once for each pixel in a frame.
  show() - show the recently sent pixel on the LEDs . Call once per frame. 
  
*/


/*********************
*
* ledsetup
*
* Set the specified pin up as digital output
*
**********************/
void ledsetup() {

  bitSet(PIXEL_DDR, PIXEL_BIT);
}


/*********************
*
* sendPixel
*
* Send a pixel value to the LED string
*
**********************/
inline void sendPixel(unsigned char r, unsigned char g, unsigned char b) {

  sendByte(r >> BRIGHTNESS);  // Neopixel wants colors in red then green then blue order
  sendByte(g >> BRIGHTNESS);  // Shift each color value by the BRIGHTNESS to make it dimmer than given RGB-value brightnesses
  sendByte(b >> BRIGHTNESS);  //    example: BRIGHTNESS=5 gives a thirty-secondth of the brightness of each value for R, G, and B
}


/*********************
*
* show
*
* To show the pixels that were sent to the LED string
*    just wait long enough without sending any bits to cause the pixels to latch and display the last sent frame
*
**********************/
/*
void show() {
	_delay_us( (RES/1000UL) + 1 );				// Round up since the delay must be _at_least_ this long (too short might not work, too long not a problem)
}
*/


/*

  That is the whole API. What follows are some demo functions rewriten from the AdaFruit strandtest code...
  
  https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino
  
  Note that we always turn off interrupts while we are sending pixels becuase an interupt
  could happen just when we were in the middle of somehting time sensitive.
  
  If we wanted to minimize the time interrupts were off, we could instead 
  get away with only turning off interrupts just for the very brief moment 
  when we are actually sending a 0 bit (~1us), as long as we were sure that the total time 
  taken by any interrupts + the time in our pixel generation code never exceeded the reset time (5us).
  
*/


/*********************
*
* showColor -- Display a single color on the whole string
*
**********************/
void showColor(unsigned char r, unsigned char g, unsigned char b) {

  cli();

  for (int p = 0; p < NUM_PIXELS; p++) {
    sendPixel(r, g, b);
  }

  sei();
  //show();
}


/*********************
*
* colorWipe -- Fill the pixels one after the other with a color
*
**********************/
/*
void colorWipe(unsigned char r , unsigned char g, unsigned char b, unsigned char wait ) {
  for (unsigned int i=0; i<NUM_PIXELS; i++) {
    
    cli();
    unsigned int p=0;
    
    while (p++<=i) {          // as we go through the wipe, the first pixels get the RGB color
        sendPixel(r, g, b);
    } 
     
    while (p++<=NUM_PIXELS) {     //    and the last pixels are off
        sendPixel(0, 0, 0);  
    }
    
    sei();

    show();
    delay(wait);
  }
}
*/


/*********************
*
* theaterChase -- Theatre-style crawling lights
*
* Changes spacing to be dynmaic based on string size
*
**********************/
/*
#define THEATER_SPACING (NUM_PIXELS/20)
void theaterChase( unsigned char r , unsigned char g, unsigned char b, unsigned char wait ) {
  
  for (int j=0; j< 3 ; j++) {  
    for (int q=0; q < THEATER_SPACING ; q++) {
      unsigned int step=0;
      
      cli();
      
      for (int i=0; i < NUM_PIXELS ; i++) {
        if (step==q) {
          sendPixel(r, g, b);
        } else {
          sendPixel(0, 0, 0);
        }
        
        step++;
        
        if (step==THEATER_SPACING) step = 0;
      }
      
      sei();
      
      show();
      delay(wait);
    }
  }
}
*/


/*********************
*
* rainbowCycle -- cyle through rainbow colors on the LED string
*
*     frames:        the number of Hues to cycle through on the LED string
*     frameAdvance:  how much to increment the Hue of the first pixel each cyle through the LED string
*     pixelAdvance:  how much the Hue in next pixel in the LED string is incremented from the previous pixel
*     wait:          delay in ms between each frame
*
**********************/
/*
void rainbowCycle(unsigned char frames , unsigned int frameAdvance, unsigned int pixelAdvance, unsigned char wait ) {
  
  // Hue is a number between 0 and 3*256 that defines a mix of r->g->b where
  // hue of 0 = Full red
  // hue of 128 = 1/2 red and 1/2 green
  // hue of 256 = Full Green
  // hue of 384 = 1/2 green and 1/2 blue
  // ...
  
  unsigned int firstPixelHue = 0;     // Color for the first pixel in the string
  
  for (unsigned int frameCount=0; frameCount<frames; frameCount++) {                                  
    unsigned int currentPixelHue = firstPixelHue;
       
    cli();    
        
    for (unsigned int i=0; i<PIXELS; i++) {
      if ( currentPixelHue >= (3*256) ) {                  // wrap the Hue value around when we incremented and overflowed
        currentPixelHue -= (3*256);
      }
            
      unsigned char phase = currentPixelHue >> 8;
      unsigned char step = currentPixelHue & 0xff;
      switch (phase) {
        case 0: 
          sendPixel(~step, step,  0);
          break;
        case 1: 
          sendPixel(0, ~step, step);
          break;
        case 2: 
          sendPixel(step, 0, ~step);
          break;
      }
      
      currentPixelHue += pixelAdvance;                                      
    } 
    
    sei();
    
    show();
    firstPixelHue += frameAdvance;
    delay(wait);
  }
}
*/


/*********************
*
* I added this one just to demonstrate how quickly you can flash the string.
* Flashes get faster and faster until *boom* and fade to black.
*
**********************/
/*
void detonate( unsigned char r , unsigned char g , unsigned char b , unsigned int startdelayms) {

  while (startdelayms) {
    showColor(r, g, b);      // Flash the color 
    showColor(0, 0, 0);
    delay(startdelayms);      
    startdelayms =  (startdelayms*4) / 5 ;           // delay between flashes is halved each time until zero
  }
  
  // Then we fade to black....
  for (int fade=256; fade>0; fade--) {
    showColor( (r * fade) / 256 , (g*fade) /256 , (b*fade)/256 );
  }
  showColor(0, 0, 0);

}
*/

#endif   // SECT_BADGE_PROJ



//--------------------
//--------------------

// Here are the variants for the setup() function, depending on the declarations in System.h:


// setup()     For the ArduTouch board, we only control the synth.
#ifdef ARDUTOUCH_PROJ
void setup() {
  // initialize the synth
  ardutouch_setup(&mySynth);
}
#endif


//---


// setup()     For the SEC-T Synth Badge, in addition to controlling the synth, also control the 5 RGB LEDs.
//                Controlling the 5 RGB LEDS will degrade the audio quality of the synth a little.
#ifdef SECT_BADGE_PROJ
   #ifdef RGB_YES
void setup() {
  // initialize the synth
  ardutouch_setup(&mySynth);

  // initialize the programmable RGB LEDs
  ledsetup();
  showColor(0, 0, 0);  // start with them all off
}
   #endif
#endif


//---


// setup()     For the SEC-T Synth Badge, in addition to controlling the synth, also control the 5 RGB LEDs.
//                Controlling the 5 RGB LEDS will degrade the audio quality of the synth a little.
#ifdef SECT_BADGE_PROJ
   #ifdef RGB_NO
void setup() {
  // initialize the synth
  ardutouch_setup(&mySynth);

  // initialize the programmable RGB LEDs
  ledsetup();
  showColor(0, 0, 0);  // start with them all off
}
   #endif
#endif



//--------------------
//--------------------

// Here are the variants for the loop() function, depending on the declarations in System.h:


// loop()     For the ArduTouch board, we only control the synth.
#ifdef ARDUTOUCH_PROJ
void loop() {
   ardutouch_loop();
}
#endif


//---


// loop()     For the SEC-T Synth Badge, in addition to controlling the synth, also control the 5 RGB LEDs.
//               Controlling the 5 RGB LEDS will degrade the audio quality of the synth a little.
#ifdef SECT_BADGE_PROJ
   #ifdef RGB_YES
// To get the best audio quality for the synthesizer, the loop() section should ideally only have one line:
//             ardutouch_loop();
// But, I added RGB LED control to the loop() section -- making use of the rainbowCycle function (which is commented out, above).
//     To avoid messing up the audio quality of the synthesizer too much,
//        we need to be sure to not take much time away from the synth engine firmware.
//     So, the lines of code for controlling the RGB LEDs are only executed
//        every LED_COUNT times through each section of the loop().
//     I chose the value of LED_COUNT empirically, so that there is at least 250ms after sending RGB color values to the LEDs.
//     That both guarantees that the LEDs will display their new RGB color values, 
//        as well as not degrading the audio quality of the synth too much.
unsigned int ledCount;  // to count times through each section of the loop() before performing next step in controlling RGB LEDs
const uint16_t LED_COUNT = 11000;      // 
const uint16_t WHEEL_CONST = 256 / NUM_PIXELS;
const unsigned char frames = 128;      // the number of Hues to cycle through on the LED string
const unsigned int frameAdvance = 23;  // the number of Hues to cycle through on the LED string
const unsigned int pixelAdvance = 42;  // how much the Hue in next pixel in the LED string is incremented from the previous pixel
unsigned int firstPixelHue;            // Hue for the first pixel in the LED string
unsigned int currentPixelHue;          // keep track of current Hue in the first pixel
unsigned int pixelCount;               // to count pixels
unsigned int frameCount;               // to count the Hue cycles in a frame
unsigned char phase;                   // for shifting Hue values
unsigned char step;                    // for masking Hue values into 8-bit byte

void loop() {
  firstPixelHue = 0;
  // cycle through all Hues in the frame
  for (frameCount = 0; frameCount < frames; frameCount++) {
    currentPixelHue = firstPixelHue;
    cli();  // no interrupts while sending all 5 Hue values to LEDs
    // cycle through all programmable RGB LEDs
    for (pixelCount = 0; pixelCount < NUM_PIXELS; pixelCount++) {
      // only update the Hues after LED_COUNT cycles of the ardutouch_loop
      // wrap the Hue value around when we incremented and overflowed
      if (currentPixelHue >= (3 * 256)) {
        currentPixelHue -= (3 * 256);
      }
      // send Hue for this pixel
      phase = currentPixelHue >> 8;
      step = currentPixelHue & 0xff;
      switch (phase) {
        case 0:
          sendPixel(~step, step, 0);
          break;
        case 1:
          sendPixel(0, ~step, step);
          break;
        case 2:
          sendPixel(step, 0, ~step);
          break;
      }
      // advance the Hue for the next pixel
      currentPixelHue += pixelAdvance;
    }
    sei();  // interrupts re-enabled
    for (ledCount = 0; ledCount < LED_COUNT; ledCount++) {
      ardutouch_loop();
    }
    // advance to the next Hue for the first pixel of the next frame
    firstPixelHue += frameAdvance;
  }
}
   #endif
#endif


//---


// loop()     For the SEC-T Synth Badge, there is an option to not control the RGB LEDs,
//               which will make the audio quality of the Synth be the same as for the ArduTouch board
//               (but the 5 RGB LEDs will remain off).  
//               (It will also free a few more bytes for hacking the synth code.)
#ifdef SECT_BADGE_PROJ
   #ifdef RGB_NO
void loop() {
   ardutouch_loop();
}
   #endif
#endif
