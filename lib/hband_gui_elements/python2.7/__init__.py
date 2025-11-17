
import os
import re
import glob
import glib
import gtk
import xdg.BaseDirectory
import traceback
import ConfigParser
from StringIO import StringIO
import __main__


def icondir2size(path):
	match = re.findall(r'/([0-9]+)x[0-9]+', path)
	if not match: return -1
	return match[0]

def find_icon_file(iconname, preferred_size_px):
	dirs = []
	for categ in 'hicolor', 'locolor', 'gnome':
		directory = os.path.join('/usr/share/icons', categ)
		small = []
		for icondir in sorted(glob.glob(directory+'/*x*/*/'), key = icondir2size):
			if icondir2size(icondir) < preferred_size_px:
				small.insert(0, icondir)
			else:
				dirs.append(icondir)
		for icondir in glob.glob(directory+'/scalable/*/'):
			dirs.append(icondir)
		dirs.extend(small)
	dirs.append('/usr/share/pixmaps/')
	
	for icondir in dirs:
		iconpath_test = None
		for ext in 'png', 'xpm', 'svg':
			if iconname.find(os.path.extsep) != -1:
				iconpath_test = os.path.join(icondir, iconname)
			else:
				iconpath_test = os.path.join(icondir, iconname) + os.path.extsep + ext
			if os.path.exists(iconpath_test):
				return iconpath_test
			if iconname.find(os.path.extsep) != -1:
				break

def get_pixbuf_from_file_at_size(filepath, width_px, height_px):
	pixbuf = gtk.gdk.pixbuf_new_from_file(filepath)
	w = pixbuf.get_width()
	h = pixbuf.get_height()
	if width_px != w or height_px != h:
		scale = min(float(width_px)/w, float(height_px)/h)
		resized_w = int(w * scale)
		resized_h = int(h * scale)
		pixbuf = pixbuf.scale_simple(resized_w, resized_h, gtk.gdk.INTERP_HYPER)
	return pixbuf

def configured_object(obj, configsteps):
	for step in configsteps:
		if callable(step):
			step(obj)
		else:
			methodname, args = step[:]
			method = getattr(obj, methodname)
			method(*args)
	return obj


class Window(gtk.Window):
	def __init__(self, *args):
		super(Window, self).__init__(*args)
		self.property_persistor = PropertyPersistor(self, 'window.main', [
			[ 'position', lambda obj, evt: obj.get_position() ],
		])
	
	@property
	def title(self):
		self.get_title()
	
	@title.setter
	def title(self, title):
		self.set_title(title)

class Scrollable(gtk.ScrolledWindow):
	def __init__(self, child = None):
		super(Scrollable, self).__init__()
		self.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
		if child is not None:
			self.add(child)

class PropertyPersistor(object):
	def __init__(self, obj, obj_name, props):
		assert hasattr(__main__, 'APPNAME') and isinstance(__main__.APPNAME, basestring) and __main__.APPNAME, "Set APPNAME global variable."
		self.obj = obj
		self.obj_name = obj_name
		self.prop = {}
		
		for notify_prop, getter in props:
			obj.connect('notify::'+notify_prop, self.on_prop_change, notify_prop, getter)
	
	def on_prop_change(self, widget, event, notify_prop, getter):
		value = getter(widget, event)
		self.prop[notify_prop] = value
		self.save()
	
	def save(self):
		ini = IniFile()
		for prop, value in self.prop:
			ini[self.obj_name][prop] = value
		
		try:
			filepath = os.path.join(xdg.BaseDirectory.save_config_path(__main__.APPNAME), 'auto-properties.ini')
			dirpath = os.path.dirname(filepath)
			if not os.path.exists(dirpath):
				os.makedirs(dirpath)
			with open(filepath, 'w') as f:
				f.write(str(ini))
		except:
			traceback.print_exc()

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
		for section in self.cp.sections():
			yield (section, IniFileSection(self, section))
	def __setitem__(self, section_name, section):
		if not self.cp.has_section(section_name):
			self.cp.add_section(section_name)
		for key, value in section.iteritems():
			self[section_name][key] = value
	def __getitem__(self, section):
		return IniFileSection(self, section)
	def __delitem__(self, section):
		# TODO
		pass
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
	
	def __init__(self, inifile, section):
		self.inifile = inifile
		self.section = section
	def __iter__(self):
		for item in self.iteritems():
			yield item
	def iteritems(self):
		return self.inifile.cp.items(self.section)
	def __getitem__(self, key, type=None):
		if self.inifile.cp.has_option(self.section, key):
			if type == self.BOOL:
				return self.inifile.cp.getboolean(self.section, key)
			elif type == self.INT:
				return self.inifile.cp.getint(self.section, key)
			elif type == self.FLOAT:
				return self.inifile.cp.getfloat(self.section, key)
			else:
				return self.inifile.cp.get(self.section, key)
		else:
			raise KeyError(key)
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
	
	def __setitem__(self, key, value, type=None):
		if not self.inifile.cp.has_section(self.section):
			self.inifile.cp.add_section(self.section)
		self.inifile.cp.set(self.section, key, value)
	def __delitem__(self, key):
		# TODO
		pass
	def __repr__(self):
		return '<%s section=%s>' % (self.__class__.__name__, self.section)
	def __str__(self):
		tmp = IniFile()
		tmp[self.section] = self
		return str(tmp)
