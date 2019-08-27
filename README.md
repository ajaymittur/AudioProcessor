# AudioProcessor
Basic program to change the **pitch** of a **WAV** audio file as required by the user.

# Library 
- Simple Direct Media Layer (SDL)
  - Inbuilt wav functions used to load audio data and devices.

# Audio Data Manipulation
Done by `void ChangePitch(void *userdata, Uint8 *stream, Uint32 length)`
 - User defined function used to manipulate the pitch of the audio and write to output stream of the audio device.
 - `userdata` cast into user defined structure `AudioData` which allows storage of required data such as position of current sample, desired pitch and total length of the audio.
 - `volume` assumed to be 100, but can be taken as user input also by adding a volume member to `AudioData`.
 - pitch is changed in the for loop and data from audio sample is copied to the output stream.
