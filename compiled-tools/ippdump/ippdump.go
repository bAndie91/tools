
package main

import (
	"fmt"
	"os"
	"strings"

	"github.com/OpenPrinting/goipp"
	"github.com/spf13/pflag"
)

var keypath_sep string
var key_value_sep string

func flatten(prefix string, attrs goipp.Attributes) {
	for _, attr := range attrs {
		key := prefix + keypath_sep + attr.Name

		for _, val := range attr.Values {
			if c, ok := val.V.(goipp.Collection); ok {
				flatten(key, goipp.Attributes(c))
			} else {
				fmt.Printf("%s%s%s\n", key, key_value_sep, val.V.String())
			}
		}
	}
}

func dump(path string) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return err
	}

	var m goipp.Message
	if err := m.DecodeBytes(data); err != nil {
		return fmt.Errorf("decode: %w", err)
	}

	fmt.Printf("REQUEST-ID%s%d\n", key_value_sep, m.RequestID)
	fmt.Printf("VERSION%s%s\n", key_value_sep, m.Version)
	fmt.Printf("OPERATION%s%s\n", key_value_sep, goipp.Op(m.Code))

	for _, g := range m.AttrGroups() {
		group := strings.TrimSuffix(g.Tag.String(), "-tag")
		flatten(group, g.Attrs)
	}

	return nil
}

func main() {
	pflag.StringVarP(&keypath_sep, "keypath-delim", "d", "/", "Delimiter for key path elements")
	pflag.StringVarP(&key_value_sep, "key-value-separator", "s", "\t", "Key-value pair separator")
	pflag.Usage = func() {
		fmt.Fprintf(pflag.CommandLine.Output(), "Usage: %s [<OPTIONS>] <FILE> [<FILE> [...]]\n", os.Args[0])
		fmt.Fprintln(pflag.CommandLine.Output(), "Options:")
		pflag.PrintDefaults()
	}
	pflag.Parse()

	failed := false
	for _, path := range pflag.Args() {
		if err := dump(path); err != nil {
			fmt.Fprintf(os.Stderr, "%s: %v\n", path, err)
			failed = true
		}
	}
	if failed {
		os.Exit(1)
	}
}
