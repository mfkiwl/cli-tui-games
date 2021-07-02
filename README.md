# cli-tui-games
Simply my games running in command line or terminal user interface


## [2048](https://github.com/skelly37/cli-tui-games/2048.cpp)

### Features

The game itself allows little features. 
- What you can expect? Basic menu, actual game, key bindings manual, credits and highscore counting system. 
- What you should not expect? Colorful UI, of course (which makes the game both more challenging and frustrating).

### Rules

My version of game runs in an endless mode. Collect points as long as you can!

You can **move up, down, left and right** (**WSAD**) — the whole table moves. It may sound silly but it iss very intuitive when you start playing. You can also **undo** move (but once! No cheating possible :) — do a move and you will be able to undo once again) with **E** and **quit** the game with **Q**

### Premade binaries

Cannot compile or simply do not want to? There you go! :)

- Windows:
- Linux:
- Mac: I have neither Mac nor friends with Mac, so it's your turn to contribute to the project ;)


## [Blackjack](https://github.com/skelly37/cli-tui-games/blackjack.cpp)

### Features

Blackjack is my favorite card game (and a favorite way to gamble, I guess) so I tried to make it the best possible.

- What you can expect? Basic menu, wins/draws/losses/total games statistics, manual about the game and the key bindings used, standard game 1v1 with (lowkey configurable) AI, Hi Lo card counting challenge, credits, my own card design, persistent settings:
  - Number of decks to use during normal game
  - Number of cards used during card counting challenge
  - Whether dealer should play with Hi Lo strategy or simply hit unless his hand is higher than 16
  - Display/Do not display information when the cards are shuffled once again 
  - Display/Do not display "animated" game logo at statup (slow/instant loading of the ASCII art, so to speak)
- What you should not expect? Any colors, mouse support, fancy text formatting, anything that does not fit into terminal.
- What might be included in the future? Other than Hi Lo card counting strategies — if there will be demand for it or simply if I will feel like to do so.

### Rules

- Standard game:
  - Get as close as possible to 21 points (inclusive, i.e. having 21 points is the best possible outcome) but after busting (exceeding 21 points) you instantly lose! In this game you can only **Hit** (**H** key), **Stand** (**S** key) or **return to main menu** (**M** key), since no money-based moves like split or insurance make any sense in such training with AI (at least for me virtual money is no realistic simulation of the true risk). Color does not mater, blackjack cares only about the value.
- Hi Lo card counting challenge  
  - Card counting helps you with predicting the possibility of getting high or low value in the next hit ([more info](https://en.wikipedia.org/wiki/Card_counting)). Enter the correct sum and confirm with enter (**SUM, not the value of the current card**) or exit entering 0.25 (because it makes input validation easier and I an lazy).
  - Hi Lo card values (color also does not matter):
    - +1: 2, 3, 4, 5, 6
    - 0:  7, 8, 9
    - -1: 10, J, Q, K, A

### Premade binaries

Cannot compile or simply do not want to? There you go! :)

- Windows:
- Linux:
- Mac: I have neither Mac nor friends with Mac, so it's your turn to contribute to the project ;)

## More games

Help me popularize CLI/TUI games and join us on Telegram and Matrix (Matrix room is way more alive to be honest)

### Matrix: 
- #terminal-gaming:matrix.org
### Telegram: 
- https://t.me/cligaming

## License
**All code and programs in this repository are relased under MIT License.**
