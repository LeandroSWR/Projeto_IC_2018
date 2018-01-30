# ic2017p2

  * Leandro Brás - a21701284
  * Hugo Martins - a21701372 
  * Diana Nóia - a21703004
 
## Our solution

This project consisted in creating a game, where zombies infected humans. It should happen in a 2d Toroidal grid1 with X and Y dimensions and a Moore neighbourhood2, and in each cell there can only be an agent, either a zombie or a human. 

At the start of the simulation, we should have n agents, that have to be spread randomly through the grid. The game is turn-based, and on each turn every agent can do an action. Humans can only move, and zombies can also move and infect humans.
 
The agent's movement can be done towards an empty cell on a Moore neighbourhood of radius 1. The game should end when there aren't any more human agents on the grid, or when the max number of turns is reached.

So, to solve this problem, we reached the conclusion that, after generating a world, we would have to find a way to generate a certain number of 2 different types of agents in a grid, where they would spawn in random positions each time, and would move one time every turn. 

This meant that we needed to find a way to search the map for the closest enemy each turn, and either move away from it or towards it, depending if it's a human running from a zombie or a zombie chasing a human.

The way we did this was creating a function that searched around the active agent  in a spiral that never passed a cell twice on the same search.

This also meant that we needed to find a way that would stop the agents from exiting the map, and that was done in a way that if an agent crosses the border of the map, he appears on the opposite side.


### Architecture
Our game was structured so that we had several files, each with a designated function/functions.
First we have the world.h and world.c files, that generate the grid, and the agents.h and agents.c files that keep all the information related to the agents.
Then we have the showworld.h and showworld_simple.c that make a simple graphic library to display the game.
The config.ini, ini.h and ini.c all manage the game definitions, and the file jogo.c creates the game world, and the ways to interact with it.

Here is a flow chart that explains the architecture of our game:

![fluxograma]()


## User guide

To play our game, you just press **ENTER** to skip turns while the game runs automatically.

When the game runs interactively you press the **numpad** keys to move your agents.

**Ex:**

| Key           | Move          |
| ------------- |:-------------:|
| **Numpad 1** | Down and Left |
| **Numpad 2** |      Down     |
| **Numpad 3** | Down and Right|
| **Numpad 4** |      Left     |
| **Numpad 5** |   Don't Move  |
| **Numpad 6** |     Right     |
| **Numpad 7** |  Up and Left  |
| **Numpad 8** |       Up      |
| **Numpad 9** |  Up and Right |

## Conclusions

By developing this game, we learned how to split a program into several files, and how to compile and build said program. We also learned about how to make automatic builds with the Make tool.
We learned about the Moore neighbourhood, toroidal grids, and we got to practice how and where to use structs and arrays.

## References

In order to find our solution it was necessary a lot of effort and dedication, after several tries we reunited with
other classmates to make a game that works according to the teacher's instructions.

* <a name="ref1">[1]</a> Pereira, A. (2017). C e Algoritmos, 2ª edição. Sílabo.
* <a name="ref2">[2]</a> "benhoyt" . [Simple .INI file parser in C](https://github.com/benhoyt/inih), GitHub.
* <a name="ref3">[3]</a> Conversas formais com os colegas Alejandro, Frederico e André Cosme sobre o projeto inteiro.
* <a name="ref4">[4]</a> Conversas formais com os colegas Leandro, Diana e Hugo sobre o projeto inteiro.

> Written with [StackEdit](https://stackedit.io/).
