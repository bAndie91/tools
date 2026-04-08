
import os
import re
import glob
import glib
import gtk
import gobject
import xdg.BaseDirectory
import fcntl
import traceback
from .files import *
import __main__


MOUSE_BUTTON_LEFT = 1
MOUSE_BUTTON_MIDDLE = 2
MOUSE_BUTTON_RIGHT = 3

def icondir2size(path):
	match = re.findall(r'/([0-9]+)x[0-9]+', path)
	if not match: return -1
	return match[0]

def find_icon_file(iconname, preferred_size_px):
	dirs = []
	for categ in gtk.settings_get_default().props.gtk_icon_theme_name, 'hicolor', 'locolor', 'gnome':
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


class Label(gtk.Label):
	def __init__(self, *pargs, **kwargs):
		gtk_kwargs = {}
		if 'str' in kwargs: gtk_kwargs['str'] = kwargs['str']
		super(Label, self).__init__(*pargs, **gtk_kwargs)
		if 'markup' in kwargs:
			self.set_markup(kwargs['markup'])
			self.set_use_markup(True)
		self.set_alignment(0, 0)

class Image(gtk.Image):
	def __init__(self, *pargs, **kwargs):
		super(Image, self).__init__(*pargs, **kwargs)
		self._icon_size = None
	@property
	def icon_size(self):
		return self._icon_size
	@icon_size.setter
	def icon_size(self, new):
		self._icon_size = new  # TODO resize already loaded icon
	@property
	def icon(self):
		return self._icon
	@icon.setter
	def icon(self, icon):
		if isinstance(icon, tuple):
			icon, self._icon_size = icon[:]
		
		if icon.find(os.path.sep)>=0 and os.path.isfile(icon):
			if self._icon_size is not None:
				width_px, height_px = gtk.icon_size_lookup(self._icon_size)
				pixbuf = get_pixbuf_from_file_at_size(icon, width_px, height_px)
				self.set_from_pixbuf(pixbuf)
			else:
				self.set_from_file(icon)
		elif icon in gtk.stock_list_ids():
			self.set_from_stock(icon, self._icon_size)
		elif gtk.icon_theme_get_default().lookup_icon(icon, self._icon_size, 0) is not None:
			self.set_from_icon_name(icon, self._icon_size)
		else:
			width_px, height_px = gtk.icon_size_lookup(self._icon_size)
			iconfile = find_icon_file(icon, width_px)
			if iconfile is not None:
				pixbuf = get_pixbuf_from_file_at_size(iconfile, width_px, height_px)
				self.set_from_pixbuf(pixbuf)
				self._icon_file = iconfile
		self._icon = icon
		return self

class Window(gtk.Window):
	def __init__(self, opt):
		super(Window, self).__init__(*opt.get('gtk-args', []))
		if opt.get('default-size'):
			self.set_default_size(*opt.get('default-size'))
		self.property_persistor = PropertyPersistor(self, opt.get('name', self.get_name()), [
			PropertyPersistor.PERSIST_PROPERTY_GEOMETRY,
			PropertyPersistor.PERSIST_PROPERTY_POSITION,
		])
		self.property_persistor.apply_saved_properties()
	
	def show(self, show_all=False):
		pos = self.property_persistor.props.get('position')
		getattr(super(Window, self), 'show_all' if show_all else 'show')()
		if pos is not None: self.move(*pos)
	
	def show_all(self):
		self.show(show_all=True)
	
	@property
	def title(self):
		self.get_title()
	
	@title.setter
	def title(self, title):
		self.set_title(title)
	
	def set_icon_from_any(self, icon):
		if icon is None:
			self.set_icon(None)
		else:
			icon_size = gtk.icon_size_lookup(gtk.ICON_SIZE_LARGE_TOOLBAR)[0]
			iconfile = find_icon_file(icon, icon_size)
			if iconfile is None:
				self.set_icon(None)
			else:
				self.set_icon(gtk.gdk.pixbuf_new_from_file(iconfile))

class Scrollable(gtk.ScrolledWindow):
	def __init__(self, child = None):
		super(Scrollable, self).__init__()
		self.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
		if child is not None:
			if gobject.signal_lookup('set-scroll-adjustments', type(child)):
				self.add(child)
			else:
				self.add_with_viewport(child)

class PropertyPersistor(object):
	PERSIST_PROPERTY_GEOMETRY = ( 'geometry', 'configure-event', lambda wdg, evt: [evt.width, evt.height], lambda wdg, value: wdg.set_default_size(*value) )
	PERSIST_PROPERTY_POSITION = ( 'position', 'configure-event', lambda wdg, evt: wdg.get_position()[:],   lambda wdg, value: wdg.move(*value) )

	def __init__(self, obj, obj_name, property_descriptors):
		assert hasattr(__main__, 'APPNAME') and isinstance(__main__.APPNAME, basestring) and __main__.APPNAME, "Set APPNAME global variable."
		self.obj = obj
		self.obj_name = obj_name
		self._filepath = os.path.join(xdg.BaseDirectory.save_config_path(__main__.APPNAME), 'auto-properties.ini')
		self._inifile = None
		self.triggers = {}
		self.applicators = {}
		self.active = True
		self.currently_applicating = False
		self.add_properties(property_descriptors)
	
	def add_properties(self, property_descriptors):
		for prop_name, trigger_signal, getter, applicator in property_descriptors:
			if trigger_signal not in self.triggers:
				self.triggers[trigger_signal] = []
				self.obj.connect(trigger_signal, self.on_trigger, trigger_signal)
			self.triggers[trigger_signal].append((prop_name, getter))
			if applicator is not None:
				self.applicators[prop_name] = applicator
	
	def on_trigger(self, widget, *cb_args):
		if not self.active: return
		if self.currently_applicating: return
		signal_args = cb_args[0:-1]
		trigger_signal = cb_args[-1]
		changed_props = {}
		for prop_name, getter in self.triggers[trigger_signal]:
			value = getter(widget, *signal_args)
			changed_props[prop_name] = value
		self.persist(changed_props)
		# TODO add timer to better schedule writing to disk
	
	def persist(self, properties):
		dirpath = os.path.dirname(self._filepath)
		if not os.path.exists(dirpath):
			os.makedirs(dirpath)
		
		with openfile(self._filepath, os.O_RDWR | os.O_CREAT, 'r+') as fh:
			fcntl.flock(fh, fcntl.LOCK_EX)
			self._inifile = IniFile(fh)
			before = str(self._inifile)
			self._inifile[self.obj_name].update(properties)
			after = str(self._inifile)
			
			if before != after:
				try:
					fh.seek(0)
					fh.write(str(self._inifile))
					fh.truncate()
				except:
					traceback.print_exc()
	
	def load_inifile(self):
		self._inifile = IniFile(self._filepath)
	
	@property
	def props(self):
		if self._inifile is None: self.load_inifile()
		return self._inifile[self.obj_name]
		# TODO combine ini files with less specific appname to fall back to the app's base instance properties if this app instance does not have a certain prop
	
	def apply_saved_properties(self):
		self.currently_applicating = True
		for prop_name, value in self.props.iteritems():
			if prop_name in self.applicators:
				try:
					self.applicators[prop_name](self.obj, value)
				except:
					traceback.print_exc()
		self.currently_applicating = False
