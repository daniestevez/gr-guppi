# gr-guppi

GUPPI raw files are often used in radioastronomy/SETI to store the channelized
IQ samples output by a polyphase filterbank. This is the typical IQ (voltage
mode, in radioastronomy terms) output for many modern telescopes, since they use
a polyphase filterbank as part of their DSP, so raw ADC samples are often not
available.

gr-guppi contains a GUPPI File Sink block that can write GUPPI files. A GUPPI
File Source block able to read GUPPI files might be added in the future.

## Installation

gr-guppi requires GNU Radio 3.10. It can be built from source and installed as
any other GNU Radio out-of-tree module:

```
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

## Example flowgraph

An example flowgraph showing how to build a polyphase filterbank that supplies
data to a GUPPI File Sink block is given in the `examples` folder. This
flowgraph generates a signal that consists of a chirp sweeping the central
channels plus AWGN, passes the signal through a polyphase filterbank such as the
one used in telescopes, and writes the results to a GUPPI file.

The output GUPPI file can then be processed by typical radioastronomy tools such
as [rawspec](https://github.com/UCBerkeleySETI/rawspec) and `watutil` from
[blimpy](https://github.com/UCBerkeleySETI/blimpy). After running the example
flowgraph for some time, run the following to view the spectrum and waterfall of
the GUPPI file.

```
rawspec -j -f 64 -t 100 guppi_sink_example
watutil -p ank guppi_sink_example.rawspec.0000.h5
```
