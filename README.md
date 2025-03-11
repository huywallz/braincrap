# 🧠 Braincrap

Braincrap is an extended version of the esoteric programming language Brainfuck, designed to provide additional functionality while maintaining the simplicity and minimalism of the original language. This extension introduces file inclusion and macro functionality, making it more versatile for larger projects and reusable code.  

Additionally, Braincrap comes with a compiler (`braincrap.c`) that translates Braincrap code into C, allowing for efficient execution after compilation with a standard C compiler.

## Features

- **File Inclusion**: Allows importing external Braincrap files using the `$` syntax.
- **Macros**: Define custom macros for easier and more efficient coding using the `#` syntax.
- **Braincrap Compiler (`braincrap.c`)**: Translates Braincrap code into C, which can then be compiled with a C compiler.
- **Efficiency**: By compiling to C instead of interpreting directly, Braincrap programs can run faster than traditional Brainfuck interpreters.
- **Modular Programming**: Using file inclusion and macros, Braincrap allows for better code organization and reuse.

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

### Writing Your First Braincrap Program

Create a file named `hello.bc` and add the following Braincrap code:
```brainfuck
+++++++++++[>++++++>+++++++++>++++++++>++++>+++>+<<<<<<-]>+++
+++.>++.+++++++..+++.>>.>-.<<-.<.+++.------.--------.>>>+.>-.
```
This is the traditional "Hello, World!" program in Brainfuck, which also works in Braincrap.

Now, compile it into C:
```sh
./braincrap hello.bc hello.c
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
This expands into:
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
This expands into:
```brainfuck
+++[->+++<]
```

---

## Compilation & Execution

Braincrap code is compiled to C using `braincrap.c`, which can then be compiled with any standard C compiler.

### Compile Braincrap to C
```sh
./braincrap myfile.bc output.c
```

### Compile the Generated C Code
```sh
gcc output.c -o myprogram
```

### Run the Compiled Program
```sh
./myprogram
```

---

## Advanced Usage

### Debugging Braincrap Code

If your Braincrap program isn't working as expected, you can:
1. Check if macros are expanding correctly by manually replacing them.
2. Verify that included files exist and contain valid Brainfuck code.
3. Inspect the generated C code (`output.c`) for any syntax errors.

### Optimizing Braincrap Code

- **Reduce unnecessary loops**: Avoid excessive loops that do nothing.
- **Use macros for readability**: Instead of repeating Brainfuck code, define macros.
- **Modularize with file inclusion**: Break large programs into smaller, reusable files.

---

## Contributing

Braincrap is an open-source project, and contributions are highly encouraged! There are many ways you can help improve Braincrap:

- **Reporting bugs**: If you encounter any issues, please open an issue on GitHub.
- **Suggesting new features**: Have an idea for a new feature? Let us know!
- **Improving documentation**: Help expand and refine the documentation for better clarity.
- **Writing example programs**: Share interesting Braincrap programs to help others learn.
- **Creating Braincrap libraries**: Develop reusable `.bc` files that others can include in their projects.

To contribute:
1. Fork the repository.
2. Make changes and test them.
3. Submit a pull request.

---

## License

Braincrap is an open-source project released under the MIT License. Feel free to use, modify, and distribute it as you see fit.