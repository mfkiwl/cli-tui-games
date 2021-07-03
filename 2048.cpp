#include<iostream>
#include<ctime>
#include<string>
#include<iomanip>
#include<fstream>

#ifdef _WIN32
	// getch()
	#include<conio.h>
	// Sleep()
	#include<Windows.h>
#else
	// get_char()
	#include<termios.h>
	// usleep()
	#include<unistd.h>
#endif

using namespace std;



class Game2048
{
	private:
	/* Class attributes */

		// current game state board
		unsigned short board[4][4] = {{0,0,0,0},
									  {0,0,0,0},
									  {0,0,0,0},
									  {0,0,0,0}},

		// board to check whether the undo is possible that keeps the board status from previous round 
		previous_board[4][4];

		// moves&points accumulators 
		unsigned int moves=0, points=0,

		// helper variable to perform undo
		previous_points,

		// najlepszy wynik
		highscore;

		// flag checking if game should display info about highscore
		bool highscore_beaten = false;
	/* End of class attributes */

	/* Board logic */

		// Main method performing moves on board
		// 	1) update helper board after previous move
		// 	2) read user input
		// 	3) if moved, update acummulators and add 2 or 4 if and where possible
		void move(char key)
		{
			switch(key)
			{
				case 'W':
					update_cache();
					move_up();
					break;
				case 'S':
					update_cache();
					move_down();
					break;
				case 'A':
					update_cache();
					move_left();
					break;
				case 'D':
					update_cache();
					move_right();
					break;
				case 'E':
					undo();
					return;
				default:
					return;
			};
			finish_move();
		}

		/* Helper methods to perform moves */
			
			/* Methods to add neighboring numbers ( e.g. 2048::add_up() performs a move upwards) */

				void add_up(unsigned short row, unsigned short column)
				{
					board[row][column] += board[row+1][column];
					points += board[row][column];
					board[row+1][column] = 0;
				}

				void add_down(unsigned short row, unsigned short column)
				{
					board[row+1][column] += board[row][column];
					points += board[row+1][column];
					board[row][column] = 0;
				}

				void add_left(unsigned short row, unsigned short column)
				{
					board[row][column] += board[row][column+1];
					points += board[row][column];
					board[row][column+1] = 0;
				}

				void add_right(unsigned short row, unsigned short column)
				{
					board[row][column+1] += board[row][column];
					points += board[row][column+1];
					board[row][column] = 0;
				}
			/* End of methods to add neighboring numbers */

			/* Methods moving zeros as away from the mentioned in their names borders as possible (e.g. 2048::clear_zeros_up() literally moves zeros as close to the bottom border as possible) */

				void clear_zeros_up(unsigned short column)
				{
					bool moved = false;

					for(unsigned short count=0; count<3; count++)
					{
						for(unsigned short row=0; row<3; row++)
						{
							if(board[row][column] == 0 and board[row+1][column] != 0)
							{
								board[row][column] = board[row+1][column];
								board[row+1][column] = 0;
								moved = true;
							}
						}
						if(not moved)
							return;
					}
				}

				void clear_zeros_down(unsigned short column)
				{
					bool moved = false;

					for(unsigned short count=0; count<3; count++)
					{
						for(unsigned short row=3; row>0; row--)
						{
							if(board[row][column] == 0 and board[row-1][column] != 0)
							{
								board[row][column] = board[row-1][column];
								board[row-1][column] = 0;
								moved = true;
							}
						}
						if(not moved)
							return;
					}
				}

				void clear_zeros_left(unsigned short row)
				{
					bool moved = false;

					for(unsigned short count=0; count<3; count++)
					{
						for(unsigned short column=3; column>0; column--)
						{
							if(board[row][column-1] == 0 and board[row][column] != 0)
							{
								board[row][column-1] = board[row][column];
								board[row][column] = 0;
								moved = true;
							}
						}
						if(not moved)
							return;
					}
				}

