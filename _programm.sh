#!/bin/bash
avrdude -p attiny13 -c usbasp -U flash:w:main.hex:i -F -P usb
