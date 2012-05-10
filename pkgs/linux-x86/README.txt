To start the tool, run the Toney.sh program.

This version requires an external program for playing audio: play. This is a
command line utility that comes with sox [1]. Install sox and make sure that
the play command is available, i.e. make sure that it exists under /usr/bin or
/usr/local/bin directory.

We noticed that on some systems the first part of sound is chopped off
depending on the audio system that the OS is using. A solution we found was to
set the AUDIODRIVER environment variable, e.g. "export AUDIODRIVER=oss".
See man pages for sox or play for details.

More documentation is available from the Help menu of the tool.

The source code for this program is avialable at github:

  https://github.com/langtech/toney

[1] SoX manual page: http://sox.sourceforge.net/sox.html