				void clear_zeros_right(unsigned short row)
				{
					bool moved = false;

					for(unsigned short count=0; count<3; count++)
					{
						for(unsigned short column=0; column<3; column++)
						{
							if(board[row][column+1] == 0 and board[row][column] != 0)
							{
								board[row][column+1] = board[row][column];
								board[row][column] = 0;
								moved = true;
							}
						}
						if(not moved)
							return;
					}
				}
			/* End of methods moving zeros as away from the mentioned in their names borders as possible */

			/* Methods performing the actual move in desired direction (e.g. 2048::move_up() moves numbers upwards) */
				void move_up()
				{
					for(unsigned short column=0; column<4; column++)
					{
						clear_zeros_up(column);


						if(board[0][column] == board[1][column])
						{
							add_up(0, column);

							if(board[2][column] == board[3][column])
								add_up(2, column);
						}

						else if(board[1][column] == board[2][column])
							add_up(1, column);

						else if(board[2][column] == board[3][column])
							add_up(2, column);

						clear_zeros_up(column);
					}
				}

				void move_down()
				{
					for(unsigned short column=0; column<4; column++)
					{
						clear_zeros_down(column);


						if(board[2][column] == board[3][column])
						{
							add_down(2, column);

							if(board[0][column] == board[1][column])
								add_down(0, column);
						}

						else if(board[1][column] == board[2][column])
							add_down(1, column);

						else if(board[0][column] == board[1][column])
								add_down(0, column);

						clear_zeros_down(column);
					}
				}

				void move_left()
				{
					for(unsigned short row=0; row<4; row++)
					{
						clear_zeros_left(row);

						if(board[row][0] == board[row][1])
						{
							add_left(row, 0);

							if(board[row][2] == board[row][3])
								add_left(row, 2);
						}

						else if(board[row][1] == board[row][2])
							add_left(row, 1);

						else if(board[row][2] == board[row][3])
							add_left(row, 2);

						clear_zeros_left(row);
					}
				}

				void move_right()
				{
					for(unsigned short row=0; row<4; row++)
					{
						clear_zeros_right(row);

						if(board[row][2] == board[row][3])
						{
							add_right(row, 2);

							if(board[row][0] == board[row][1])
								add_right(row, 0);
						}

						else if(board[row][1] == board[row][2])
							add_right(row, 1);

						else if(board[row][0] == board[row][1])
							add_right(row, 0);

						clear_zeros_right(row);
					}
				}
			/* End of methods performing the actual move in desired direction */
		/* End of helper methods to perform moves */

		// Method returning a pointer to the dynamically allocated array of empty coordinates in main board --  that is where new 2 or 4 should be placed
		unsigned short** get_empty_indexes()
		{
			// initlialize the array...
			unsigned short** empty_indexes = new unsigned short*[16];

			// ...and make it 2D
			for(unsigned short index=0; index<16; index++)
				empty_indexes[index] = new unsigned short[2];

			// fill empty places with 9 -- helpful for iterating over the array
			for(unsigned short row=0; row<16; row++)
			{
					empty_indexes[row][0] = 9;
					empty_indexes[row][1] = 9;
			}

			// helper counter, used as an index for the loop saving coordinates
			unsigned short array_helper = 0;

			// loop saving coordinates of the empty spaces
			for(unsigned short row=0; row<4; row++)
			{
				for(unsigned short column=0; column<4; column++)
				{
					if(board[row][column] == 0)
					{
						empty_indexes[array_helper][0] = row;
						empty_indexes[array_helper][1] = column;
						array_helper++;
					}
				}
			}
			// return the result
			return empty_indexes;
		}

