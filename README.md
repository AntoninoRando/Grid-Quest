# Grid Quest

A minimal console game in which the goal is to reduce a grid of numbers up to a single value.

(*By Antonino Rando*)

## Gameplay

When a new game starts, a grid of numbers with a goal appear on the screen. A new *Quest* has just started! To win this Quest, the grid must be reduced until only one number remains, and it must be the same as the goal.

To reduce the grid, first two cells must be selected. To select those cells, simply move a cursor inside the grid.

| `w`            | `a`              | `s`              | `d`               | `q`                | `e`                |
| -------------- | ---------------- | ---------------- | ----------------- | ------------------ | ------------------ |
| Move cursor up | Move cursor left | Move cursor down | Move cursor right | Rotate cursor left | Rotate cursor right|

Then, press one of these key to perform an operation between the two selected cells.

| `+`             | `-`                  | `*`                  | `/`                | `%`                | $\mid$               |
| --------------- | -------------------- | -------------------- | ------------------ | ------------------ | -------------------- |
| Add two numbers | Subtract two numbers | Multiply two numbers | Divide two numbers | Module two numbers | Juxtapose two numbers|

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

## User Requirements

This section gathers some user requirements for the project.

### Functional Requirements

1. **Download the Application.** The application can be downloaded directly from the GitHub page. That page contains an executable file that is the playable game.

2. **Add profiles.** The system shall allow users to create local profiles by providing *distinct* usernames. If a user opens the game without a profile, the game shall ask (1) the user a nickname; (2) create a profile with that nickname; (3) use that profile. There is no verification process since the profile does not contain any important information.

3. **Change Profiles.** The system shall allow users to switch profile in any moment while in the menu.

4. **Delete Profiles.** The system shall allow users to delete their profiles in any moment, except when they have only one profile left. When a profile is deleted, so are their associated progress and suspended game.

5. **Change Settings.** The system shall allow users to customize controls (i.e., the commands they use to play) and visual options. The settings configuration is unique for machine (i.e., different profiles on the same machine share the same configuration).

6. **Track Progress.** The system shall allow users to track the progress of their profiles. In particular, users shall be able to see how many games they've played with a particular profile and what is their winning-ratio.

7. **Interrupt Game.** The system shall allow user to interrupt a game. When a game is interrupted, the user can decide to discard it and start a new one from the menu.

8. **Single Player.** The system shall run offline in single-player mode.

### Non-functional Requirements

1. The system shall not be strict in secure, since no important data are required and every progress is saved locally.

2. The system UI shall be high customizable and easy to change.

3. The system UI shall be easy to navigate and easy to understand.

4. The game should be challenging, yet not hard to a point that it becomes frustrating.

5. The progress page should be exhaustive.

### Diagrams

<details><summary>Use-Case Diagrams</summary>

![use-case-diagram](doc/Software/usecase.png "Use case diagram")

</details>

## System Requirements

### Functional Requirements

1. **Operating System.** Windows 8 or higher.

2. The system aesthetics and controls shall change immediately whenever any settings is modified and saved.

3. After a game is completed, the system shall update the user score with a new game and its result.

4. When a game is interrupted, the system shall save its state even for an immediate re-use.

5. While gaming, the system should track what user does after any input of interest.

### Non-Functional Requirements

1. The game shall run smooth without flickering. In other words, with high and constant performances.

2. The system shall be easy and fast to run since no expensive setup is required.

3. The system shall run in any windows system (8+) without requiring any change.

4. The game shall be run at any time and for any duration of time without any problem and system overloading.

5. The game should never have invalid settings. In case of wrong input or settings file, the system should run anyway with a default setting. Settings can't contrast each others.

### Diagrams

<details><summary>Architectural Diagram</summary>

![architecture](doc/Software/architecture.png "Architectural Diagram")

</details>

<details><summary>Activity Diagrams</summary>

#### Quest

![architecture](doc/Software/questactivity.png "Architectural Diagram")

</details>

<details><summary>State Diagrams</summary>

#### Menu

![architecture](doc/Software/state-diagram(menu).png "Architectural Diagram")

</details>

<details><summary>Message Sequence Charts</summary>

![architecture](doc/Software/message-sequence-chart.png "Architectural Diagram")

</details>

## Implementation

The project is written in C++, using

- the `hiredis` library for the Redis connections;
- the `libpqxx` library for the PostgreSQL connections;
- the `GTest` library for unit tests.

The software requires the connection to the Redis `localhost` server, at port `6379`. The connection with a postgreSQL is also required; specifically, to the `postgresql://postgres:postgres@localhost/gridquest`.

### System Components

