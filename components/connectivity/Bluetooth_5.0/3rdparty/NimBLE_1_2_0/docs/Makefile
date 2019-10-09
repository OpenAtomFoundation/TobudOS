# Make a preview site for Sphinx & Doxygen output

SPHINXOPTS    =
SPHINXBUILD   = sphinx-build
SPHINXPROJ    = Mynewt
SOURCEDIR     = .
BUILDDIR      = _build/sphinx

.PHONY: Makefile clean preview doxygen

clean:
	rm -rf _build

preview: _build doxygen sphinx

_build:
	mkdir -p _build

doxygen:
	mkdir -p _build/html
	cd .. && doxygen docs/doxygen.xml

sphinx:
	sphinx-build . _build/sphinx
	mv _build/sphinx _build/html/documentation