		// Method getting a pointer to the dynamically allocated array of empty coordinates in main board and inserting 4 or 2 there (chances are 1:8)
		void add_2_or_4(unsigned short** empty_indexes)
		{
			// there's no point in keeping the function going if there's no empty place in the board, just removing the empty array
			if(empty_indexes[0][0] == 9)
			{
				delete[] empty_indexes;
				return;
			}

			// variable to store the number that will be inserted into the board
			unsigned short two_or_four;

			// generating 4 or 2 with odds 1:8
			if(get_random_number(1, 9) == 9)
				two_or_four = 4;
			else
				two_or_four = 2;

			// counting empty places in the array 
			unsigned short num_of_indexes = 0;
			for(short count=0; count<16; count++)
			{
				if(empty_indexes[count][0] != 9)
					num_of_indexes++;
				else
					break;
			}

			// generating random, non-empty (9) index of the empty_indexes array and getting coordinates from there
			unsigned short 	random_index = get_random_number(0, num_of_indexes-1),
							row = empty_indexes[random_index][0],
						   	column = empty_indexes[random_index][1];

			// inserting the 2 or 4 into the chosen coordinates of the main board
			board[row][column] = two_or_four;

			// deleting the temporary array from 2048::get_empty_indexes()
			delete[] empty_indexes;
		}

		// Method checking whether player has changed anything in the board in this move
		bool anything_changed()
		{
			for(unsigned short row=0; row<4; row++)
			{
				for(unsigned short column=0; column<4; column++)
				{
					if(board[row][column] != previous_board[row][column])
						return true;
				}
			}
			return false;
		}

		// Name is self-explanatory — undo the move
		void undo()
		{
			bool move_undone = false;

			for(unsigned short row=0; row<4; row++)
			{
				for (unsigned short column=0; column<4; column++)
				{
					if(board[row][column] != previous_board[row][column])
					{
						board[row][column] = previous_board[row][column];
						move_undone = true;
					}

				}
			}

			if(move_undone)
			{
				moves--;
				points = previous_points;
			}
		}


		// Method updating helper array and the points accumulator 
		void update_cache()
		{
			for(unsigned short row=0; row<4; row++)
			{
				for (unsigned short column=0; column<4; column++)
					previous_board[row][column] = board[row][column];
			}

			previous_points = points;
		}

		// Method finishing the move (if there was any move at all) — incrementing moves accumulator, updating the board and (if beaten) highscore
		void finish_move()
		{
			if(anything_changed())
			{
				moves++;
				add_2_or_4(get_empty_indexes());
				update_highscore();
			}
		}
	/* End of board logic */

	/* Other logic */

		// Method checking whether any move is possible (0 in board or exact same numbers are neighbors)
		bool may_be_continued()
		{
			for(unsigned short row=0; row<4; row++)
			{
				for(unsigned short column=0; column<4; column++)
				{
					if(board[row][column] == 0)
						return true;
					else if(board[row][column] == board[row-1][column] and row>0)
						return true;
					else if(board[row][column] == board[row+1][column] and row<3)
						return true;
					else if(board[row][column] == board[row][column-1] and column>0)
						return true;
					else if(board[row][column] == board[row][column+1] and column<3)
						return true;
				}
			}
			return false;
		}

		// Update highscore (if beaten) and set flag needed by 2048::game_over() (if not already set)
		void update_highscore()
		{
			if(points>highscore)
			{
				highscore = points;
				if(not highscore_beaten)
					highscore_beaten = true;
			}
		}
	/* End of other logic */

	/* Game UI */

		// whole game UI
		void print_ui()
		{
			print_moves();
			print_board();
			print_points();
			print_highscore();
			print_game_controls();
		}

			/* Game UI helpers */
			void print_board()
			{
				string theboard = "\t\t╔═══════════════════════╗\n";

				for(unsigned short row=0; row<4; row++)
				{
					theboard += "\t\t║";
					for(unsigned short column=0; column<4; column++)
					{
						string current_num = to_string(board[row][column]);
						if(current_num == "0")
							current_num = " ";
						if(column>0)
							theboard += "│";

						switch(current_num.length())
						{
							case 5:
								theboard += current_num;
								break;
							case 4:
								theboard += " " + current_num;
								break;
							case 3:
								theboard += "  " + current_num;
								break;
							case 2:
								theboard += "   " + current_num;
								break;
							case 1:
								theboard += "    " + current_num;
								break;
						}
					}
					theboard += "║\n";
					if(row<3)
						theboard += "\t\t║─────┼─────┼─────┼─────║\n";
				}
				cout << theboard << "\t\t╚═══════════════════════╝\n";
			}

