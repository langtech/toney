Tones is a GUI tool for sorting audio clips into different categories.

The input to the tool is an audio recording and a TextGrid file defining the
start and end time of each audio clip within the audio recording. TextGrid
file can be created using Praat [1]. The minimal requirement for the TextGrid
file is a tier called "target". Optionally, "frame" tier can be added to
provide context for the target sound. The labels for the target should be of
the following form: LABEL:CATEGORY. The LABEL and CATEGORY can be any string.
For labels that have no category, "X" should be used. Labels not matching the
label format are not loaded into the tool.

This version requires an external program for playing audio: play. This is a
command line utility that comes with SoX [2]. Install SoX and make sure that
the play command is available, i.e. make sure that it exists under /usr/bin or
/usr/local/bin directory.

To start the tool, run the Tones.sh program.

This package was compiled on openSUSE 11.4 (32 bit), and tested on a version of
Ubuntu and Debian (both 32 bit).

The source code of this program can be found in the Google Code web site:

  http://code.google.com/p/prosody/source/browse/tone_classifier/Tones/

For latest version of this package, check out the following web site:

  http://code.google.com/p/prosody/downloads/list
  
[1] http://www.fon.hum.uva.nl/praat/
[2] SoX manual page: http://sox.sourceforge.net/sox.html

