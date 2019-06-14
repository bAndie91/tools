#!/usr/bin/env python

import smbpasswd
import sys

passwd = sys.argv[1]

print 'LANMAN hash is', smbpasswd.lmhash(passwd)               
print 'NT hash is', smbpasswd.nthash(passwd)

print 'both hashes at once = %s:%s (lm:nt)' % smbpasswd.hash(passwd)
