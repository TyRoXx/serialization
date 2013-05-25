#!/usr/bin/env python

import uuid
import sys

count = 1
if len(sys.argv) > 1:
	count = int(sys.argv[1])

for i in range(count):
	print "_" + str(uuid.uuid4()).replace("-", "_").upper()
