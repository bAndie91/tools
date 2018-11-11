#! /usr/bin/env python2

from Crypto.Cipher import AES
from Crypto.Protocol.KDF import PBKDF2
import sys
import os
import binascii

# Function to get rid of padding
def clean_padding(x): 
    #return x[:-x[-1]].decode('utf8')
    return x[:-ord(x[-1])]

while True:
	record = sys.stdin.readline()
	if not record:
		break
	
	field_number_to_decrypt = int(sys.argv[1])
	fields = record.split()
	try:
		encrypted_value = binascii.unhexlify(fields[field_number_to_decrypt])
		
		# Trim off the 'v10' that Chrome/ium prepends
		encrypted_value = encrypted_value[3:]
		
		# Default values used by both Chrome and Chromium in OSX and Linux
		salt = b'saltysalt'
		iv = b' ' * 16
		length = 16
		
		# On Mac, replace MY_PASS with your password from Keychain
		# On Linux, replace MY_PASS with 'peanuts'
		my_pass = os.environ['CHROMIUM_DECRYPT_PASS']
		my_pass = my_pass.encode('utf8')
		
		# 1003 on Mac, 1 on Linux
		#iterations = 1003
		iterations = 1
		
		key = PBKDF2(my_pass, salt, length, iterations)
		cipher = AES.new(key, AES.MODE_CBC, IV=iv)
		
		decrypted = clean_padding(cipher.decrypt(encrypted_value))
		fields[field_number_to_decrypt] = decrypted
	except IndexError:
		pass
	print '\t'.join(fields)
