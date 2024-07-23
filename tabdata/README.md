# Tabular Data management tools

Tabular Data program suit (Tabdata) is a set of CLI tools (`td-*`) to
manipulate line-based data stream (text).
Each line represent a data **row**, also known as **record**,
which consists of some **cells**,
which are just strings separated by TAB (`0x09`, `\t`) char.
This way each **cell** holds the **value** of a **field**.
**Field** is a bit abstract part of Tabdata,
it refers to either to a **cell** in a given column position determined by field name - when speaking of it in the context of a row,
or to a whole **column** when speaking about the whole data stream.
So **fields** make up the **columns** labelled by the field's name in the first **row**.
Which makes the first row effectively the **header**.

You guessed, newline and tab (`\n`, `\t`) chars can not be in the cells
as raw data, but need to encode/escape somehow.
See "Escaping" section below.

Objective of Tabdata is to provide user-friendly
(guru/admin/terminal-warrior/poweruser-friendly) means to do everyday data processing
tasks, complementing (not superseding) well-established toolsets:
use Tabdata in combination with coreutils, moreutils, util-linux, ...
tools.

Focus is on text as being primarily for humans, not machines.
That's why control chars are not considered meaningful data.
You still can encode/decode your data stream before/after piping into
Tabdata tools, to process arbitrary bytestream as well.

# Escaping

Escaping rules in Tabdata:

 | Raw char  | Tabdata escape sequence |
 |-----------|-------------------------|
 | ESC       | Backslash + "e"         |
 | TAB       | Backslash + "t"         |
 | LF        | Backslash + "n"         |
 | CR        | Backslash + "r"         |
 | Backslash | Backslash + Backslash   |

## Q. Why not use ESC control code (0x1B) instead of Backslash?

Tabdata tools are intended to write output to file, pipes, and terminals alike.
The ESC code is avoided because it's handled specially by terminals,
whereas Backslash is not.

## Q. Why not escape the other control characters?

Feel yourself rather lucky that even these chars are escaped.
It would be better if you pre-filter or transform your input data
before passing it forward to Tabdata tools.
Tabdata does not do it for you, to prevent unneccessary computing
in cases where the user knows that the input data is good to go through as-is.

# Tool Descriptions

[descriptions.txt](descriptions.txt)

# Compare to CSV

Tabdata is very similar to CSV, except it has well-defined field- and record separators
and a well-defined encoding schema.

# Similar tools/projects

- GNU [datamash](https://www.gnu.org/software/datamash/) ([git](git://git.sv.gnu.org/datamash.git))
- [vnlog](https://github.com/dkogan/vnlog)
- [tsv-utils](https://github.com/eBay/tsv-utils-dlang)
- [csvkit](https://github.com/wireservice/csvkit)
- [q](https://github.com/harelba/q) Run SQL directly on delimited files...
- [sqawk](https://github.com/dbohdan/sqawk)

# Accessory tools

- [tabularize](../user-tools/tabularize) - to display results nicely on terminal
