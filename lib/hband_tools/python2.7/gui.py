
import os
import re
import glob
import glib
import gtk
import xdg.BaseDirectory
import traceback
from .files import *
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
		property_persistor = PropertyPersistor(self, 'window.main', [
			( 'geometry', 'configure-event', lambda wdg, evt: [evt.width, evt.height], lambda wdg, value: wdg.set_default_size(*value) ),
			( 'position', 'configure-event', lambda wdg, evt: wdg.get_position()[:],   lambda wdg, value: wdg.move(*value) ),
		])
		property_persistor.apply_saved_properties()
	
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
	def __init__(self, obj, obj_name, property_descriptors):
		assert hasattr(__main__, 'APPNAME') and isinstance(__main__.APPNAME, basestring) and __main__.APPNAME, "Set APPNAME global variable."
		self.obj = obj
		self.obj_name = obj_name
		self._filepath = os.path.join(xdg.BaseDirectory.save_config_path(__main__.APPNAME), 'auto-properties.ini')
		self._inifile = None
		self.triggers = {}
		self.applicators = {}
		
		for prop_name, trigger_signal, getter, applicator in property_descriptors:
			if trigger_signal not in self.triggers:
				self.triggers[trigger_signal] = []
				obj.connect(trigger_signal, self.on_trigger, trigger_signal)
			self.triggers[trigger_signal].append((prop_name, getter))
			self.applicators[prop_name] = applicator
	
	def on_trigger(self, widget, event, trigger_signal):
		changed_props = {}
		for prop_name, getter in self.triggers[trigger_signal]:
			value = getter(widget, event)
			changed_props[prop_name] = value
		self.persist(changed_props)
		# TODO add timer to better schedule writing to disk
	
	def load_inifile(self):
		self._inifile = IniFile(self._filepath)
	
	def persist(self, properties):
		self.load_inifile()
		self._inifile[self.obj_name].update(properties)
		
		try:
			dirpath = os.path.dirname(self._filepath)
			if not os.path.exists(dirpath):
				os.makedirs(dirpath)
			with open(self._filepath, 'w') as f:
				f.write(str(self._inifile))
		except:
			traceback.print_exc()
	
	@property
	def props(self):
		if self._inifile is None: self.load_inifile()
		return self._inifile[self.obj_name]
		# TODO combine ini files with less specific appname to fall back to the app's base instance properties if this app instance does not have a certain prop
	
	def apply_saved_properties(self):
		for prop_name, value in self.props.iteritems():
			if prop_name in self.applicators:
				try:
					self.applicators[prop_name](self.obj, value)
				except:
					traceback.print_exc()
