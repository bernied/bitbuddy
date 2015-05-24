v / version           flag        "version of bitbuddy"
a                     string      "first set of bits"
b                     string      "second set of bits"
c / chunk-size        int         "size of h, a and b (defaults to input size/3)"
f / free              flag        "don't free nodes"
g / garbage-collect   flag        "free before garbage collection"
h / hash              string      "hash value input"
i / initial-hash      flag        "use standard initial sha256 hash"
l / length            flag        "use length of parameter a"
n / nodes             int         "number of nodes to pre-allocate"
s / sat               string      "attempt to find input for given output"

#usage_begin
Usage: __PROGRAM_NAME__ [OPTION]... [FILE]

__GLOSSARY_GNU__

#usage_end
