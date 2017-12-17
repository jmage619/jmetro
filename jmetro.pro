QT += widgets

HEADERS = window.h
SOURCES = main.cpp window.cpp
LIBS += -ljack -lsndfile

binary.path = /usr/local/bin
binary.files = jmetro

data.path = /usr/local/share/jmetro
data.files = rimshot.wav

doc.path = /usr/local/share/doc/jmetro
doc.files = README LICENSE

INSTALLS += binary data doc
