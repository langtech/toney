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

Click on the border area of a cluster to pick up the cluster and drag it to a
new position.

There might be different classes of categories, e.g. tone categories and
speaker genders. The user can assign a numeric index, called *value position*,
to each class, e.g. 1 for tone categories and 2 for speaker genders. This
numeric index is displayed at the top left coner of the tool, and this can be
changed using the ``Edit -> Value Position`` menu. For example, if it said
``Value Position: 1``, the forms would need to be clustered into tone
categories. Toney remembers current clustering, so if the user switched to a
different value position and changed it back to current position, then the
original clustering would still be displayed.