			void print_moves()
			{
				cout << "\t\tMoves: " << setw(20) << moves << endl;
			}

			void print_points()
			{
				cout << "\t\tPoints: " << setw(19) << points << endl;
			}

			void print_highscore()
			{
				cout << "\t\tHighscore: " << setw(10) << highscore << endl;
			}

			void print_game_controls()
			{
				cout << endl;
				cout << "\t\tWSAD -- move the board" << endl;
				cout << "\t\tE    -- undo" << endl;
				cout << "\t\tQ    -- exit" << endl;
			}
	/* End of game UI */

	/* MENU */
		static void print_menu(unsigned short position=0)
		{
			Game2048::clear_screen();

			print_menu_header();

			print_position1_new_game(position);
			print_position2_controls(position);
			print_position3_credits(position);
			print_position4_exit(position);

			print_menu_footer();
		}

			static void print_position1_new_game(unsigned short position)
			{
				if(position==1)
					cout << "\t    >>> NEW GAME <<<" << endl;
				else
					cout << "\t\tNEW GAME" << endl;
			}

			static void print_position2_controls(unsigned short position)
			{
				if(position==2)
					cout << "\t    >>> CONTROLS <<<" << endl;
				else
					cout << "\t\tCONTROLS" << endl;
			}

			static void print_position3_credits(unsigned short position)
			{
				if(position==3)
					cout << "\t    >>> CREDITS/INFO <<<" << endl;
				else
					cout << "\t\tCREDITS/INFO" << endl;
			}

			static void print_position4_exit(unsigned short position)
			{
				if(position==4)
					cout << "\t    >>> EXIT <<<" << endl;
				else
					cout << "\t\tEXIT" << endl;;
			}

		static void print_menu_header()
		{
			cout << "\t\t |2|0|4|8|" << endl;
			cout << "\t\tC++ edition" << endl << endl;
		}

		static void print_menu_footer()
		{
			cout << endl << endl;
			cout << "\t\tW - up" << endl;
			cout << "\t\tS - down" << endl;
			cout << "\t\tE - confirm" << endl;
		}
	/* End of MENU */

	public:
	/* Default constructor method, checking the highscore, initailizing the board and making the helper array */
		Game2048()
		{
			fstream highscore_file;
			highscore_file.open("highscore.dat", ios::in | ios::binary);

			if(highscore_file)
				highscore_file >> highscore;
			else
			{
				highscore_file.open("highscore.dat", ios::out | ios::binary);
				highscore_file << 0;
				highscore = 0;
			}

			highscore_file.close();

			add_2_or_4(get_empty_indexes());
			update_cache();
		}

	/* Destructor method, updating the highscore file after finishing the game */
		~Game2048()
		{
			fstream highscore_file;
			highscore_file.open("highscore.dat", ios::out | ios::binary);
			highscore_file << highscore;
		}

	/* Game core*/
		void game()
		{
			char choice;

			do
			{
				clear_screen();
				print_ui();
				choice = get_choice();
				move(choice);
			}while(choice != 'Q' and may_be_continued());
		}

	/* Communicative methods */

		static void print_controls()
		{
			cout << "\t\t-------------" << endl;
			cout << "\t\tControls:" << endl;
			cout << "\t\t-------------" << endl;
			cout << "\t\tW -- up" << endl;
			cout << "\t\tS -- down" << endl;
			cout << "\t\tA -- left" << endl;
			cout << "\t\tD -- right" << endl;
			cout << "\t\tE -- (in menu) confirm choice, (in game, available once per move) undo" << endl;
			cout << "\t\tQ -- exit" << endl << endl;

			cout << "\t\tPress any key to start game" << endl;
			cout << "\t\tor Q to exit" << endl;
			cout << "\t\t-----------------------------------------------\n";

			if(get_choice()=='Q')
				exit(0);
		}

