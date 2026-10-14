
package main

import (
	"fmt"
	"os"
	"strings"

	"github.com/OpenPrinting/goipp"
)

const sep = "/"

func flatten(prefix string, attrs goipp.Attributes) {
	for _, attr := range attrs {
		key := prefix + sep + attr.Name

		for _, val := range attr.Values {
			if c, ok := val.V.(goipp.Collection); ok {
				flatten(key, goipp.Attributes(c))
			} else {
				fmt.Printf("%s=%s\n", key, val.V.String())
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

	fmt.Printf("REQUEST-ID=%d\n", m.RequestID)
	fmt.Printf("VERSION=%s\n", m.Version)
	fmt.Printf("OPERATION=%s\n", goipp.Op(m.Code))

	for _, g := range m.AttrGroups() {
		group := strings.TrimSuffix(g.Tag.String(), "-tag")
		flatten(group, g.Attrs)
	}

	return nil
}

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s FILE...\n", os.Args[0])
		os.Exit(2)
	}

	failed := false
	for _, path := range os.Args[1:] {
		if err := dump(path); err != nil {
			fmt.Fprintf(os.Stderr, "%s: %v\n", path, err)
			failed = true
		}
	}
	if failed {
		os.Exit(1)
	}
}
