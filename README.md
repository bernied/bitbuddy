BitBuddy
========

`bitbuddy` is a tool that takes a simple input file and constructs a Binary
Decision Diagram using one of the many BDD libraries that are available.

Building `bitbuddy`
-------------------

In order to build `bitbuddy`, execute the
```
.\build
```
command at the root level.
Note, that you must have the various BDD libraries installed if you want the
corresponding `bitbuddy_<lib>` tool to be built. See the table below.

| Tool Name | BDD library | URL | Notes |
| ------------- | -------------|-----| -------| 
| `bitbuddy_a` | ABCD | http://fmv.jku.at/abcd/ |  Broken. |
| `bitbuddy_b` | Buddy | http://buddy.sourceforge.net/manual/main.html |  |
| `bitbuddy_c` | CUDD | http://vlsi.colorado.edu/~fabio/CUDD/html/index.html |  |
| `bitbuddy_d` | SDD | http://reasoning.cs.ucla.edu/sdd/ |  |
| `bitbuddy_l` | CAL | https://embedded.eecs.berkeley.edu/Research/cal_bdd/ |  |
| `bitbuddy_s` |  Sylvan | http://fmt.cs.utwente.nl/tools/sylvan/ |  |
| `bitbuddy_z` | CUDD/ZDD | http://vlsi.colorado.edu/~fabio/CUDD/html/index.html | zdd library from CUDD. |

`bitbuddy` format
-----------------

The file format for `bitbuddy` is line oriented. Each line in the file is a
separate command. A command consists of an Operation, and some parameters,
which are tab delimted. A command ends with a new line. The file must start
with an `IO` command which defines the number inputs and outputs for the file.

The commands follow in this table.

| Operation Name | Operand 1 | Operand 2 | Operand 3 | Description |
| :--------------: | -----------------| ------------------| ----------| ------------|
| `IO`             | Number of inputs | Number of outputs | | Defines the number of inputs and outputs for the BDD. Must be first line of file |
| `IN `            | Id of the input | Count of the input | | Defines an input for the BDD |
| `OUT`            | Id of the output | Id of lhs of the output | Id of the rhs of the output | Defines an output variable for BDD |
| `DOT`            | Id of the node to create a `.dot` file for | | | Generates a `.dot` file for visualization |
| `FREE`           | Id of the node to free | | | Node is no longer needed, so free the memory for it |
| `NOP`            | | | | Does nothing |
| `NOT`            | Id of the output for `not` operation | Id of the input for `not` | | |
| `AND`            | Id of the output for `and` operation | Id of lhs of `and` | Id of rhs of `and` | Returns bdd that is `and` of two nodes |
| `OR`             | Id of the output for `or` operation | Id of lhs of `or` | Id of rhs of `or` | Returns bdd that is `or` of two nodes |
| `XOR`            | Id of the output for `xor` operation | Id of lhs of `xor` | Id of rhs of `xor` | Returns bdd that is `xor` of two nodes |
| `CON`            | Id of the output for conjunction | Up to six conjunction inputs | | Conjunction of inputs |
| `DIS`            | Id of the output for disjunctions | Up to six disjunction inputs | | Disjunction of inputs |
| `TRUE`           | Id of the node that is `true`| | | Defines a node that is always true |
| `FALSE`          | Id of the node that is `false`| | | Defines a node that is always false |


`bitbuddy` Command Line Usage
-----------------------------

```
Usage: ./bitbuddy_xxx [OPTION]... [FILE]

  -v, --version         version of bitbuddy
  -b, --bits            bits to set for inputs
  -c, --commands        show commands
  -f, --file-bits       file containing bits to set for inputs
  -k, --keep-nodes      do not free nodes
  -g, --garbage-collect free before garbage collection
  -p, --print-gc        print garbage collection events
  -n, --nodes           number of nodes to pre-allocate
  -r, --rounds          use up to r rounds to find sat
  -s, --sat             attempt to find input for given output with max bits
  -h, --help            Display this help and exit.
```