		static void print_credits()
		{
			cout << "\t\t--------------------" << endl;
			cout << "\t\tCredits and info" << endl;
			cout << "\t\t--------------------" << endl;

			cout << "\t\tDeveloper: skelly37 (https://github.com/skelly37), 2021, MIT License";
			cout << endl << endl;
			cout << "\t\tMore terminal/terminal-like UI games:" << endl;
			cout << "\t\tMatrix: #terminal-gaming:matrix.org" << endl;
			cout << "\t\tTelegram: https://t.me/cligaming" << endl;
			cout << endl << endl;
			cout << "\t\tJoin us and help us with making TUI games more popular!" << endl;
			cout << "\t\tPress any key to start game" << endl;
			cout << "\t\tor Q to exit" << endl;
			cout << "\t\t-----------------------------------------------\n";

			if(get_choice()=='Q')
				exit(0);
		}

		void game_over()
		{
			clear_screen();
			print_board();
			cout << endl << "\t\t----------------------------" << endl;
			cout << "\t\tGAME OVER" << endl;
			if(highscore_beaten)
				cout << "\t\tNEW HIGHSCORE!";
			cout << endl;
			print_points();
			print_moves();
			cout << "\t\t----------------------------" << endl;
			cout << endl << "\t\tTo play once again press E" << endl;
			cout << "\t\tTo exit press Q" << endl;
			cout << "\t\t-----------------------------------------------\n";
		}

		static unsigned short main_menu()
		{
			char input;
			unsigned short position = 1;

			do
			{
				print_menu();
				
				Game2048::wait(0.1);

				print_menu(position);

				input = Game2048::get_choice();

				switch(input)
				{
					case 'W':
						position--;
						break;
					case 'S':
						position++;
						break;
				};

				if(position==0)
					position=4;
				else if(position==5)
					position=1;

			}while(input!='E');

			return position;
		}
	/* End of communicative methods */

	/* Static helper methods */

		static void clear_screen()
		{
			#ifdef _WIN32
				system("cls");
			#else
				system("clear");
			#endif
		}

		static char get_choice()
		{
			#ifdef _WIN32
				return toupper(getch());
			#else
				struct termios old, current;

				// getting terminal i/o settings
				tcgetattr(0, &old);

				// setting temporary settings same as normal ones
				current = old;

				// disabling the buffer and the visibility of entered key
				current.c_lflag &= ~ICANON;
				current.c_lflag &= ~ECHO;

				// applying new i/o settings
				tcsetattr(0, TCSANOW, &current);

				// getting a character from user
				char ch;
				cin >> ch;

				// restoring previous i/o settings
				tcsetattr(0, TCSANOW, &old);

				// returning the uppercase character
				return toupper(ch);
			#endif
		}

		// Method uses inclusive range, i.e. 2048::get_random_number(1,2) returns 1 or 2
		static unsigned short get_random_number(unsigned short min_value, unsigned short max_value)
		{
			srand(time(0));
			return (rand() % (max_value - min_value + 1)) + min_value;
		}

		static void wait(long double seconds)
		{
			#ifdef _WIN32
                    Sleep(seconds*1000);
            #else
                    usleep(seconds*1000000);
            #endif
		}
	/* End of static helper methods */
};

void mainloop()
{
	char choice;

	// loop creating new games after the previous one is finished
	while(choice != 'Q')
	{
		Game2048 game;
		game.game();
		game.game_over();
		choice = Game2048::get_choice();
	}
}

int main()
{
	char choice;

	unsigned short position = Game2048::main_menu();

	Game2048::clear_screen();

	switch(position)
	{
		case 4:
			exit(0);

		case 3:
			Game2048::print_credits();
			break;
		case 2:
			Game2048::print_controls();		
	}
	mainloop();
}
