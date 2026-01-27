
/*
how to compile?
---------------
1. download glib source
   tested with glib2.0-2.74.6
   eg. "apt source glib2.0"
2. cd subprojects/gvdb/gvdb
3. gcc -g gvdb-dump.c gvdb-reader.c gvdb-builder.c -I../../glib -I../../glib/gio -I../../glib/gvdb `pkg-config --cflags --libs glib-2.0 gio-2.0` -o gvdb-dump
*/

#include <stdio.h>
#include <stdlib.h>
#include <gio/gio.h>
#include "gvdb-reader.h"

void dump_table(GvdbTable *table, GPtrArray *path) {
    gsize n_keys;
    gchar **keys = gvdb_table_get_names(table, &n_keys);
    if (!keys) return;

    for (gsize i = 0; i < n_keys; i++) {
        gchar *key = keys[i];
        g_ptr_array_add(path, g_strdup(key));

        GVariant *value = gvdb_table_get_value(table, key);
        if (value) {
            for (guint i = 0; i < path->len; i++) printf("prefix\t%s\n", (char *)path->pdata[i]);
            gchar *valstr = g_variant_print(value, TRUE);
            printf("value\t%s\n\n", valstr);
            g_free(valstr);
//            g_variant_unref(value);
        } else {
            GvdbTable *sub = gvdb_table_get_table(table, key);
            if (sub) {
                dump_table(sub, path);
//                g_object_unref(sub);
            }
        }
        g_free(g_ptr_array_index(path, path->len - 1));
        g_ptr_array_remove_index(path, path->len - 1);
        g_free(key);
    }
    g_free(keys);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.gvdb\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    GError *error = NULL;

    // open GVDB table (read-only)
    GvdbTable *table = gvdb_table_new(filename, FALSE, &error);
    if (!table) {
        fprintf(stderr, "Failed to open GVDB: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    
    GPtrArray *path = g_ptr_array_new();
    
    dump_table(table, path);
    return 0;
}
