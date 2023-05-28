# cal
A simple interactive calculator.
### Features
- signed 64 bit integer
- operators: `+` `-` `*` `/` `%` `&` `|` `^` `~`
- parens: `(1+2)*3`
- variable

### Statements
Every statement ends in a newline ('\n').
- `expr`
  - Evaluate and print the value of the expression.
- `name = expr`
  - Evaluate and assign the value of `expr` to variable `name`.
- `: var1 var2 var3`
  - Print the value of the requested variables in decimal, hexdecimal and octal form.
  - If no variable is provided, print all variables created, in reverse the order to the creation of the variables.
- `^Z`
  - Exit the calculator. `^Z` can be typed by pressing `Ctrl-Z`.
