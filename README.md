# Grid Quest

A minimal console game in which the goal is to reduce a grid of numbers up to a single value.

(*By Antonino Rando*)

## Gameplay

When a new game starts, a grid of numbers with a goal appear on the screen. A new *Quest* has just started! To win this Quest, the grid must be reduced until only one number remains, and it must be the same as the goal.

To reduce the grid, first two cells must be selected. To select those cells, simply move a cursor inside the grid.

| w              | a                | s                | d                 | q                  | e                   |
| -------------- | ---------------- | ---------------- | ----------------- | ------------------ | ------------------- |
| Move cursor up | Move cursor left | Move cursor down | Move cursor right | Rotate cursor left | Rotate cursor right |

Then, press one of these key to perform an operation between the two selected cells.

| +               | -                    | \*                   | /                  | %                  | \|                    |
| --------------- | -------------------- | -------------------- | ------------------ | ------------------ | --------------------- |
| Add two numbers | Subtract two numbers | Multiply two numbers | Divide two numbers | Module two numbers | Juxtapose two numbers |

But be careful! Every new challenge you start with only 10 HP and every operation has a cost:

- after reducing two cells, you lose an amount of HP equal to the difference between the numbers in those two cells;
- luckily, each 2 operation performed you gain 5 HP.

## Project Motivation

This game was made for the seek of Software Engineering, not Game Design. In particular, to demonstrate competence in:

- Software development;
- Software documentation;
- Software testing;
- C++;
- Redis;
- PostgreSQL.

Thus, Grid Quest is not intended to be played for fun (unless you like it, of course!).

## Project Structure

- `doc` folder contains a) a `.pdf` document about the project b) diagram of the software and database structure;
- `src` folder contains the source code and the `include` folder contains the headers file for the `.cpp` files.

## Looking Forward

This project is still a working progress. Many aspects has been designed yet not fully implemented, thus something here could be incomplete.

### To Do List

- Build the project so that the shared libraries are static (i.e., embedded into th executable) or add an installer that installs them.
- Make settings changes preserve when game is closed.
