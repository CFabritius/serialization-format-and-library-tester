# Serialization Format and C Library serialization tests
This repository is mostly meant to verify the code and results for a report. Aside from the code to run the tests, there is also a spreadsheet, `results.xlsx`, containing all measurements and derived values.

## To Run the tester
The code for all the libraries tested needs to be included as static library files inside `libraries/`.

`./run.sh <DEPTH> <BREADTH> <LIBRARY>`

- `DEPTH`: How many layers the generated tree should have.
- `BREADTH`: How many children every node should have.
- `LIBRARY`: Which library should be tested. The options correspond to the `Library` enum found in `main.c`.
