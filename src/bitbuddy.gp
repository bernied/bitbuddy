v / version           flag        "version of bitbuddy"
b / bits              string      "bits to set for inputs"
c / commands          flag        "show commands"
f / file-bits         string      "file containing bits to set for inputs"
k / keep-nodes        flag        "do not free nodes"
g / garbage-collect   flag        "free before garbage collection"
p / print-gc          flag        "print garbage collection events"
n / nodes             int         "number of nodes to pre-allocate"
r / rounds            int         "use up to r rounds to find sat"
s / sat               int         "attempt to find input for given output with max bits"

#usage_begin
Usage: __PROGRAM_NAME__ [OPTION]... [FILE]

__GLOSSARY_GNU__

#usage_end
