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
		self._tags = None
		self._thumbnail = None
	
	def rotate(self, callback, user_data):
		orientation = self.orientation
		
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
	
	def _load_by_pyexiv2(self):
		metadata = pyexiv2.ImageMetadata.from_buffer(self.imagedata)
		metadata.read()
		return metadata
	
	def _load_by_piexif(self):
		return piexif.load(self.imagedata)
	
	@property
	def tags(self):
		if self._tags is None:
			self._tags = {}
			if pyexiv2 is not None:
				metadata = self._load_by_pyexiv2()
				for key, tag in metadata.items():
					if hasattr(tag, 'human_value'):
						v = tag.human_value
					elif hasattr(tag, 'values'):
						v = '\n'.join(tag.values)
					else:
						v = tag.value
					self._tags[key] = v
					self._tags[tag.name] = v
			elif piexif is not None:
				metadata = self._load_by_piexif()
				for tagtype in metadata.keys():
					if hasattr(metadata[tagtype], 'iteritems'):
						for tagid, value in metadata[tagtype].iteritems():
							self._tags[tagid] = value
							try:
								self._tags[ piexif.TAGS[tagtype][tagid]['name'] ] = value
							except KeyError:
								pass
		return self._tags
	
	@property
	def orientation(self):
		return self.tags.get('Exif.Image.Orientation', self.tags.get('Orientation'))
	
	@property
	def comment(self):
		txt = None
		if pyexiv2 is not None:
			metadata = self._load_by_pyexiv2()
			try:
				txt = metadata.comment
			except:
				pass
		else:
			# resort to external command
			import subprocess
			p = subprocess.Popen(["exiftool", "-b", "-Comment", "-"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=False)
			stdout, stderr = p.communicate(self.imagedata)
			if p.returncode == 0:
				txt = stdout
		return txt
	
	@property
	def thumbnail(self):
		if self._thumbnail is None:
			if pyexiv2 is not None:
				metadata = self._load_by_pyexiv2()
				tn = metadata.exif_thumbnail
				if tn.data:
					self._thumbnail = tn.data
			elif piexif is not None:
				metadata = self._load_by_piexif()
				self._thumbnail = metadata.get('thumbnail')
		return self._thumbnail
