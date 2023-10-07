# RiLab Theorem Proving Software

## Basic Information

RiLab is designed to prove mathematical statements. For extra speed, the entire source code is written in C++, and uses multithreading to speed up calculations even further.

## Documentation

### Syntax

RiLab supports several different types of elements:

- `type_name` : A *fixed* type name. The default ones are Int, Bool, Float, and _. _ will match any type, including user defined types.
- `type_var` : A *variable* type name. This allows the programmer to specify functions where the output type is the same as one of the input types.
- `literal` : A literal value. These cannot be substituted, ie `a + literal` and `a + b` are not equivalent, since the literal has a fixed value. By default, `true` and `false` are Bool literals, regex `[0-9]*` is an Int literal, and regex `[0-9]*(.[0-9]+)?` is a Float literal.

- `operator` : An operator is a function with a fixed number of input parameters (all of fixed type, `TypeVar` or `_`) and a single output parameter. It is strongly typed. The default ones are `--> : (Bool, Bool) -> Bool` (implication) and `--<> : (Bool, Bool) -> Bool` (equivalence).

- `variable` : A strongly typed value that can hold *any* value. For example, an `Int` variable can have values 0 and 3 (though not simultaneously), while a literal cannot.

- `rule` : See the next section.

### Rule Syntax

A rule **must** have an output of type Bool and must respect operator types. It is constructed as follows:

```
data Rule a = Op r1 ...
            | Val
```

Here, `val` can be any variable or literal value, and `Op` can be any global or user-defined operation. You must pass the correct number of input parameters to the operation.

`r1`, `r2`, and so on are sub-rules. These are other rules (not necessarily with boolean output type) that are enclosed within parentheses.

Example:

`--> (InNatural x) (InNatural (S x))`

Here, the rule says that InNatural(x) implies InNatural(S(x)), for all `x` matching this variable's type.

### Main Commands

**Note:** From here on out, `<arg>` will be used to denote a required argument, and `[arg]` will be used to denote an optional argument.

RiLab supports the following commands:

`show` : This shows all variables, `TypeVar`s, operators, literals, and rules currently created.

`source <file>` : Run all the commands in the file. The file **MUST** be in LF, or this won't work. Every line in this file will be executed as a command, including `show`, `ask`, and other `source` commands.

Some useful files can be found in `rules` and can be `source`d via `source rules/file.rilab` .

**Important Note:** RiLab does not check for circular file `source` commands and **will** infinite loop if one is found. This cannot be interrupted via `SIGINT`, so it's the user's responsibility to ensure that no such circular `source` commands exist.

`declare` : This is the main command in the application, and has the following sub-commands:

- `declare type <type_name>` : Declare a new type.
- `declare typevar <type_name>` : Declare a new `TypeVar`. These are used for generating functions `f : a -> a` without needing to declare them for every type.
- `declare literal <string> <type_name>`: Declares a literal of the given type.
- `declare var <var_name> <var_type>` : Declare a new variable with name `var_name` and type `var_type`.
- `declare operator <op_name> <output_type> <in_type_1> [in_type_2] ...` : Declare a new operator `op_name` with output type and one or more input types. The number of input types specified will be equal to the number of arguments passed to the operator. Use `_` to mean an input or output of an unspecified type (this will match any type).
- `declare rule <rule>` : Declare a new rule.

`ask <rule>` : `ask` is the only command that can be used for proofs. It will attempt to prove the rule with the declared rules. If it succeeds, it prints the full proof to the console. Otherwise, it will print an error message. Note that this does **not** add the rule to the environment if it's valid (you must do that yourself).

## Building and running

This code was tested on WSL 2 + Windows 10, but should work on any Linux system. Correctness is untested on Mac or Windows command prompt.

To build and run the unit tests, use the following commands:

- `make parse_debug && bin/parse_debug`
- `make logic_debug && bin/logic_debug`
- `make thread_debug && bin/thread_debug`

For (manual) integration testing use

- `make main_debug && bin/main_debug`

All tests are GDB compatible.

To compile the full application, use
- `make production`

The main executable is called `RiLab` and is in the `bin` folder by default. To run it, you must pass in the following parameters:

- `bin/RiLab [thread_count] [recursion_limit]`

(Note that both arguments are optional.)

Here, `thread_count` is the number of WORKER threads to start. This means the full application will run `thread_count + 1` total threads. This is set to 4 by default.

Here, `recursion_limit` is the maximum length that a proof can be (ie, the maximum number of rule applications). This is set to 10 by default.

To interrupt a running `ask` command, simply send a `SIGINT` to the console. (control+C)

## Licensing / Attribution

The executable is licensed under CC-BY No-Derivatives 4.0. You are free to use this in your own projects as long as you cite the source. You may not modify or transform this work in any way.

The source code is licensed under CC-BY SA NC 4.0. You are free to use, modify, or transform the source code for noncommercial purposes as long as you cite the source.

This utility was made by me (rayyaw). Since I intend to remain anonymous, but may need to claim credit for this work, anyone capable of producing x such that

SHA256(x) = d2b57bba7bfa40176a62f0513ded10e1daaea379487ab65de7775bcb8b7bba25

may claim credit for this work.
