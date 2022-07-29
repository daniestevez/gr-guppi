title: GUPPI raw files handling
brief: gr-guppi includes blocks to handle GUPPI raw files used in radioastronomy/SETI
tags: # Tags are arbitrary, but look at CGRAN what other authors are using
  - radioastronomy
  - seti
  - guppi
  - ata
author:
  - Daniel Estevez <daniel@destevez.net>
copyright_owner:
  - Daniel Estevez <daniel@destevez.net>
license:
gr_supported_version: 3.10
repo: https://github.com/daniestevez/gr-guppi
#website: <module_website> # If you have a separate project website, put it here
#icon: <icon_url> # Put a URL to a square image here that will be used as an icon on CGRAN
---
GUPPI raw files are often used in radioastronomy/SETI to store the channelized
IQ samples output by a polyphase filter bank. This is the typical IQ (voltage
mode, in radioastronomy terms) output for many modern telescopes, since they use
a polyphase filterbank as part of their DSP, so raw ADC samples are often not
available.

gr-guppi contains a GUPPI File Sink block that can write GUPPI files. A GUPPI
File Source block able to read GUPPI files might be added in the future.
