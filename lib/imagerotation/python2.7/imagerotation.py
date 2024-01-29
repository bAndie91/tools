#!/usr/bin/env python2
# -*- coding: utf-8 -*-

pyexiv2 = None
piexif = None

try:
	import pyexiv2
except ImportError:
	pass
else:
	if not hasattr(pyexiv2, 'ImageMetadata'):
		pyexiv2 = None

if pyexiv2 is None:
	try:
		import piexif
	except ImportError:
		pass
		
if pyexiv2 is None and piexif is None:
	raise ImportError("Neither pyexiv2 nor piexif module found.")

FLIP, FLOP, CLOCKWISE, COUNTERCLOCKWISE, UPSIDEDOWN = range(5)

class Image(object):
	def __init__(self, imagedata):
		self.imagedata = imagedata
	
	def rotate(self, callback, user_data):
		orientation = self.exif_orientation
		
		# flip-flop:
		if orientation in [2, 4]:
			# mirror on the vertical axis
			callback(FLIP, user_data)
			orientation -= 1
		elif orientation in [5, 7]:
			# mirror on the horizontal axis
			callback(FLOP, user_data)
			orientation += 1
		# rotation:
		rotation = None
		if orientation == 6:
			# rotate 90° CW
			callback(CLOCKWISE, user_data)
		elif orientation == 3:
			# rotate 180° CW
			callback(UPSIDEDOWN, user_data)
		elif orientation == 8:
			# rotate 270° CW
			callback(COUNTERCLOCKWISE, user_data)
	
	@property
	def exif_orientation(self):
		orientation = None
		if pyexiv2 is not None:
			metadata = pyexiv2.ImageMetadata.from_buffer(self.imagedata)
			metadata.read()
			if 'Exif.Image.Orientation' in metadata.exif_keys:
				orientation = metadata['Exif.Image.Orientation'].value
		elif piexif is not None:
			metadata = piexif.load(self.imagedata)
			for tagtype in metadata.keys():
				for tagid, value in metadata[tagtype].iteritems():
					try:
						if piexif.TAGS[tagtype][tagid]['name'] == 'Orientation':
							orientation = value
							break
					except KeyError:
						pass
				if orientation is not None:
					break
		return orientation
