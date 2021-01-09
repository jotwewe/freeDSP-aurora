This branch https://github.com/jotwewe/freeDSP-aurora/tree/2.0.3_Display_Encoder
is based on 2.0.3. Summary of changes:

 - Toggle wifi by pressing the encoder's button on bootup (from 2.1.2).
 - Enhanced display of wifi state.
 - Reading rotary encoder differs from the code in 2.1.2, it might be
   faster. It does not interfere with accessing the spi file system. Previous
   versions crashed/rebootet the system.   
 - This code is meant for a stereo system: input selection via encoder
   (three analog inputs, USB, and Expansion Port).
 - Master volume can be changed using the encoder.
 - Preset can be changed via encoder; the input selection stored in the
   preset is overwritten by what had been selected via encoder. Master 
   volume does not change when switching presets using the encoder.
 - Display turns off after some seconds of inactivity. Rotating or
   pressing the encoder turns it on.
 - The expansion port can be selected as an input.
 - It is only meant to be used with the plugins "8channels" or
   "8channels_ExpI2S".
 - "8channels_ExpI2S" only differs from "8channels" regarding the
   configuration of input from the expansion port. It is configured to
   be used with https://github.com/MKSounds/ADAU1701-I2S-Audio-Driver-for-Raspberry-Pi
   The author uses https://www.musicpd.org/ on a Raspberry Pi Zero connected to the
   Aurora via the expansion port to free both USB ports for other uses.

The code contains two modes of changing presets (see if (false) in line 4303):
 (1) preset is changed at once (this behaviour is active in this commit)
 (2) preset is changed about two seconds after any encoder input. So one is
     able to move to any of the four presets without activating others while
     moving to it. An active preset is shown with an uppercase letter; the
     one which will be activated after encoder input is shown with a
     lowercase letter until it becomes active.
     
"8channels_ExpI2S" differs from "8channels" regarding settings of
SDATA_IN3 (Hardware Configuration/ADAU145x Register
Controls/SERIAL_PORTS/SDATA_IN3):

 - "MSB Position": "I2S - BCLK delay by 1"
 - "TDM Mode": "2 channels, 32bit"
 - "LRCLK type": "50/50 duty cycle clock"
 - "LRCLK Polarity": "Negative polarity"
 - "BCLK Polarity": "Positive polarity"
 
Connections:

|=======================================
| raspi  | aurora X102                
| Pin 12 | Pin 7, BCLK:3 -> BCLK_IN3   
| Pin 35 | Pin 9, LRCK:3 -> LRCLK_IN3    
| Pin 39 | Pins 2, 4, 6, .., 14, 16    
| Pin 40 | Pin 3, MDI:3  -> SDATA_IN3 
|=======================================

From /etc/mpd.conf:

  +--------------------------------------------------------
  | audio_output {
  |         type            "alsa"
  |         name            "ALSA I2S"
  |         device          "hw:0,0"        # optional
  |         format          "48000:24:*"
  |         mixer_type      "software"      # optional
  | }
  +--------------------------------------------------------
