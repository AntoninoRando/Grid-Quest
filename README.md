# Grid Quest

A simple console game in which your goal is to complete a quest by reducing a grid of numbers!

The game is meant to be played in spare times, when you are at your computer and want something fast and relaxing. It does not require expensive setup, neither does it requires much computer's power, so it is perfect to run and play instantly.

## Gameplay

When you play a new game, you are given a grid of numbers and a ***quest***, that is a single number. Your goal is to reduce the grid until one number remains, and it must be the same as your quest.

To reduce the grid, move your cursor around the gird with the keyboard and select two cells. Then, perform an operation ($+$, $-$, $*$, …) by pressing the corresponding key on the keyboard.

Be careful! Every new challenge you start with 10 life points but every operation has a cost:

- after reducing two grid, you lose an amount of hp equal to the difference between the two cells;
- luckily, each 2 operation performed you gain 5 hp.

## User Requirements

### Functional Requirements

1. #### User Registration

    The system should allow users to create 3 local accounts by providing distinct usernames.

    There is no verification process since the account does not contain important information.

2. #### Settings Personalization

    The system should allow users to edit input and visual settings in order to personalize the game experience.

    The settings should be saved between session and they should be global (i.e. not linked to a single account but linked to the local host).

3. #### Track Progress

    The system should allow users to track their progress (i.e. the history of game won/lost, the highscore, and so on.)

### Use-Case Diagram

![use-case-diagram](diagrams\out\usecase.png "Use case diagram")

## System Requirements

### Non-Functional Requirements

1. #### Operating System

    Windows 7, 8, or 10;

### Architectural Diagram

![architecture](diagrams\out\architecture.png "Architectural Diagram")

### Activity Diagram

#### Quest

![architecture](diagrams\out\questactivity.png "Architectural Diagram")

### State Diagram

#### Menu

![architecture](diagrams\out\state-diagram(menu).png "Architectural Diagram")

### Message Sequence Chart

![architecture](diagrams\out\message-sequence-chart.png "Architectural Diagram")
