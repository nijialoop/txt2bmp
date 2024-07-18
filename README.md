# txt2bmp
This tool can convert UTF-8 encoded Asian strings to BMP bitmaps

The project will use support for the freetype library, please install it in the corresponding library file directory.
The repository contains the free-type x86/ARM library files that have been compiled.
Please read the Makefile in detail and make corrections according to the directory in which you built your environment.

Currently supports custom Asian character library, Latin character library, as well as the definition of picture length width, word spacing, foreground, background color.
The input string can be in the UTF-8 character set. Unicode encoding
