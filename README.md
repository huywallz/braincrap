# 🧠 Braincrap

Braincrap is an extended version of the esoteric programming language Brainfuck, designed to provide additional functionality while maintaining the simplicity and minimalism of the original language. This extension introduces file inclusion and macro functionality, making it more versatile for larger projects and reusable code.

Additionally, Braincrap comes with a powerful compiler (`braincrap.c`) that can translate Braincrap code into various target languages, enabling efficient execution after compilation.

## Features

- **File Inclusion**: Import external Braincrap files using the `$` syntax.
- **Macros**: Define custom macros for cleaner and more efficient coding using the `#` syntax.
- **Multiple Output Languages**: Compile to:
  - **C** (`-t c`): Standard C code.
  - **C0** (`-t c0`): Similar to C, but optimized for faster GCC compilation with slightly larger binaries.
  - **Brainfuck** (`-t bf`): Generates pure Brainfuck code directly.
- **Version Information**: Use `-v` to display the current version.
- **Help Message**: Use `-h` or `--help` to see available options and usage.
- **Specify Output File**: Use `-o <filename>` to choose the output file name.
- **Efficient Compilation**: By compiling to C or Brainfuck instead of interpreting directly, Braincrap programs can run faster than traditional Brainfuck interpreters.
- **Modular Programming**: Utilize file inclusion and macros for better code organization and reuse.

---

## Getting Started

### Prerequisites

To compile and run Braincrap programs, you need:
- A C compiler (such as `gcc` or `clang`)
- A terminal (Linux/macOS) or Command Prompt (Windows)

### Installing the Braincrap Compiler

First, compile the `braincrap.c` compiler:
```sh
gcc braincrap.c -o braincrap
```
This will create an executable named `braincrap`.

## Usage

To compile a Braincrap file, use the following syntax:
```sh
./braincrap [options] [filepath]
```

### Available Options:
| Option               | Description                                                         |
|----------------------|---------------------------------------------------------------------|
| `-o name`            | Output to file `name` (default is `bc.out`).                         |
| `--output`, `--out`  | Alternative forms of the `-o` option.                                |
| `-t`                 | Compile to the specified language (`bf`, `c`, `c0`).                  |
| `--target`           | Alternative form of the `-t` option.                                 |
| `-h`                 | Show this help message.                                              |
| `--help`             | Alternative form of the `-h` option.                                 |
| `-v`                 | Show version information.                                            |
| `--version`, `--ver` | Alternative forms of the `-v` option.                                |

---

## Writing Your First Braincrap Program

Create a file named `hello.bc` and add the following Braincrap code:
```brainfuck
+++++++++++[>++++++>+++++++++>++++++++>++++>+++>+<<<<<<-]>+++
+++.>++.+++++++..+++.>>.>-.<<-.<.+++.------.--------.>>>+.>-.
```

Compile it into C:
```sh
./braincrap -t c -o hello.c hello.bc
```

Then, compile the generated C code:
```sh
gcc hello.c -o hello
```

Finally, run the compiled program:
```sh
./hello
```

You should see:
```
Hello, World!
```

---

## Braincrap Syntax

Braincrap extends Brainfuck's standard 8 commands (`+`, `-`, `<`, `>`, `[`, `]`, `.` and `,`) with the following new features:

### Macros

Macros allow defining custom single-character shortcuts for Brainfuck code.

#### Syntax:
```brainfuck
#<character> <Brainfuck code>
```
#### Example:
```brainfuck
#X +++[->++<]
X
```
Expands to:
```brainfuck
+++[->++<]
```

### File Inclusion

Files can be included using the `$` symbol followed by the filepath.

#### Syntax:
```brainfuck
$<filepath>
```
#### Example:
```brainfuck
$utilities.bc
```
This will insert the contents of `utilities.bc` into the program.

---

## Examples

### Example 1: Using Macros
```brainfuck
#A +++[->++<]  
#B ---[->--<]  

AABBA  
```
Expands to:
```brainfuck
+++[->++<]+++[->++<]---[->--<]---[->--<]+++[->++<]
```

### Example 2: File Inclusion
Create a file called `math.bc`:
```brainfuck
#M +++[->+++<]
```
Now use it in another file:
```brainfuck
$math.bc
M
```
Expands to:
```brainfuck
+++[->+++<]
```

---

## Compilation & Execution

Braincrap code is compiled using the following syntax:
```sh
./braincrap [options] <input file>
```

### Example: Compiling to C
```sh
./braincrap -t c -o output.c myfile.bc
gcc output.c -o myprogram
./myprogram
```

### Example: Compiling to C0
```sh
./braincrap -t c0 -o output.c myfile.bc
gcc output.c -o myprogram
./myprogram
```

### Example: Compiling to Brainfuck
```sh
./braincrap -t bf -o output.bf myfile.bc
bf output.bf
```

---

## Contributing

Braincrap is an open-source project, and contributions are highly encouraged! There are many ways you can help improve Braincrap:

- **Reporting Bugs**: Open an issue on GitHub if you encounter any problems.
- **Suggesting New Features**: Share your ideas to make Braincrap even better.
- **Improving Documentation**: Help expand and refine the documentation.
- **Writing Example Programs**: Share interesting Braincrap programs.
- **Creating Libraries**: Develop reusable `.bc` files that others can include in their projects.

To contribute:
1. Fork the repository.
2. Make changes and test them.
3. Submit a pull request.

---

## License

Braincrap is released under the MIT License. Feel free to use, modify, and distribute it as you see fit.
