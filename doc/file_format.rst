File Format
-----------

Toney works with the Praat TextGrid format.  It requires a "target" tier, and
optionally uses "frame" and "speaker" tiers.  Examples of the file format are
available for download.  The target tier contains transcriptions of the tokens
of interest.  Transcriptions consist of positional values separated by colon.
The first value represents the token and used by the tool to display it.  The
rest of the value positions are defined by the user.  For example, the second
position could be a tone category, the third, speaker gender, and so on. User
can select the value position from a tool menu, and the tool would create
clusters (categories) based on the values at the specified position.

For example, a target tier might contain the following sequence of word labels:
``muta paroma kidenu``.  In this case, the tool would create 3 items with no
class.  If the user created a cluster called LMH (low-mid-high) and added
``paroma`` to that cluster, then the TextGrid file would be modified to contain
``muta paroma:LMH kidenu``.

As another example, a target tier might contain the following sequence of
syllable labels: ``mu ta pa:L ro:L:male ma:H ki::female de nu``.  In this case,
the tool would create 8 items. Five syllables ``mu``, ``ta``, ``ki``, ``de``
and ``nu`` would be displayed with no cluster.  Two clusters would be created:
``L`` and ``H``. The ``L`` cluster would be populated by ``pa`` and ``ro``, and
the ``H``, by ``ma``. If the user created a new ``M`` category, and moved all 3
syllables in those clusters into this category, then the TextGrid file would be
modified to contain ``mu ta pa:M ro:M:male ma:M ki::female de nu``.

Continuing from the previous example, the user could select a new value
position from the ``Edit -> Value Position`` manu. The default value position
is 1 which means the second values of the colon separated transcripts. If the
user selected the value position 2, the tool would still display 8 items. Two
clusters would be created. Six syllables would be displayed with no cluster.
One syllable ``ro`` would be displayed inside the ``male`` cluster, and one
syllable ``ki`` would be displayed inside the ``female`` cluster.

The optional frame tier is used to delimit a larger span of recorded material
that contains the (sentence) frame in which the target form was elicited (e.g.
``F1`` for Frame 1).  The optional frame tier is used to provide a speaker
identifier, such as initials (e.g. ``PT``).  The ``View`` menu can be used to
toggle the display of this extra information.  Thus, a token ``paroma`` could
appear as ``paroma-F1-PT`` in the display, enabling the user to study
variability due to the elicitation frame or the speaker.

