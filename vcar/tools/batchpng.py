#!/usr/bin/python

import os
import sys

for ppmname in sys.argv[1:] :
  pngname = ppmname.replace(".ppm",".png")
  cmd = "pnmtopng %s > %s" % (ppmname, pngname)
  os.system(cmd)

