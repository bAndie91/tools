
import ConfigParser
from StringIO import StringIO
import json


class IniFile(dict):
	def __init__(self, filepath=None):
		self.cp = ConfigParser.ConfigParser()
		if filepath is not None:
			try:
				self.cp.read(filepath)
			except:
				# don't fail if file not found, just reflect emptiness.
				pass
	def __iter__(self):
		for item in self.iteritems():
			yield item[0]
	def iteritems(self):
		for section in self.cp.sections():
			yield (section, IniFileSection(self, section))
	def __getitem__(self, section_name):
		return IniFileSection(self, str(section_name))
	def __setitem__(self, section_name, section_dict):
		self.cp.remove_section(section_name)
		for key, value in section_dict.iteritems():
			self[section_name][key] = value
	def update(self, dictionary):
		for section_name, section_dict in dictionary.iteritems():
			self[section_name].update(section_dict)
	def __delitem__(self, section_name):
		self.cp.remove_section(str(section_name))
	def __str__(self):
		buf = StringIO()
		self.cp.write(buf)
		buf.seek(0)
		return buf.read()

class IniFileSection(dict):
	STRING = 0
	BOOL = 1
	INT = 2
	FLOAT = 3
	JSON = 4
	
	def __init__(self, inifile, section):
		self.inifile = inifile
		self.section = str(section)
	def __iter__(self):
		for item in self.iteritems():
			yield item[0]
	def iteritems(self):
		if self.inifile.cp.has_section(self.section):
			for key in self.inifile.cp.options(self.section):
				yield (key, self[key])
	def __getitem__(self, key, type=None):
		key = str(key)
		if self.inifile.cp.has_option(self.section, key):
			if type == self.BOOL:
				return self.inifile.cp.getboolean(self.section, key, raw=True)
			elif type == self.INT:
				return self.inifile.cp.getint(self.section, key, raw=True)
			elif type == self.FLOAT:
				return self.inifile.cp.getfloat(self.section, key, raw=True)
			elif type == self.STRING:
				return self.inifile.cp.get(self.section, key, raw=True)
			else:
				return json.loads(self.inifile.cp.get(self.section, key, raw=True))
		else:
			return None
	def get(self, key, default=None, type=None):
		try:
			return self.__getitem__(key, type)
		except KeyError:
			return default
	def getstr(self, key):
		return self.__getitem__(key, self.STRING)
	def getbool(self, key):
		return self.__getitem__(key, self.BOOL)
	def getint(self, key):
		return self.__getitem__(key, self.INT)
	def getfoat(self, key):
		return self.__getitem__(key, self.FLOAT)
	
	def keys(self):
		return self.inifile.cp.options(self.section)
	def values(self):
		return [value for key, value in self.iteritems()]
	
	def __setitem__(self, key, value, type=None):
		if not self.inifile.cp.has_section(self.section):
			self.inifile.cp.add_section(self.section)
		self.inifile.cp.set(self.section, str(key), json.dumps(value))
	def update(self, dictionary):
		for key, value in dictionary.iteritems():
			self[key] = value
	def __delitem__(self, key):
		if self.inifile.cp.has_section(self.section):
			self.inifile.cp.remove_option(self.section, str(key))
	
	def __repr__(self):
		return '<%s section=%s>' % (self.__class__.__name__, repr(self.section))
	def __str__(self):
		tmp = IniFile()
		tmp[self.section] = self
		return str(tmp)
