# run-from-maze
This is a homework I made for the **Algorithm and Data Structures** 2013 class at the **[University of Naples](http://www.unina.it)**.

It's a Pacman-inspired game based on graph theory. The main character has to run from enemies and go through a bunch of level, finding the exit. Enemies will try to reach the main character using some kind of shortest-path-based strategy.

The game is developed in C and it includes some libraries for managing linked lists, heaps and graphs algorithm (**BFS, Dijkstra and A***). Code is developed in a 'data-agnostic' way: algorithms are indipendent from data representation - one can use any kind of data representation (providing, of course, functions that can handle that representation) and the algorithms will work the same.

Heap allocation was debugged and tested with [Valgrind](valgrind.org/).

# How to use
It should work flawlessly on Linux (and I think mac OS too). Just make sure to have **ncurses-devel** and **gcc** installed.
Then just download the code and run *play.sh*, making sure to make it executable (*chmod +x* or *chmod 744*).

Enjoy!
