# Tabdata commands

- [csv2td](#csv2td)
- [ics2td](#ics2td)
- [kvpairs2td](#kvpairs2td)
- [mrkv2td](#mrkv2td)
- [rextr](#rextr)
- [td2html](#td2html)
- [td2kvpairs](#td2kvpairs)
- [td2mrkv](#td2mrkv)
- [td-add-headers](#td-add-headers)
- [td-alter](#td-alter)
- [td-collapse](#td-collapse)
- [td-disamb-headers](#td-disamb-headers)
- [td-env](#td-env)
- [td-expand](#td-expand)
- [td-filter](#td-filter)
- [td-format](#td-format)
- [td-gnuplot](#td-gnuplot)
- [td-keepheader](#td-keepheader)
- [td-lpstat](#td-lpstat)
- [td-ls](#td-ls)
- [td-nup](#td-nup)
- [td-pivot](#td-pivot)
- [td-ps](#td-ps)
- [td-rename](#td-rename)
- [td-select](#td-select)
- [td-sort](#td-sort)
- [td-trans](#td-trans)
- [td-trans-fixcol](#td-trans-fixcol)
- [td-trans-group](#td-trans-group)
- [td-trans-gshadow](#td-trans-gshadow)
- [td-trans-ls](#td-trans-ls)
- [td-trans-mount](#td-trans-mount)
- [td-trans-passwd](#td-trans-passwd)
- [td-trans-shadow](#td-trans-shadow)
- [vcf2td](#vcf2td)

# csv2td

## NAME

csv2td - Transform CSV to tabular data format.

## DESCRIPTION

Read CSV data on STDIN.
Output tabular data to STDOUT.

## OPTIONS

Any option which Text::CSV(3pm) takes.
See `Text::CSV-`known\_attributes> for extensive list.
Example:

    csv2td --sep=';' --blank-is-undef=0 --binary

becomes:

    Text::CSV->new({sep=>";", blank_is_undef=>0, binary=>1})

## F. A. Q.

Why there is no td2csv?

Why would you go back to ugly CSV when you have nice shiny Tabdata?

## SEE ALSO

[csv2](#csv2)(1), [mrkv2td](#mrkv2td)(1)

# ics2td



# kvpairs2td

## NAME

kvpairs2td - Transform lines of key-value pairs to tabular data stream

## OPTIONS

- -i, --ignore-non-existing-columns

    Do not fail when encounters a new field after the first record.

- -w, --warn-non-existing-columns
- -c, --column _COLUMN_

    Indicate that there will be a column by the name _COLUMN_.
    This is useful if the first record does not have _COLUMN_.
    This option is repeatable.

- -r, --restcolumn _NAME_

    Name of the column where the rest of the input line will be put
    which is not part of key-value pairs.
    Default is **\_REST**.

- -u, --unknown-to-rest

    Put unknown (non-existing) fields in the "rest" column
    (see **-r** option).

## SEE ALSO

[td2mrkv](#td2mrkv)(1), [td2kvpairs](#td2kvpairs)(1)

# mrkv2td

## NAME

mrkv2td - Transform multi-record key-value (MRKV) stream to tabular data format.

## DESCRIPTION

As tabular data format presents field names at the start of transmission,
[mrkv2td](#mrkv2td)(1) infers them only from the first record,
so no need to buffer the whole dataset to find all fields,
and it's usual for all records to have all fields anyways.

## OPTIONS

- -s, --separator _REGEXP_

    Regexp which separates field name from cell data in MRKV stream.
    Default is TAB (`\t`).

- -g, --multiline-glue _STRING_
- -i, --ignore-non-existing-columns
- -w, --warn-non-existing-columns
- -c, --column _NAME_

    Repeatable option.

## SEE ALSO

[td2mrkv](#td2mrkv)(1)

# rextr

## NAME

rextr - Extract string groups from text file matching by Regular Expressions

## SYNOPSIS

rextr _REGEXP_ \[_REGEXP_ \[...\]\]

## DESCRIPTION

Takes line-based input on [stdin](#stdin)(3) and
matches all the given _REGEXP_ regular expression patterns to the input lines.
Outputs tabular data with fields being the named and unnamed capture groups in the given _REGEXP_es.

Fields coming from unnamed capture groups are named as **F_n_** where _n_ is a 1-based counter,
like: **F1**, **F2**, **F3**, ...
Note, it is not always equivalent to the regexp capture group number (`$1`, `$2`, ...),
because [rextr](#rextr)(1) takes multiple _REGEXP_es, each with their own first capture group,
but the counter in field names is ever-increasing.

## LIMITATIONS

## SEE ALSO

[pcut](#pcut)(1)

# td2html

## NAME

td2html - Transform tabular data stream into a HTML table.

## SYNOPSIS

td2html

## DESCRIPTION

Takes a tabular data stream on STDIN and outputs a HTML table
enclosed in `<table>...</table>` tags.

# td2kvpairs

## NAME

td2kvpairs - Transform tabular data into key-value pairs

## OPTIONS

- -r, --prefix-field _NAME_

    Put this field's content before the list of key-value pairs.
    Default is **\_REST**.
    Prefix and the key-value pairs are separated by a space char,
    if there is any prefix.

## SEE ALSO

[td2mrkv](#td2mrkv)(1), [kvpairs2td](#kvpairs2td)(1)

# td2mrkv

## NAME

td2mrkv - Transform tabular data into multi-record key-value (MRKV) format.

## OPTIONS

- -s, --separator _STR_

    String to separate field name from content.
    Default is TAB (`\t`).

- -K, --sort-keys

    Note, sorting by keys does not support duplicated column names.

- -V, --sort-values

## EXAMPLE

getent passwd | tr : "\\t" | td-add-headers USER PW UID GID GECOS HOME SHELL | td-select +ALL -PW | td2mrkv

## SEE ALSO

[mrkv2td](#mrkv2td)(1), [td2html](#td2html)(1)

# td-add-headers

## NAME

td-add-headers - Add headers to the tabular data stream and pass through the rows.

## SYNOPSIS

td-add-headers _COLNAME\_1_ _COLNAME\_2_ ...

## DESCRIPTION

Add header row to the tabular data stream. Headers names will be the
ones specified in the command line arguments, from the left-most 1-by-1.

If there are more fields in the first data row, then additional columns
will be added with names like "COL4", "COL5", etc. by the index number
of the column counting from 1.
This may be prevented by --no-extra-columns option.

## OPTIONS

- -x, --extra-columns

    Give a name also to those columns which are not given name in the command parameters.

- -X, --no-extra-columns

    Do not add more columns than specified in the command parameters.

## EXAMPLE

    who | td-trans | td-add-headers USER TTY DATE TIME COMMENT

# td-alter

## NAME

td-alter - Add new columns and fields to tabular data stream, and modify value of existing fields.

## USAGE

td-alter _COLUMN_=_EXPR_ \[_COLUMN_=_EXPR_ \[_COLUMN_=_EXPR_ \[...\]\]\]

## DESCRIPTION

On each data row, sets field in _COLUMN_ to the value resulted by _EXPR_
Perl expression.

In _EXPR_, you may refer to other fields by `$F{NAME}` where _NAME_ is the column name;
or by `$F[INDEX]` where _INDEX_ is the 0-based column index number.
Furthermore you may refer to uppercase alpha-numeric field names, simply by bareword `COLUMN`,
well, enclosed in paretheses like `(COLUMN)` to avoid parsing unambiguity in Perl.
It's possible because these column names are set up as subroutines internally.

Topic variable (`$_`) initially is set to the current value of _COLUMN_ in _EXPR_.
So for example `N='-$_'` makes the field N the negative of itself.

You can create new columns simply by referring to a _COLUMN_ name that does not exist yet.
You can refer to an earlier defined _COLUMN_ in subsequent _EXPR_ expressions.

## EXAMPLES

Add new columns: TYPE and IS\_BIGFILE.
IS\_BIGFILE depends on previously defined TYPE field.

    ls -l | td-trans-ls | td-alter TYPE='substr MODE,0,1' IS_BIGFILE='SIZE>10000000 && TYPE ne "d" ? "yes" : "no"'

Strip sub-seconds and timezone from DATETIME field:

    TIME_STYLE=full-iso ls -l | td-trans-ls | td-alter DATETIME='s/\..*//; $_'

## OPTIONS

- -H, --no--header

    do not show headers

- -h, --header

    show headers (default)

## REFERENCES

"Alter" in td-alter comes from SQL.
[td-alter](#td-alter)(1) can change the "table" column layout.
But contrary to SQL's ALTER TABLE, [td-alter](#td-alter)(1) can modify the records too, so akin to SQL UPDATE as well.

# td-collapse

## NAME

td-collapse - Collapse multiple tabular data records with equivalent keys into one.

## SYNOPSIS

td-collapse \[_OPTIONS_\]

## DESCRIPTION

It goes row-by-row on a sorted tabular data stream
and if 2 or more subsequent rows' first (key) cell are
the same then collapse them into one row.
This is done by joining corresponding cells' data from each row into one
cell, effectively keeping every column's data in the same column.

If you want to group by an other column, not the first one, then first
reorder the columns by [td-select](#td-select)(1). Eg. `td-select KEYCOLUMN +REST`.

## OPTIONS

- -g, --glue _STR_

    Delimiter character or string between joined cell data.
    Default is space.

- -u, --distribute-unique-field _FIELD_

    Take the _FIELD_ column's cells from the first collapsed group,
    and multiplicate all other columns as many times as many rows are in this group,
    in a way that each cell goes under a new column corresponding to that cell's original row.
    _FIELD_ field's cells need to be unique within each groups.

    If an unexpected value found during processing the 2nd row group and onwards,
    ie. a value which was not there in the first group,
    it won't be distibuted into the new column, since the header is already sent,
    but left in the original column just like **-u** option would not be in effect.
    See "pause" and "resume" in the example below.

    **Example**:

        ID | EVENT  | TIME  | STATUS
        15 | start  | 10:00 |
        15 | end    | 10:05 | ok
        16 | start  | 11:00 |
        16 | end    | 11:06 | err
        16 | pause  | 11:04 |
        16 | resume | 11:05 |
        
        td-collapse -u EVENT -z
        
        COUNT | ID | EVENT        | TIME        | TIME_start | TIME_end | STATUS | STATUS_start | STATUS_end
        2     | 15 |              |             | 10:00      | 10:05    |        |              | ok
        4     | 16 | pause resume | 11:04 11:05 | 11:00      | 11:06    |        |              | err

- -s, --distributed-column-name-separator _STR_

    When generating new columns as described at **-u** option,
    join the original column name with each of the unique field's values
    by _STR_ string.
    See example at **-u** option description.
    Default is underscore `_`.

- -k, --keep-equivalent-cells-united

    Don't repeat the original cells' content
    in the collapsed cell if all of the original cell are the same.

- -z, --empty-distributed-cells

    Clear cells of which data moved to other columns by **-u** option.

## EXAMPLES

This pipeline shows which users are using each of the configured default
shells, grouped by shell path.

    # get the list of users
    getent passwd |\
    
    # transform into tabular data stream
    tr : "\t" |\
    td-add-headers USER X UID GID GECOS HOME SHELL |\
    
    # put the shell in the first column, and sort, then collapse
    td-select SHELL USER | td-keepheader sort | td-collapse -g ' ' |\
    
    # change header name "USER" to "USERS"
    td-alter USERS=USER | td-select +ALL -USER
    

**Output**:

    | COUNT | SHELL             | USERS                                        |
    | 4     | /bin/bash         | user1 user2 nova root                        |
    | 5     | /bin/false        | fetchmail hplip sddm speech-dispatcher sstpc |
    | 1     | /bin/sync         | sync                                         |
    | 1     | /sbin/rebootlogon | reboot                                       |
    | 6     | /usr/sbin/nologin | _apt avahi avahi-autoipd backup bin daemon   |

## CAVEATS

Have to sort input data first.

Group key is always the first input column.

If a row in the input data has more cells than the number of columns, those are ignored.

## SEE ALSO

[td-expand](#td-expand)(1) is a kind of an inverse to [td-collapse](#td-collapse)(1).

## REFERENCES

[td-collapse](#td-collapse)(1) roughly translates to SELECT COUNT(\*) + GROUP\_CONCAT() + GROUP BY in SQL.

# td-disamb-headers

## NAME

td-disamb-headers - Disambiguate headers in tabular data

## DESCRIPTION

Change column names in input tabular data stream by appending a sequential number 
to the duplicated column names.
The first occurrance is kept as-is.
If a particular column name already ends with an integer, it gets incremented.

## EXAMPLE

    echo "PID     PID     PID2    PID2    USER    CMD" | td-disamb-headers

Output:

    PID   PID3    PID2    PID4    USER    CMD

# td-env

## NAME

td-env - Add tabular data fields to environment and invoke a command for each record

## SYNOPSIS

td-env \[_OPTIONS_\] \[--\] _COMMAND_ \[_ARGS_\]

## DESCRIPTION

Takes Tabular Data on its input
and adding fields to the environment for _COMMAND_
and executes it for each input record.

## EXAMPLE

Data:

    | username | score |
    |----------|-------|
    | joe      | 1.0   |
    | james    | 2.1   |
    | jeremy   | n/a   |

Command:

    td-env sh -c 'Hi $username, your score is $score!'

Output:

    Hi joe, your score is 1.0!
    Hi james, your score is 2.1!
    Hi jeremy, your score is n/a!

## OPTIONS

- -p, --prefix _PREFIX_

    Prefix environment variable names by _PREFIX_

- -e, --errexit

    Stop processing records once a command failed.
    [td-env](#td-env)(1) exit with the last command's exit code
    (or 128 + signal number if terminated by a signal).

## SECURITY

[td-env](#td-env)(1) does not do any measure to protect critical environment variables,
such as PATH, HOME, etc.
They (probably) won't affect [td-env](#td-env)(1) itself (perhaps the perl interpreter is affected)
but the called _COMMAND_ does.
It's the user's responsibility to feed tabular data into it with safe header names.

## SEE ALSO

[env](#env)(1), [environ](#environ)(7)

# td-expand

## NAME

td-expand - Generate multiple rows from each one row in a Tabular data stream.

## SYNOPSIS

td-expand \[-f _FIELD_\] \[-s _SEPARATOR_\]

## DESCRIPTION

It goes row-by-row and splits the given _FIELD_ at _SEPARATOR_ chars,
creates as many rows on the output as many parts _FIELD_ is split into,
fills the _FIELD_ column in each row by one of the parts,
and fills all other columns in all resulted rows with the corresponding column's data in the input.

More illustratively:

    | SHELL       | USERS         |
    | /bin/bash   | user1 user2   |
    | /bin/dash   | user3 user4   |
    | /bin/sh     | root          |
    
    td-expand -f USERS -s ' ' | td-alter USER=USERS | td-select +ALL -USERS
    
    | SHELL       | USER          |
    | /bin/bash   | user1         |
    | /bin/bash   | user2         |
    | /bin/dash   | user3         |
    | /bin/dash   | user4         |
    | /bin/sh     | root          |

## OPTIONS

- -f, --field _FIELD_

    Which field to break up.
    Default is always the first one.

- -s, --separator _PATTERN_

    Regexp pattern to split _FIELD_ at.
    Default is space.

## SEE ALSO

[td-collapse](#td-collapse)(1) is a kind of inverse to [td-expand](#td-expand)(1).

# td-filter

## NAME

td-filter - Show only those records from the input tabular data stream which match to the conditions.

## USAGE

td-filter \[_OPTIONS_\] \[--\] _COLUMN_ _OPERATOR_ \[_R-VALUE_ | **field** _R-COLUMN_\] \[\[**or**\] _COLUMN_ _OPERATOR_ \[_R-VALUE_ | **field** _R-COLUMN_\] \[\[**or**\] ...\]\]

td-filter \[_OPTIONS_\] --perl _EXPR_

## DESCRIPTION

Pass through those records which match at least one of the conditions (inclusive OR).
A condition consists of a triplet of _COLUMN_, _OPERATOR_, and _R-VALUE_.
Instead of _R-VALUE_, may put **field** _R-COLUMN_,
in which case _COLUMN_ is compared not to a constant r-value, but to the value of _R-COLUMN_ field per each row.
You may put together conditions conjunctively (AND) by chaining multiple [td-filter](#td-filter)(1) commands by shell pipes.
Example:

    td-filter NAME eq john NAME eq jacob | tr-filter AGE -gt 18

This gives the records with either john or jacob, and all of them will be above 18.

The optional word "**or**" between triplets makes your code more explicite.

[td-filter](#td-filter)(1) evaluates the Perl expression in the second form and passes through records
only if the result is true-ish in Perl (non zero, non empty string, etc).
Each field's value is in `@F` by index, and in `%F` by column name.
You can implement more complex conditions in this way.

## OPTIONS

- -H, --no-header

    do not show headers

- -h, --header

    show headers (default)

- -i, --ignore-non-existing-columns

    do not treat non-existing (missing or typo) column names as failure

- -w, --warn-non-existing-columns

    only show warning on non-existing (missing or typo) column names, but don't fail

- -N, --no-fail-non-numeric

    do not fail when a non-numeric r-value is given to a numeric operator

- -W, --no-warn-non-numeric

    do not show warning when a non-numeric r-value is given to a numeric operator

## OPERATORS

These operators are supported, semantics are the same as in Perl, see [perlop](#perlop)(1).

    == != <= >= < > =~ !~ eq ne gt lt

For your convenience, not to bother with character escaping, you may also use these operators as alternatives to the canonical ones above:

- is
- = _(single equal sign)_

    string equality (**eq**)

- is not

    string inequality (**ne**)

- -eq

    numeric equality (**==**)

- -ne

    numeric inequality (**!=**)

- <>

    numeric inequality (**!=**)

- -gt

    numeric greater than (**>**)

- -ge

    numeric greater or equal (**>=**)

- -lt

    numeric less than (**<**)

- -le

    numeric less or equal (**<=**)

- match
- matches

    regexp match (**=~**)

- does not match
- do not match
- not match

    negated regexp match (**!~**)

Other operators:

- is \[not\] one of
- is \[not\] any of

    _R-VALUE_ is split into pieces by commas (`,`) and
    equality to at least one of them is required.
    Equality to none of them is required if the operator is negated.

- contains \[whole word\]

    Substring match.
    Plural form "contain" is also accepted.
    Optional _whole word_ is a literal part of the operator.

- contains \[one | any\] \[whole word\] of

    Similar to **is one of**, but substring match is checked
    instead of full string equality.
    Plural form "contain" is also accepted.
    Optional _whole word_ is a literal part of the operator.

- ends with
- starts with

    Plural forms are also accepted.

Operators may be preceeded by _not_, _does not_, _do not_ to negate their effect.

## CAVEATS

If there is no _COLUMN_ column in the input data, it's silently considered empty.
[td-filter](#td-filter)(1) does not need _R-VALUE_ to be quoted or escaped, however your shell may do.

## REFERENCES

[td-filter](#td-filter)(1) is analogous to SQL WHERE.

# td-format

## NAME

td-format - Print formatted lines per Tabular Data record

## SYNOPSIS

td-format _TEMPLATE_

## DESCRIPTION

Field names in _TEMPLATE_ are enclosed in curly brackets.

## OPTIONS

- --nofield=\[**empty**|**leave**|**name**|**skip-record**|**fail**\]

    How to resolve non-existent field names in template variables?

    - **empty**

        Replace with empty string.
        This is the default.

    - **leave**

        Leave the `{field_name`}> string there unresolved.

    - **name**

        Replace with the field name itself.

    - **skip-record**

        Don't output anything for the given record.
        Continue with the next one.

    - **fail**

        Exit the program immediately with error code.

- -n

    No newline.

## SEE ALSO

# td-gnuplot

## NAME

td-gnuplot - Graph tabular data using [gnuplot](#gnuplot)(1)

## USAGE

td-gnuplot \[_OPTIONS_\]

## DESCRIPTION

Invoke [gnuplot](#gnuplot)(1) to graph the data represented in Tabular data format on STDIN.
The first column is the X axis, the rest of the columns are data lines.

Default is to output an ascii-art chart to the terminal ("dumb" output in gnuplot).

td-gnuplot guesses the data format from the column names.
If the 0th column matches to "date" or "time" (case insensitively) then the X axis will be a time axis.
If the 0th column matches to "time", then unix epoch timetamp is assumed.
Otherwise specify what date/time format is used by eg. **--timefmt=%Y-%m-%d** option.

Plot data read from STDIN is buffered in a temp file
(provided by `File::Temp->new(TMPDIR=>1)` and immediately unlinked so no waste product left around),
because [gnuplot](#gnuplot)(1) need to seek in it when plotting more than 1 data series.

## OPTIONS

- -i

    Output an image (PNG) to the STDOUT,
    instead of drawing to the terminal.

- -d

    Let [gnuplot](#gnuplot)(1) decide the output medium,
    instead of drawing to the terminal.

- --_SETTING_
- --_SETTING_=_VALUE_

    Set any gnuplot setting, optionally set its value to _VALUE_.
    _SETTING_ is a setting name used in `set ...` gnuplot commands, except spaces replaced with dasshes.
    _VALUE_ is always passed to gnuplot enclosed in double quotes.
    Examples:

        --format-x="%Y %b"
        --xtics-rotate-by=-90
        --style-data-lines

    Gnuplot equivalent command:

        set format x "%Y %b"
        set xtics rotate by "-90"
        set style data lines

- -e _COMMAND_

    Pass arbitrary gnuplot commands to gnuplot.
    This option may be repeated.
    This is passed to [gnuplot](#gnuplot)(1) in command line (**-e** option) 
    after [td-grnuplot](#td-grnuplot)(1)'s own sequence of gnuplot setup commands
    and after the **--_SETTING_** settings are applied,
    so you can override them.

# td-keepheader

## NAME

td-keepheader - Plug a non header-aware program in the tabular-data processing pipeline

## USAGE

td-keepheader \[--\] <COMMAND> \[<ARGS>\]

## EXAMPLE

ls -l | td-trans-ls | td-select NAME +REST | td-keepheader sort | tabularize

# td-lpstat

## NAME

td-lpstat - [lpstat](#lpstat)(1) wrapper to output printers status in Tabular Data format

# td-ls

## NAME

td-ls - [ls](#ls)(1)-like file list but more machine-parseable

## SYNOPSIS

td-ls \[_OPTIONS_\] \[_PATHS_\] \[-- _FIND-OPTIONS_\]

## OPTIONS, [ls](#ls)(1)-compatible

- -A, --almost-all
- -g
- -G, --no-group
- -i, --inode
- -l (implied)
- -n, --numeric-uid-gid
- -o
- --time=\[atime, access, use, ctime, status, birth, creation, mtime, modification\]
- -R, --recursive
- -U (implied, pipe to [sort](#sort)(1) if you want)

## OPTIONS, not [ls](#ls)(1)-compatible

- --devnum
- -H, --no-header
- --no-symlink-target
- --add-field _FIELD-NAME_

    Add extra fields by name.
    See field names by **--help-field-names** option.
    May be added multiple times.

- --add-field-macro _FORMAT_

    Add extra fields by [find](#find)(1)-style format specification.
    For valid _FORMAT_s, see **-printf** section in [find](#find)(1).
    May be added multiple times.
    Putting `\\0` (backslash-zero) in _FORMAT_ screws up the output; don't do that.

- --help-field-names

    Show valid field names to be used for **--add-field** option.

## DESCRIPTION

Columns are similar to good old [ls](#ls)(1):
PERMS (symbolic representation),
LINKS,
USERNAME (USERID if **-n** option is given),
GROUPNAME (GROUPID if **-n** option is given),
SIZE (in bytes),
time field is either ATIME, CTIME, or default MTIME (in full-iso format),
BASENAME (or RELPATH in **--recursive** mode),
and SYMLINKTARGET (unless **--no-symlink-target** option is given).

Column names are a bit different than [td-trans-ls](#td-trans-ls)(1) produces, but this is intentional,
because fields by these 2 tools have slightly different meaning.
[td-trans-ls](#td-trans-ls)(1) is less smart because it just transforms [ls](#ls)(1)'s output and
does not always know what is in the input exactly; while [td-ls](#td-ls)(1) itself controls
what data goes to the output.

No color support.

## FORMAT

Output format is tabular data: a table, in which fields are delimited by TAB
and records by newline (LF).

Meta chars may occur in some fields (path, filename, symlink target, etc),
these are escaped this (perl-compatible) way:

    | Raw char  | Substituted to |
    |-----------|----------------|
    | ESC       | \e             |
    | TAB       | \t             |
    | LF        | \n             |
    | CR        | \r             |
    | Backslash | \\             |

Other control chars (charcode below 32 in ASCII)
including NUL, vertical-tab, and form-feed are left as-is.

## ENVIRONMENT

- TIME\_STYLE

    **TIME\_STYLE** is ignored as well as _--time-style_ option.
    Always show date-time in `%F %T %z` [strftime](#strftime)(3) format!
    It's simply the most superior.
    Equivalent to **TIME\_STYLE=full-iso**.

## SEE ALSO

[td-select](#td-select)(1), [td-filter](#td-filter)(1), [td-trans-ls](#td-trans-ls)(1), [lr](https://github.com/leahneukirchen/lr)

# td-nup

## NAME

td-nup - Transform lines of text into tabluar data by unroll N lines into a row

## OPTIONS

- -n _NUM_
- -E, --no-escape

    Don't escape text.
    By default, [td-nup](#td-nup)(1) assumes input lines are raw text data,
    thus need to escape tabular data special chars in them.

# td-pivot

## NAME

td-pivot - Switch columns for rows in tabular data

## SYNOPSIS

td-pivot

## CAVEAT

Must read and buffer the whole STDIN before output any data,
so inpractical on large data.

# td-ps



# td-rename

## NAME

td-rename - Rename tabular data columns

## USAGE

td-rename _OLDNAME_ _NEWNAME_ \[_OLDNAME_ _NEWNAME_ \[_OLDNAME_ _NEWNAME_ \[...\]\]\]

## EXAMPLE

    conntrack -L | sd '^(\S+)\s+(\S+)\s+(\S+)' 'protoname=$1 protonum=$2 timeout=$3' | kvpairs2td | td-rename _REST FLAGS

## SEE ALSO

Not to confuse with [rename.td](#rename.td)(1) which renames files, not columns.

# td-select

## NAME

td-select - Show only the specified columns from the input tabular data stream.

## USAGE

td-select \[_OPTIONS_\] \[--\] \[-\]_COLUMN_ \[\[-\]_COLUMN_ \[...\]\]

## OPTIONS

- -H, --no--header

    do not show headers

- -h, --header

    show headers (default)

- -i, --ignore-non-existing-columns

    do not treat non-existing (missing or typo) column names as failure

- -w, --warn-non-existing-columns

    only show warning on non-existing (missing or typo) column names, but
    don't fail

- --strict-columns

    warn and fail on non-existing (missing or typo) column names given in
    parameters, even if it's prefixed with hyphen, ie. when the user want to
    remove the named column from the output.

## DESCRIPTION

_COLUMN_ is either a column name,
or one of these special keywords:

- +ALL

    all columns

- +REST

    the rest of columns not given yet in the parameter list

_COLUMN_ is optionally prefixed with minus (`-`),
in which case the given column will not be shown,
ie. removed from the shown columns.

So if you want to show all columns except one or two:

    td-select +ALL -PASSWD

If you want to put a given column (say "KEY") to the first place and left others intact:

    td-select KEY +REST

## EXAMPLE

    ls -l | td-trans-ls | td-select -- NAME +REST -INODE -LINKS -MAJOR -MINOR

## REFERENCES

"Select" in td-select comes from SQL.
Similarly to SQL, [td-select](#td-select)(1) is to choose some of the columns and return them in the given order.

# td-sort

## NAME

td-sort - Sort tabular data by the columns given by name

## USAGE

td-sort _OPTIONS_

## OPTIONS

All those which are accepted by [sort](#sort)(1),
except you don't need to refer to columns by ordinal number,
but by name.

- -k, --key=_KEYDEF_

    [sort](#sort)(1) defines _KEYDEF_ as `F[.C][OPTS][,F[.C][OPTS]]`,
    where **F** is the (1-based) field number.
    However with [td-sort](#td-sort)(1) you may refer to fields by name.
    But since **F** is no longer consists only of digits,
    but is an arbitrary string,
    it's may be ambiguous where the name ends.
    So you may enclose them in round/square/curly/angle brackets.
    Choose the one which does not occur in the column name.

    You don't need to even type **-k**, because a lone _COLUMN-NAME_
    is interpreted as "**-k** _F_" where _F_ is the corresponding field number.

## REFERENCES

[td-sort](#td-sort)(1) is analogous to SQL ORDER BY.

# td-trans

## NAME

td-trans - Transform whitespace-delimited into TAB-delimited lines ignoring sorrounding spaces.

## OPTIONS

- -n, --min-columns _NUM_

    Register at least this many columns
    even if the first records has less than this many.

- -x, --max-columns _NUM_

    Maximum number of columns.
    The _NUM_th column may have any whitespace.
    By default it's the number of fields in the header (first line).

# td-trans-fixcol

## NAME

td-trans-fixcol - Transform a table-looking text, aligned to fixed columns by spaces, into tabular data.

## DESCRIPTION

First line is the header consisting of the column names.
Each field's text must start in the same terminal column as the column name.

## OPTIONS

- -m, --min-column-spacing _NUM_

    Minimum spacing between columns.
    Default is 2.
    This allows the input data to have column names with single spaces.

## EXAMPLE

    arp -n | td-trans-fixcol

# td-trans-group



# td-trans-gshadow



# td-trans-ls

## NAME

td-trans-ls - Transform [ls](#ls)(1) output into fix number of TAB-delimited columns.

## USAGE

ls -l | td-trans-ls

## DETAILS

Supported [ls](#ls)(1) options which affect its output format:

- --human-readable
- --inode
- --recursive
- --time-style={iso,long-iso,full-iso}

Unsupported options:

- --author
- -g
- -o
- --time-style=locale

# td-trans-mount

## NAME

td-trans-mount - Transform [mount](#mount)(1) output to tabular data stream.

## DESCRIPTION

Supported [mount](#mount)(1) options which affect output format:

- -l (show labels)

## EXAMPLES

mount | td-trans-mount

mount -l | td-trans-mount

# td-trans-passwd



# td-trans-shadow



# vcf2td

## NAME

vcf2td - Transform VCF to tabular data format.

## OPTIONS

- -c, --column _COLUMN_

    Indicate that there will be a column by the name _COLUMN_.
    Useful if the first record does not contain all fields
    which are otherwise occur in the whole data stream.
    By default, [vcf2td](#vcf2td)(1) recognize fields which are in the first record in the VCF input,
    does not read ahead more records before sending the header.
    This option is repeatable.

- -i, --ignore-non-existing-columns

    Don't fail and don't warn when ecountering new field names.

    Tabular data format declares all of the field names in the column headers,
    so it can not introduce new columns later on in the data stream
    (unless some records were buffered which are not currently).
    However in VCF, each record may have fields different from the first record.
    That's why [vcf2td](#vcf2td)(1) fails itself by default
    if it encounters a field it can not convert to tabular.

- -w, --warn-non-existing-columns

    Only warns on new fields, but don't fail.

- -g, --multivalue-glue _STR_

    A string to glue repeated fields' values together
    when the repeated fields are handled by uniting their content into one tabdata column.
    Default is newline.

    Note, eventhough newline is the default glue, but
    if you want to be explicit about it (or want to set an other glue _STR_ expressed often by some backslash sequence),
    `vcf2td -g "\n" ...` probably won't quite work as one may expect (depending on one's shell),
    because the shell passes the "backslash" + "n" 2-chars string,
    instead of a string consisting just 1 "newline" char.
    So, in bash, put it as `vcf2td -g $'\n' ...`.

## COMMON vCard FIELDS

- N

    **N** is for a contact's name, different parts separated by `;` semicolon.
    [vcf2td](#vcf2td)(1) simplifies the **N** field by removing excess semicolons.
    If you need one or more name parts precisely,
    request the **N.family**, **N.given**, **N.middle**, **N.prefixes** fields
    by the **-c** option if you want,
    but this name partitioning method is not quite internationally useful,
    use the **FN** (full name) field for persons' names as much as you can.
