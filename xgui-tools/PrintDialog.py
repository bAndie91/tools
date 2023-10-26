#!/usr/bin/python

import sys
import os
import gtk


def do_print(prn_op, prn_ctx):
	pass

def do_page(prn_op, prn_ctx, page_nr):
	print "page", page_nr
	
	cc = context.get_cairo_context()
	width = context.get_width()
	cr.rectangle(0, 0, width, HEADER_HEIGHT)
	cr.set_source_rgb(0.8, 0.8, 0.8)
	cr.fill()
	layout = context.create_pango_layout()
	desc = pango.FontDescription("sans 14")
	layout.set_font_description(desc)
	layout.set_text("some text")
	layout.set_width(width)
	layout.set_alignment(pango.ALIGN_CENTER)
	x,layout_height = layout.get_size()
	text_height = layout_height / pango.SCALE
	cr.move_to(width / 2,  (HEADER_HEIGHT - text_height) / 2)
	cr.show_layout(layout)


Glob = {
	'print_settings': None,
}

def show_print_dialog():
	prn_op = gtk.PrintOperationPreview()
	
	if len(sys.argv) > 1:
		jobname = os.path.basename(sys.argv[1])
		prn_op.set_job_name(jobname)
		prn_op.set_export_filename(jobname)
	
	prn_op.set_track_print_status(True)
	prn_op.set_show_progress(True)
	if Glob['print_settings'] is not None:
		prn_op.set_print_settings(Glob['print_settings'])
	
	prn_op.connect('begin-print', do_print)
	prn_op.connect('draw-page', do_page)
	
	resp = prn_op.run(gtk.PRINT_OPERATION_ACTION_PRINT_DIALOG, None)
	Glob['print_settings'] = prn_op.get_print_settings()
	print resp
	return (resp, prn_op.get_error())

while True:
	resp, error = show_print_dialog()
	if resp == gtk.PRINT_OPERATION_RESULT_CANCEL:
		break
	elif resp == gtk.PRINT_OPERATION_RESULT_APPLY:
		print "apply..."
		break
	elif resp == gtk.PRINT_OPERATION_RESULT_ERROR:
		error_dialog = gtk.MessageDialog(None, gtk.DIALOG_DESTROY_WITH_PARENT, gtk.MESSAGE_ERROR, gtk.BUTTONS_CLOSE, str(error))
		error_dialog.set_title("Print Error")
		error_dialog.run()
		error_dialog.destroy()
