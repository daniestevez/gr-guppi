#
# Copyright 2022 Daniel Estevez <daniel@destevez.net>
#
# This file is part of gr-guppi
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

'''
gr-guppi

GUPPI raw files are often used in radioastronomy/SETI to store the channelized
IQ samples output by a polyphase filterbank. This is the typical IQ (voltage
mode, in radioastronomy terms) output for many modern telescopes, since they use
a polyphase filterbank as part of their DSP, so raw ADC samples are often not
available.

gr-guppi contains a GUPPI File Sink block that can write GUPPI files. A GUPPI
File Source block able to read GUPPI files might be added in the future.
'''

# import pybind11 generated symbols into the guppi namespace
from .guppi_python import *

# import any pure python here
#
