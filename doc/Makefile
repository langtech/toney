QTBIN = /Users/haejoong/QtSDK/Desktop/Qt/4.8.1/gcc/bin
QHELPGENERATOR = $(QTBIN)/qhelpgenerator
QCOLLECTIONGEN = $(QTBIN)/qcollectiongenerator
RST2HTML = rst2html-2.7.py
RST = index.rst file_format.rst user_interface.rst license.rst credits.rst
CSS = html4css1.css

toney.qhc: toney.qhcp toney.qch
	$(QCOLLECTIONGEN) $< -o $@

toney.qch: toney.qhp $(RST:.rst=.html) $(CSS)
	$(QHELPGENERATOR) $< -o $@

%.html: %.rst
	$(RST2HTML) --stylesheet=$(CSS) --link-stylesheet $< > $@

clean:
	rm -rvf $(RST:.rst=.html)
	rm -rvf toney.qch toney.qhc
