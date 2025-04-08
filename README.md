# Arduino Piano Player with Sound Spectrum Visualizer

#### Project Description:
This project is an interactive Arduino-based piano that plays musical notes and a song stored on an SD card. It also includes a sound spectrum visualizer using an LED matrix, which reacts to the audio input from the piano or the music.  The project combines hardware components like a multiplexer, sound sensor, and LCD display with software for real-time audio processing.

#### Main Features:
\- Piano Mode: Press buttons to play different musical notes and the LCD shows the note name.

\- Music Player: Play/pause songs stored on an SD card with a button and the LCD displays the status.

\- Sound Visualizer: The LED matrix shows the audio frequency spectrum based on the sound sensor input (currently under refinement).

#### Current Status:
  &nbsp;&nbsp;&nbsp;&nbsp;The base functions of the project, including the Piano Mode and Music Player, are fully operational. In Piano Mode, the buttons correctly play notes, and the music player can play/pause songs stored on the SD card.
  
  &nbsp;&nbsp;&nbsp;&nbsp; However, the LED matrix isn’t yet displaying frequencies correctly because it’s connected to the wrong pins (2, 7, 8) instead of the required SPI pins (10–13), which are already used by the SD card. This causes communication conflicts. The fix involves assigning unique Chip Select (CS) pins to each device to avoid SPI communication interference. 
I’m actively working on this improvement.

#### Demo:


https://github.com/user-attachments/assets/b58b066a-2c41-477c-bfc3-ea0d24e2e1ba

A better quality video demo has been uploaded in the repository.

#### Components:
Arduino Uno, 10 buttons (for notes and play/pause), SD card module, 1602 LCD, 8x8 LED matrix, Sound sensor module, Speaker, multiplexer, 2 breadboards, wires, resistors

#### Working principle:
The buttons send signals to the Arduino to play notes or control music playback.
A sound sensor detects audio input, and the LED matrix visualizes the sound spectrum.
The SD card module stores the music file that can be played through the system.

#### Documentation:
Detailed setup, circuit diagrams, and code explanations are in the attached documentation.
