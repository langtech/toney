QHELPGENERATOR = /h/QtSDK/Desktop/Qt/4.8.1/gcc/bin/qhelpgenerator
QCOLLECTIONGEN = /h/QtSDK/Desktop/Qt/4.8.1/gcc/bin/qcollectiongenerator
RST = index.rst file_format.rst user_interface.rst license.rst credits.rst
CSS = html4css1.css

toney.qhc: toney.qhcp toney.qch
	$(QCOLLECTIONGEN) $< -o $@

toney.qch: toney.qhp $(RST:.rst=.html) $(CSS)
	$(QHELPGENERATOR) $< -o $@

%.html: %.rst
	rst2html --stylesheet=$(CSS) --link-stylesheet $< > $@

clean:
	rm -rvf $(RST:.rst=.html)
	rm -rvf toney.qch toney.qhc