The major system components are the Grid, the Quest, and the Settings. These components involves different C++ classes and design patterns (e.g., singleton pattern, state pattern, …, learn more at [Refactoring.Guru](https://refactoring.guru/)). We now present an overview of how they are used.

#### Grid

```
1. Let G be a 10x10 matrix of optional<int> (i.e., int or EMPTY).
2. Initialize G with all EMPTY optionals.
3. When a Quest is started:
    1. Let X be a random int between 18 and 25;
    2. Fill G from left to right, bottom to top, with X random int between 0 and 9.
4. When an operation A#B is request:
    1. If either A or B is EMPTY: return;
    2. Replace A with A#B;
    3. Replace B with an EMPTY optional;
    4. Adjust the gap created in B:
        1. Move down non-empty cells above B;
        2. Move to the left non-empty cells to the right of B.
```

#### Quest

```
1. Let G be a Grid, GOAL a random int between 0 and 99, and HP=10.
2. Fill G as described in (Grid.3).
3. When the user gives an input:
    1. If its a "move cursor" input, move the cursor inside G.
       If the cursor reach an extremity of the Grid, move it to the other extremity;
    2. If its an operation A#B:
        1. Apply the operation in G as described in (Grid.4);
        2. Reduce HP by |A-B|;
        3. If this is an odd-order operation: Increase HP by 5;
        4. If HP <= 0: return DEFEAT;
        5. If G has only 1 cell C left:
            1. If C == GOAL: return VICTORY;
            2. Otherwise: return DEFEAT.
4. After quest ends: execute Monitors to update Profile.
```

In the above pseudo-code, `return VICTORY` and `return DEFEAT` have to be interpreted as change scene in Victory or Defeat screen.

#### Settings

```
1. Make the default settings.
2. Read the file containing the saved settings configuration.
   For every line:
    1. Try change the default setting.
    2. If the change returns an error, abort change.
3. When user try to change a setting:
    1. Try change that setting.
    2. If the change returns an error, abort change.
```

Other important components of the software are the `MonitorState`, `Monitor` and `StreamParser` classes. We now introduce them.

#### Monitor

The `Monitor` class works like a finite-state machine. The `Monitor` in every moment has a single `MonitorState`, which can be executed to commit an SQL query. The `Monitor` changes its state through the `stateTransition` method, that uses three information to compute:

1. A Redis stream entry ID;
2. A key in the Redis stream entry;
3. A value in the Redis stream entry.

To read all the Redis entries from `gridquest` stream, with all the key-value pairs, is the `StreamParser` class.

```
1. Run the StreamParser on Monitors m1, m2, ... mN.
2. For every entry:
    1. Let ID be the id of that entry;
    2. For every KEY:VALUE pair in that entry:
       Call the stateTransition method for m1, m2, ..., mN in order.
3. Finally, if the TRIM value is >= 0: execute the Redis command
   "XTRIM gridquest MAXLEN TRIM"
```

### Database

Grid-quest uses a PostgreSQL database to save users information and statistics. The database is also used to save the time performances of the code; more specifically, scene loading times. More information can be found under the section [Experiments Results](#experiments-results).

### Redis Connection

We've implemented a custom class `Redis` using the `hiredis` library. This class works like the `std::cout`: we can put strings inside the class writing `Redis::get() << ...` and then we can execute the resulting command with `Redis::get().push()`. This class is intended to work specifically with a Redis stream> putting strings inside the class is like putting entries inside the stream. For example:

```Cpp
Redis::get() << "quest-start 1 "
             << "grid 1.1.0.1.0";
Redis::get().push();
```

Here we can notice that the `Redis` class is a Singleton; that is because we use it like it were the `gridquest` Redis stream. While the game software executes, important information (e.g., when the user changes nickname, enters a scene, loses/win a game, interrupts a game, …) are pushed into that stream.

After every quest is completed, the information pushed into the stream are parsed by a series of Monitors, which in turn commit those information in a Postgres database. This operation is necessary to update the user profile infos with the new played quest. When the game closes, the Redis stream is trimmed to preserve space. (Monitors can also be executed separately, in fact each monitor has its own executable.)

<details><summary>Database ER</summary>

![architecture](doc/Database/database-ER.png "Architectural Diagram")

</details>

## Experiments Results

Tests for the software can be found inside the `test` folder; those include unit-tests for the core functions of the software. Many other tests have been carried out manually. In fact, since this software is a videogame, one of the main aspects to analyze is how the users interact with it. Many features of the software (e.g., the graphics, the user experience) can only be tested in real time. Some metrics, however, can also be described with numbers:

- **Game Balancement**: Balancing a game is a though task that requires a deep analysis. To help in this work, we track of many games are won and how many are lost. Generally speaking, if the game is too easy we could expect an high win-rate since from the first quests played. On the other hand, if the game is harder than expected we would see a low win-rate even for experienced player.

- **Game Fun**: Making a game fun is the main goal of a game designer. We track how many time users spend while playing the game to serve this purpose. Ideally, if users play the game for a quite extended period of time we could expect the game to have satisfied those users. Of course, this is just a minimal way to see the problem.

    Of course we also need to understand potential harms to the player satisfaction. That is: if users spend few minutes playing, then we assume that the game is not fun; but why it is not fun?

    Of the main problematic that can make users drops the game is the slow performance of the code. Users don't want to spend their times waiting for scenes to load, neither do they want to play a game with low fps! Thus, we track how many milliseconds each scenes requires to fully display.

- **Bugs**: The main functions of the software has been tested with unit-test, so the game should not encounter problem during its execution.

### Monitors

We summarize what we've just said by listing the monitors we used to implement the following requirements (first 3 are non-functional, last 3 are functional):

1. **The game should be challenging**

    The monitor checks the users stats to determine whether the game is indeed hard for new-user, and fairly challenging for experienced users.

2. **The game shall run smooth**

    The monitor checks the time required to complete the main operations and determines whether the system is updating too slow.

3. **The game shall run as long as desired**

    The monitor checks whether the game closes unexpectedly and whether it requires more memory the more time it runs.

4. **Changes to settings shall be applied immediately**

    The monitor checks whether a non-existing input produces some effects (i.e., before an input mapping changed, the old input was still usable)

5. **Event Sourcing**

    The user inputs are tracked to determine whether the user is trying to achieve some operations in other ways, thus revealing that the user inputs are misleading.

6. **Track user stats**

### Results

A complete analysis of the software requires many testers that can play the game for a while. However, even without participants we could analyze the system performances and functionality: no errors have been encountered!

## Looking Forward

This project is still a working progress. Many aspects has been designed yet not fully implemented, thus something here could be incomplete. However, everything required by the assignment should be there.
