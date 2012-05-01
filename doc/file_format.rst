File Format
-----------

Toney works with the Praat TextGrid format.  It requires a "target" tier, and optionally uses
"frame" and "speaker" tiers.  Examples of the file format are available for download.
The target tier contains transcriptions of the tokens of interest.
Transcriptions consist of a segmental component and a category ("X" for unknown), separated by colon.
Toney only considers the labels that contain this colon; other labels are ignored.

For example, a target tier might contain the following sequence of word labels: ``muta paroma:X kidenu``.
In this case, only the second item ``paroma`` would be displayed.  If the user created a
class called LMH (low-mid-high), then the TextGrid file would be modified to contain
``muta paroma:LMH kidenu``.

As another example, a target tier might contain the following sequence of syllable labels: ``mu ta pa:L ro:X ma:H ki de nu``.
In this case, only the sylables ``pa``, ``ro``, and ``ma`` would be displayed.
Two categories ``L`` and ``H`` would be populated with ``pa`` and ``ma`` respectively.
However, ``ro`` would be displayed without any category.  If the user created a new ``M`` category,
and moved all three syllables into this category, then the TextGrid file would be
modified to contain ``mu ta pa:M ro:M ma:M ki de nu``.

The optional frame tier is used to delimit a larger span of recorded material that contains the
(sentence) frame in which the target form was elicited (e.g. ``F1`` for Frame 1).
The optional frame tier is used to provide a speaker identifier, such as initials (e.g. ``PT``).
The ``View`` menu can be used to toggle the display of this extra information.
Thus, a token ``paroma`` could appear as ``paroma-F1-PT`` in the display,
enabling the user to study variability due to the elicitation frame or the speaker.
