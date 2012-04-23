Tones is a GUI tool for sorting audio clips into different categories.

The input to the tool is an audio recording and a TextGrid file defining the
start and end time of each audio clip within the audio recording. TextGrid
file can be created using Praat [1]. The minimal requirement for the TextGrid
file is a tier called "target". Optionally, "frame" tier can be added to
provide context for the target sound. The labels for the target should be of
the following form: LABEL:CATEGORY. The LABEL and CATEGORY can be any string.
For labels that have no category, "X" should be used. Labels not matching the
label format are not loaded into the tool.

To start the tool, simply open the Tones.exe program. This program depends on
some DLL files installed on your computer. These files are provided by
Microsoft and, in many cases, installed by other applications you have
installed on your computer. If those files are missing, the tool will complain
about missing DLLs. If you get such an error message, run vcredist_x86.exe
program included in this package to install the DLLs.

This package was compiled using 32 bit Windows 7. It was tested on a 32 bit
Windows XP computer.

The source code of this program can be found in the Google Code web site:

  http://code.google.com/p/prosody/source/browse/tone_classifier/Tones/

For latest version of this package, check out the following web site:

  http://code.google.com/p/prosody/downloads/list
  
[1] http://www.fon.hum.uva.nl/praat/