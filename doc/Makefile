QHELPGENERATOR = /h/QtSDK/Desktop/Qt/4.8.1/gcc/bin/qhelpgenerator
RST = index.rst file_format.rst user_interface.rst license.rst
CSS = html4css1.css

toney.qch: toney.qhp $(RST:.rst=.html) $(CSS)
	$(QHELPGENERATOR) $< -o $@

%.html: %.rst
	rst2html --stylesheet=$(CSS) --link-stylesheet $< > $@

clean:
	rm -rvf $(RST:.rst=.html)
	rm -rvf toney.qch