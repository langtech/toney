User Interface
--------------

The user interface is divided into two regions.  The lower area is a canvas
containing unclassified linguistic forms.  You can double-click on an item to
hear it, and drag it so that it is closer to items that sound similar.  The
forms can be clustered according to any phonetic category of interest.

Once a putative category has been identified, use the ``edit`` menu to create a
cluster.  Give the cluster a label, and drag forms into it.  Create as many
clusters as desired.  Click on the small play button at the bottom of the
cluster to listen to all items in the cluster in sequence.  Any items that do
not belong in the cluster can be moved out of it.

The tool allows multiple catetory types. Each catetory type corresponds to a
position in a colon-delimited value list explained in the File Format section.
For example, value position 1 can be tone category, and value position 2 can
be speaker gender. Value position is displayed on the top right corner of the
tool and can be changed from the Edit menu. When the value position is changed,
the clusters will be recreated based on the catetories in the category type.

If the forms were collected using an elicitation frame, where these are
indicated using the "frame" tier in the TextGrid file, then the display of
frame labels can be turned on and off using the ``view`` menu.  If a speaker
identifier is indicated using the "speaker" tier in the TextGrid file, then
this can also be controlled using the ``view`` menu.  To listen to forms with
the frame, use right-click (on the canvas) or the large play button (at the
bottom of the cluster box).

To delete a cluster and return any items it contains to the canvas, click on
the ``X`` button.

Click on the F0 tab to see the F0 traces for the items in the cluster.  When
switching between the ``label`` and ``F0`` tab, the highlighted form stays the
same.  Thus, any outlying F0 trace can be easily associated with the
corresponding form, and it can be moved out of the cluster.

How F0 traces are generated is controlled by a set of parameters. These
parameters can be adjusted from the Edit -> F0 Parameters menu. Move mouse over
a parameter label, and an explanation of the parameter will be displayed.

Click on the border area of a cluster to pick up the cluster and drag it to a
new position.

The amplitude widget on the top right corner of the tool allows to change the
aplitude of the loaded waveforms. However, it doesn't change the global audio
volume.

