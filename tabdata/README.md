# Tabular Data management tools

Tabular Data program suit (Tabdata) is a set of CLI tools (`td-*`) to
manipulate line-based data stream (text).
Each line consists of some fields of text which are separated by tab
(`0x09`, `\t`) char.
Fields also make up columns labelled by the fields of the first line,
making it effectively a header line.
You guessed, newline and tab (`\n`, `\t`) chars can not be represented
without additional encoding in this format. Consider it a feature, not a
lack thereof.

Objective of Tabdata is to provide user-friendly
(guru/admin/poweruser-friendly) means to do everyday data processing
complementing (not superseding) well-established toolsets:
use Tabdata in compination with  coreutils, moreutils, util-linux, ...
tools.

Focus is on text as being primarily for humans, not machines.
That's why control chars are not considered meaningful data.
You still can encode/decode your data stream before/after piping into
Tabdata tools, to process arbitrary bytestream as well.

# Tool Descriptions

[descriptions.txt](descriptions.txt)

# Similar tools/projects

- GNU [datamash](https://www.gnu.org/software/datamash/) ([git](git://git.sv.gnu.org/datamash.git))
- [vnlog](https://github.com/dkogan/vnlog)
- [tsv-utils](https://github.com/eBay/tsv-utils-dlang)
- [csvkit](https://github.com/wireservice/csvkit)
- [q](https://github.com/harelba/q) Run SQL directly on delimited files...
- [sqawk](https://github.com/dbohdan/sqawk)
