#include<iostream>
#include<iomanip>
#include<string>
#include<vector>
#include<fstream>
#include<ctime>

#ifdef _WIN32
	//getch()
	#include<conio.h>
	
	//for Blackjack::wait()
	#include<windows.h>
#else
	//for my lowkey getch()
	#include<termios.h>
	
	//for Blackjack::wait()
	#include<unistd.h>
#endif

using namespace std;

class Blackjack
{
	private:
		/* I could never fix those names in my brain lmao */

			/* ♠  	spades 		*/
			/* ♥ 	hearts 		*/
			/* ♦ 	diamonds 	*/
			/* ♣ 	clubs 		*/

		/* Hi-Lo card counts */

			/* 2-6       =  1  */
			/* 7-9       =  0  */
			/* 10-K + A  =  -1 */


		// current sum of cards and current values of aces (1 or 11)
		unsigned short	player_hand_sum,
						dealer_hand_sum,
						player_ace_value,
						dealer_ace_value,
						dealer_known_points;

		short			dealer_card_counting_memory;

		unsigned long long 	stats[5], 
							settings[5];

		// hands of players and the whole deck
		vector<string> 	dealer_hand,
		 				player_hand,
		 			 	cards;


/* User settings/stats accessor functions */
		bool display_shuffle_info()
		{
			return settings[0];
		}

		unsigned long long get_number_of_decks()
		{
			return settings[1];
		}

		unsigned long long get_number_of_cards_for_counting()
		{
			return settings[2];
		}

		bool if_dealer_counts_cards()
		{
			return settings[3];
		}

		bool logo_at_startup()
		{
			return settings[4];
		}

		unsigned long long get_total_games()
		{
			return get_wins()+get_draws()+get_losses();
		}

		unsigned long long get_wins()
		{
			return stats[0];
		}

		unsigned long long get_draws()
		{
			return stats[1];
		}

		unsigned long long get_losses()
		{
			return stats[2];
		}

		unsigned long long get_wins_in_counting()
		{
			return stats[3];
		}

		unsigned long long get_losses_in_counting()
		{
			return stats[4];
		}

		unsigned long long get_total_counting_games()
		{
			return get_wins_in_counting() + get_losses_in_counting();
		}
/* End of user settings/stats accessor functions */

/* Game initializers */

		void generate_card_stack(char game_or_training)
		{
			unsigned short num_of_decks;
			long long num_of_cards = -1;

			// exemplary one, complete deck
			const string deck[52] =	{"S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9", "S10", "S11", "S12", "S13", 
									"H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8", "H9", "H10", "H11", "H12", "H13", 
									"D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10", "D11", "D12", "D13",
									"C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "C10", "C11", "C12", "C13"};

			if(game_or_training=='G')
				num_of_decks = get_number_of_decks();
			else
			{
				num_of_cards = get_number_of_cards_for_counting();
				num_of_decks = num_of_cards/52;
				if(num_of_cards%52!=0)
					num_of_decks++;
			}

			clear_screen();
			if(display_shuffle_info())
			{
				cout << "Shuffling cards..." << endl;
				if(game_or_training=='G')
					cout << "Number of decks used:\t" << num_of_decks << endl;
				else
					cout << "Number of cards used:\t" << num_of_cards << endl;
				
				wait(1.25);
			}

			for(unsigned short count=0; count<num_of_decks; count++)
			{
				if(num_of_cards!=0)
				{
					for(unsigned short index=0; index<52; index++)
					{
						cards.push_back(deck[index]);
						num_of_cards--;
					}
				}
			}		
		}

		void initialize_new_game(char game_or_training)
		{
			dealer_hand.clear();
			dealer_ace_value = 11;
			dealer_hand_sum = 0;

			if(game_or_training=='G')
			{
				player_hand_sum = 0;
				player_ace_value = 11;
				dealer_known_points = 0;
				
				player_hand.clear();
			
			// Be sure that cards don't run out
				if(cards.size()<30)
				{
					dealer_card_counting_memory = 0;
					generate_card_stack('G');
				}
			
				get_random_card('P');
				get_random_card('P');
				get_random_card('D');
			}
			else
				generate_card_stack('T');
				
			get_random_card('D');
		}
/* End of game initializers */

/* Game logic */
		// Method making the AI dealer count the cards 
		void modify_card_count(unsigned short card_value)
		{
			if(card_value>1 and card_value<7)
				dealer_card_counting_memory++;
			else if(card_value==1 or card_value>9)
				dealer_card_counting_memory--;
		}

		// generate some random index from the deck, add it to the proper hand and then remove it from the deck
		void get_random_card(char dealer_or_player)
		{
			unsigned short index = get_random_number(0, cards.size()-1);
			update_hand(cards[index], dealer_or_player);
			cards.erase(cards.begin()+index);	
		}

		string get_card_symbol(string card)
		{
			string symbol(1, card[1]);

			if(card.length() == 3)
			{
				symbol += card[2];
				if(symbol=="10")
					return "10";
				else if(symbol=="11")
					return "J";
				else if(symbol=="12")
					return "Q";
				else if(symbol=="13")
					return "K";
			}

			else
			{
				if(symbol!="1")
					return symbol;
				else
					return "A";
			}

			// return written only to silence compiler warnings about no return statement
			return "Error";
		}

		string get_card_color(string card)
		{
			char color = card[0];

			switch(color)
			{
				case 'S':
					return "♠";
				case 'H':
					return "♥";
				case 'D':
					return "♦";
				case 'C':
					return "♣";
			}

			// return written only to silence compiler warnings about no return statement
			return "Error";
		}

		void update_hand(string random_card, char dealer_or_player)
		{
			// convert card to digit, e.g. "S2" -> 2
			char first_digit = random_card[1];
			unsigned short new_card_value = (unsigned short) first_digit - '0';
			
			switch(dealer_or_player)
			{
				case 'P':
				{
					// 10, J, Q, K give 10 points
					if(random_card.length() == 3)
						new_card_value = 10;

					// If there's only 2 characters and the string contdealerns '1', it must be an ace. Time for checking whether aces should be still counted as 11 or drop their value to 1
					// If there's already Ace (A and A), change its value to 1 as well.
					// Then adding the card to the hand and adding its value to the sum of points
					else if(first_digit == '1')
					{
						if(player_ace_value == 11 and (player_hand_sum+player_ace_value) > 21)
						{
							player_ace_value = 1;
							if(ace_in_hand('P'))
								player_hand_sum -= 10;
						}

						new_card_value = player_ace_value;
					}
					
					if(new_card_value+player_hand_sum>21 and ace_in_hand('P') and player_ace_value != 1)
					{
						player_ace_value = 1;
						player_hand_sum -= 10;
					}

					if(player_hand.size()==0)
						modify_card_count(new_card_value);

					player_hand.push_back(random_card);
					player_hand_sum += new_card_value;
					break;
				}
				
				case 'D':
				{
					if(random_card.length() == 3)
						new_card_value = 10;
					else if(first_digit == '1')
					{
						if(dealer_ace_value == 11 and (dealer_hand_sum+dealer_ace_value) > 21)
						{
							dealer_ace_value = 1;
							if(ace_in_hand('D'))
								dealer_hand_sum -= 10;
						}

						new_card_value = dealer_ace_value;
					}
					
					if(new_card_value+dealer_hand_sum>21 and ace_in_hand('D') and dealer_ace_value != 1)
					{
						dealer_ace_value = 1;
						dealer_hand_sum -= 10;
					}

					if(if_dealer_counts_cards())
						modify_card_count(new_card_value);

					dealer_hand.push_back(random_card);
					dealer_hand_sum += new_card_value;

					if(dealer_hand.size()==1)
						dealer_known_points = new_card_value;
				}
			}
		}

		bool ace_in_hand(char dealer_or_player)
		{
			switch(dealer_or_player)
			{
				case 'P':
				{
					for(string card : player_hand)
					{
						if(card.length() == 2)
						{
							char num = card[1];
							if(num == '1')
								return true;
						}
					}
					break;
				}
				case 'D':
				{
					for(string card : dealer_hand)
					{
						if(card.length() == 2)
						{
							char num = card[1];
							if(num == '1')
								return true;
						}
					}
				}
			}
			return false;
		}

		bool dealer_move()
		{
			if(if_dealer_counts_cards())
			{
				if 	((dealer_hand_sum<12) or 
					(dealer_hand_sum<15 and dealer_card_counting_memory<=0) or 
					(dealer_hand_sum<17 and dealer_card_counting_memory<0))
				{
					get_random_card('D');
					return true;
				}
				return false;
			}
			else
			{
				if(dealer_hand_sum < 17)
				{
					get_random_card('D');
					return true;
				}
				return false;
				
			}
		}

		unsigned short player_move()
		{
			char choice = get_char();
			if(choice == 'H')
			{
				get_random_card('P');
				return 1;
			}
			else if(choice == 'S')
				return 0;
			else if(choice == 'M')
			{
				initialize_new_game('G');
				return 2;
			}

			return 3;
		}

		// Core of the standard game
		bool game()
		{
			char choice;
			do
			{
				unsigned short if_player_moved=1;
				bool if_dealer_moved;
				initialize_new_game('G');

				do
				{
					clear_screen();
					print_game_ui();

					if(if_player_moved)
						do
						{
							if_player_moved = player_move();
							if(if_player_moved==2)
								return true;
						}while(if_player_moved>1);	

					if(player_hand_sum>21)
						break;
					
					if_dealer_moved = dealer_move();

				}while(player_hand_sum < 22 and dealer_hand_sum < 22 and (if_player_moved or if_dealer_moved));

				print_results();

				// update the card counting values if dealer counts them with player's cards unseen by the dealer 
				if(if_dealer_counts_cards())
				{
					for(unsigned short count=1; count<player_hand.size(); count++)
					{
						string card = player_hand[count];
						char first_digit = card[1];
						unsigned short value = first_digit - '0';

						if(card.length()==3)
							modify_card_count(10);
						else
							modify_card_count(value);
					}
				}
				choice = get_char();
			}while(choice != 'M');

			return true;
		}

		// Core of the card counting challenge
		bool training()
		{
			long double choice;
			
			bool looped;

			do
			{
				initialize_new_game('T');

				unsigned long long number_of_cards = get_number_of_cards_for_counting();

				for(unsigned short count=0; count<number_of_cards; count++)
				{
					clear_screen();
					print_line_of_cards('D', true);

					cout << "\t\tEnter the current count or 0.25 to exit" << endl << "\t\t";
					while(!(cin >> choice))
					{
						cin.clear();
						cin.ignore(10000, '\n');
						clear_screen();
						print_line_of_cards('D', true);
						cout << "\t\tEnter the current count or 0.25 to exit" << endl << endl;
						cout << "\t\tInvalid value, try again." << endl << "\t\t";
					}

					if(choice==0.25)
						return true;
					else if(choice!=dealer_card_counting_memory)
					{
						update_stats('U');
						cout << "\t\tYou lost." << endl;
						cout << "\t\tCorrect value:\t" << dealer_card_counting_memory << endl;
						cout << "\t\tYour answer:\t" << choice << endl;
						cout << "\t\tClick E to play once again or M to return to main menu" << endl;
						if(train_once_again())
						{
							looped = true;
							break;
						}
						else
							return true;
					}

					if(count<(number_of_cards-1))
					{
						dealer_hand.clear();
						get_random_card('D');
					}
					else
					{
						update_stats('S');
						cout << "\t\tYou won!" << endl;
						cout << "\t\tClick E to play once again or M to return to main menu" << endl;
						if(train_once_again())
							looped = true;
						else
							return true;
					}
				}
			}while(looped);


			cin.ignore();
			return true;
		}

			bool train_once_again()
			{
				char choice;

				do
				{
					choice = get_char();
				}while(choice!='M' and choice!='E');

				if(choice=='E')
					return true;

				return false;
			}

		// Core of the settings menu
		void settings_menu()
		{
			char input;
			unsigned short setting=1;
			unsigned long long temp_settings[5];

			for(unsigned long long setting=0; setting<5; setting++)
				temp_settings[setting] = settings[setting];

			do
			{
				do
				{
					print_settings_menu(temp_settings);
					wait(0.1);
					print_settings_menu(temp_settings, setting);
					input = get_char();

					if(input == 'W')
						setting--;
					else if(input == 'S')
						setting++;

					if(setting<1)
						setting = 7;
					else if(setting>7)
						setting = 1;

				}while(input != 'E');

				switch(setting)
				{
					// Enable/disable info about card shuffling
					case 1:
					{
						binary_settings_changer(setting, temp_settings);
						break;
					}

					// Enter number of decks for normal game
					case 2:
					{
						print_change_num_of_decks(temp_settings[1]);

						unsigned long long number_of_decks;

						while(!(cin >> number_of_decks) and number_of_decks<1)
						{
							cin.clear();
							cin.ignore(10000, '\n');
							clear_screen();
							print_change_num_of_decks(temp_settings[1]);
							cout << "\t\tInvalid value, try again." << endl << "\t\t";
						}

						temp_settings[1] = number_of_decks;

						break;
					}

					// Enter number of cards for card counting
					case 3:
					{
						print_change_num_of_cards(temp_settings[2]);

						unsigned long long number_of_cards;

						while(!(cin >> number_of_cards) and number_of_cards<1)
						{
							cin.clear();
							cin.ignore(10000, '\n');
							clear_screen();
							print_change_num_of_cards(temp_settings[2]);
							cout << "\t\tInvalid value, try again." << endl << "\t\t";
						}

						temp_settings[2] = number_of_cards;
						break;
					}

					case 4:
					{
						binary_settings_changer(setting, temp_settings);
						break;
					}

					case 5:
					{
						binary_settings_changer(setting, temp_settings);
						break;
					}
				}

			}while(setting<6);

			if(setting==6)
				update_settings(temp_settings);
		}

			void binary_settings_changer(unsigned short position, unsigned long long temp_settings[])
			{
				if(temp_settings[position-1] == 0)
					temp_settings[position-1] = 1;
				else
					temp_settings[position-1] = 0;
			}

			void print_change_num_of_decks(unsigned long long number_of_decks)
			{
				clear_screen();
				cout << "\t\tEnter number of decks you would like to use during normal game and confirm choice with Enter" << endl;
				cout << "\t\tCurrent number of decks:\t" << number_of_decks << endl << endl << "\t\t";
			}

			void print_change_num_of_cards(unsigned long long number_of_cards)
			{
				clear_screen();
				cout << "\t\tEnter number of cards you would like to use during card counting challenge and confirm choice with Enter" << endl;
				cout << "\t\tCurrent number of cards:\t" << number_of_cards << endl << endl << "\t\t";
			}
/* End of game logic */

/* Output/UI functions */
		// Game UI and its helper functions
		void print_game_ui()
		{
			cout << "\t\tPlayer\t\tPoints:   " << player_hand_sum << endl << endl;
			print_line_of_cards('P', true);

			cout << "\t\tDealer\t\tPoints:   " << dealer_known_points << endl << endl;
			print_line_of_cards('D', false);

			cout << endl << endl;
			cout << "\t\tH -- Hit" << endl;
			cout << "\t\tS -- Stand" << endl;
			cout << "\t\tM -- Exit to main menu" << endl;
		}

			void print_results()
			{
				clear_screen();

				cout << "\t\tPlayer\t\tPoints:   " << player_hand_sum << endl << endl;

				print_line_of_cards('P', true);

				cout << "\t\tDealer\t\tPoints:   " << dealer_hand_sum << endl << endl;

				print_line_of_cards('D', true);

				cout << endl << endl << endl;

				if(player_hand_sum>21 or (player_hand_sum<dealer_hand_sum and dealer_hand_sum<=21))
				{
					cout << "\t\tYOU LOST!" << endl;
					update_stats('L');
				}
				else if(player_hand_sum<=21 and(player_hand_sum>dealer_hand_sum or dealer_hand_sum>21))
				{
					cout << "\t\tYOU WON!" << endl;
					update_stats('W');
				}
				else
				{
					cout << "\t\tDRAW!" << endl;
					update_stats('D');
				}

				cout << endl;

				cout << "\t\tIf you want to play again press E, otherwise press M" << endl;
			}

			void print_line_of_cards(char dealer_or_player, bool visible)
			{
				string 	card_symbol, 
						card_color;

				switch(visible)
				{
					case true:
					{
						switch(dealer_or_player)
						{
							case 'P':
							{
								unsigned short num_of_cards = player_hand.size();
								cout << "\t\t";
								for(unsigned short card = 0; card<num_of_cards; card++)
									cout << "╔═════╗  ";

								cout << endl << "\t\t";;

								for(unsigned short card = 0; card<num_of_cards; card++)
								{
									card_symbol = get_card_symbol(player_hand[card]);
									cout << "║ " << card_symbol << "  ";
									if(card_symbol.size()==1)
										cout << " ";
									cout << "║  ";
								}

								cout << endl << "\t\t";

								for(unsigned short card = 0; card<num_of_cards; card++)
								{
									card_color = get_card_color(player_hand[card]);
									cout << "║  "<< card_color <<"  ║  ";
								}

								cout << endl << "\t\t";

								for(unsigned short card = 0; card<num_of_cards; card++)
								{
									card_symbol = get_card_symbol(player_hand[card]);
									cout << "║  "; 
									if(card_symbol.size()==1)
										cout << " ";
									cout << card_symbol << " ║  ";
								}

								cout << endl << "\t\t";

								for(unsigned short card = 0; card<num_of_cards; card++)
									cout << "╚═════╝  ";

								cout << endl << endl << endl;
								return;
							}
							case 'D':
							{
								unsigned short num_of_cards = dealer_hand.size();

								cout << "\t\t";
								for(unsigned short card = 0; card<num_of_cards; card++)
									cout << "╔═════╗  ";

								cout << endl << "\t\t";

								for(unsigned short card = 0; card<num_of_cards; card++)
								{
									card_symbol = get_card_symbol(dealer_hand[card]);
									cout << "║ " << card_symbol << "  ";
									if(card_symbol.size()==1)
										cout << " ";
									cout << "║  ";
								}

								cout << endl << "\t\t";

								for(unsigned short card = 0; card<num_of_cards; card++)
								{
									card_color = get_card_color(dealer_hand[card]);
									cout << "║  "<< card_color <<"  ║  ";
								}

								cout << endl << "\t\t";

								for(unsigned short card = 0; card<num_of_cards; card++)
								{
									card_symbol = get_card_symbol(dealer_hand[card]);
									cout << "║  "; 
									if(card_symbol.size()==1)
										cout << " ";
									cout << card_symbol << " ║  ";
								}

								cout << endl << "\t\t";

								for(unsigned short card = 0; card<num_of_cards; card++)
									cout << "╚═════╝  ";

								cout << endl << endl;
								return;
							}

						}
					}
					case false:
					{
							unsigned short num_of_cards = dealer_hand.size();

							cout << "\t\t";
							if(num_of_cards>0)
							{
								for(unsigned short card = 0; card<num_of_cards; card++)
									cout << "╔═════╗  ";

								cout << endl << "\t\t";


								card_symbol = get_card_symbol(dealer_hand[0]);
								cout << "║ " << card_symbol << "  ";
								if(card_symbol.size()==1)
									cout << " ";
								cout << "║  ";

								for(unsigned short card = 1; card<num_of_cards; card++)
								{
									card_symbol = get_card_symbol(dealer_hand[card]);
									cout << "║ └┬┘ ║  ";
								}

								cout << endl << "\t\t";


								card_color = get_card_color(dealer_hand[0]);
								cout << "║  "<< card_color <<"  ║  ";

								for(unsigned short card = 1; card<num_of_cards; card++)
								{
									card_color = get_card_color(dealer_hand[card]);
									cout << "║──┼──║  ";
								}

								cout << endl << "\t\t";


								card_symbol = get_card_symbol(dealer_hand[0]);
								cout << "║  "; 
								if(card_symbol.size()==1)
									cout << " ";
								cout << card_symbol << " ║  ";

								for(unsigned short card = 1; card<num_of_cards; card++)
								{
									card_symbol = get_card_symbol(dealer_hand[card]);
									cout << "║ ┌┴┐ ║  ";
								}

								cout << endl << "\t\t";

								for(unsigned short card = 0; card<num_of_cards; card++)
									cout << "╚═════╝  ";

								cout << endl << endl;
							}					
					}
				}

			}

		// Main menu and its helper functions
		void print_menu(unsigned short position=0)
		{
			clear_screen();

			print_logo();

			print_position_1_game(position);
			print_position_2_training(position);
			print_position_3_settings(position);
			print_position_4_help(position);
			print_position_5_stats(position);
			print_position_6_credits(position);
			print_position_7_exit(position);
			cout << endl << "\t\t\t     W -- up" << endl;
			cout << "\t\t\t     S -- down" << endl;
			cout << "\t\t\t     E -- confirm" << endl;
		}

			void print_position_1_game(unsigned short position)
			{
				if(position == 1)
					cout << "\t\t\t>>>  Standard game vs AI  <<<" << endl;
				else
					cout << "\t\t\t     Standard game vs AI" << endl;
			}

			void print_position_2_training(unsigned short position)
			{
				if(position == 2)
					cout << "\t\t\t>>>  Train Hi Lo card counting  <<<" << endl;
				else
					cout << "\t\t\t     Train Hi Lo card counting" << endl;
			}
			void print_position_3_settings(unsigned short position)
			{
				if(position == 3)
					cout << "\t\t\t>>>  Settings  <<<" << endl;
				else
					cout << "\t\t\t     Settings" << endl;
			}
			void print_position_4_help(unsigned short position)
			{
				if(position == 4)
					cout << "\t\t\t>>>  How to play blackjack?  <<<" << endl;
				else
					cout << "\t\t\t     How to play blackjack?" << endl;
			}
			void print_position_5_stats(unsigned short position)
			{
				if(position == 5)
					cout << "\t\t\t>>>  Stats  <<<" << endl;
				else
					cout << "\t\t\t     Stats" << endl;
			}
			void print_position_6_credits(unsigned short position)
			{
				if(position == 6)
					cout << "\t\t\t>>>  Credits/Info  <<<" << endl;
				else
					cout << "\t\t\t     Credits/Info" << endl;
			}
			void print_position_7_exit(unsigned short position)
			{
				if(position == 7)
					cout << "\t\t\t>>>  Exit  <<<" << endl;
				else
					cout << "\t\t\t     Exit" << endl;
			}

		// Settings menu and its helper functions
		void print_settings_menu(unsigned long long temp_settings[], unsigned short setting=0)
		{
			clear_screen();
			cout << "\t     Game settings:" << endl << endl << endl;

			print_setting_1_shuffle(setting, temp_settings);
			print_setting_2_decks(setting, temp_settings);
			print_setting_3_num_of_cards(setting, temp_settings);
			print_setting_4_card_counting_dealer(setting, temp_settings);
			print_setting_5_logo_startup(setting, temp_settings);
			print_setting_6_save(setting);
			print_setting_7_discard(setting);
		}
			
			void print_setting_1_shuffle(unsigned short setting, unsigned long long temp_settings[])
			{
				cout << "\t";
				if(setting==1)
					cout << ">>>  "; 
				else
					cout << "     "; 

				cout << "Display info when the cards are shuffled:   ";
				
				if(temp_settings[0])
					cout << "ENABLED";
				else
					cout << "DISABLED";

				cout << endl << endl;				
			}

			void print_setting_2_decks(unsigned short setting, unsigned long long temp_settings[])
			{
				cout << "\t";
				if(setting==2)
					cout << ">>>  "; 
				else
					cout << "     "; 

				cout << "Number of decks in use during normal game:   " << temp_settings[1];
				cout << endl << endl;
			}

			void print_setting_3_num_of_cards(unsigned short setting, unsigned long long temp_settings[])
			{
				cout << "\t";
				if(setting==3)
					cout << ">>>  "; 
				else
					cout << "     "; 

				cout << "Number of cards in use during card counting training:   " << temp_settings[2];
				cout << endl << endl;
			}

			void print_setting_4_card_counting_dealer(unsigned short setting, unsigned long long temp_settings[])
			{
				cout << "\t";
				if(setting==4)
					cout << ">>>  "; 
				else
					cout << "     "; 

				cout << "Dealer strategy:   ";
				if(temp_settings[3])
					cout << "Hi Lo card counting";
				else
					cout << "Hit if sum of cards is less than 17";	

				cout << endl << endl;			
			}

			void print_setting_5_logo_startup(unsigned short setting, unsigned long long temp_settings[])
			{
				cout << "\t";
				if(setting==5)
					cout << ">>>  "; 
				else
					cout << "     "; 

				cout << "Display game logo on startup:   ";
				if(temp_settings[4])
					cout << "ENABLED";
				else
					cout << "DISABLED";	

				cout << endl << endl;			
			}

			void print_setting_6_save(unsigned short setting)
			{
				cout << "\t";
				if(setting==6)
					cout << ">>>  "; 
				else
					cout << "     ";

				cout << "Save changes and exit to main menu";
				cout << endl << endl;
			}

			void print_setting_7_discard(unsigned short setting)
			{
				cout << "\t";
				if(setting==7)
					cout << ">>>  "; 
				else
					cout << "     ";

				cout << "Discard changes and exit to main menu";
				cout << endl << endl;
			}

		// Logo
		void print_logo(bool startup=false)
		{
			if(startup)
			{
				cout << "\t..         ....   ......   ......   ..             ..   ......   ......   ..\n" ;
				wait(0.2);
				cout << "\t..           ..       ..   ..       ..    ..                ..   ..       ..    ..\n";
				wait(0.2);
				cout << "\t..           ..   ......   ..       ..  ..         ..   ......   ..       ..  ..\n";
				wait(0.2);
				cout << "\t........     ..   ..  ..   ..       ....           ..   ..  ..   ..       ....\n";
				wait(0.2);
				cout << "\t..    ..     ..   ..  ..   ..       ..  ..     ..  ..   ..  ..   ..       ..  ..\n";
				wait(0.2);
				cout << "\t........     ..   ......   ......   ..    ..   ......   ......   ......   ..    .. \n";
			}
			else
				cout <<"\t..         ....   ......   ......   ..             ..   ......   ......   ..\n\t..           ..       ..   ..       ..    ..                ..   ..       ..    ..\n\t..           ..   ......   ..       ..  ..         ..   ......   ..       ..  ..\n\t........     ..   ..  ..   ..       ....           ..   ..  ..   ..       ....\n\t..    ..     ..   ..  ..   ..       ..  ..     ..  ..   ..  ..   ..       ..  ..\n\t........     ..   ......   ......   ..    ..   ......   ......   ......   ..    .. \n\n";
		}

		// Credits screen
		void print_credits()
		{
			clear_screen();
			print_logo();
			cout << endl;
			cout << "\t\tDeveloper: skelly37 (https://github.com/skelly37), 2021, MIT License";
			cout << endl << endl;
			cout << "\t\tMore terminal/terminal-like UI games:" << endl;
			cout << "\t\tMatrix: #terminal-gaming:matrix.org" << endl;
			cout << "\t\tTelegram: https://t.me/cligaming" << endl;
			cout << endl << endl;
			cout << "\t\tJoin us and help us with making TUI games more popular!" << endl;
			cout << "\t\tClick E to return to main menu, Q to exit the game" << endl;
		}

		// Help screen
		void print_help()
		{
			clear_screen();
			cout << "\t\tRules of Blackjack:" << endl << endl;
			cout << "\t\tGoal:\t\tBe as close to 21 points as possible" << endl;
			cout << "\t\tLose:\t\tBust or have less points than the opponent" << endl << endl << endl;
			cout << "\t\tGlossary:" << endl << endl;
			cout << "\t\tHit:\t\tGet another card" << endl;
			cout << "\t\tStand:\t\tBack out and wait for results" << endl;
			cout << "\t\tBust:\t\tExceed limit of 21 points and lose" << endl << endl << endl;
			cout << endl << "\t\tNot implemented here because they are useless without real money:" << endl << endl;
			cout << "\t\tDouble down:\tIf you have exactly two cards, you can double your bet\n\t\tand get exactly one card" << endl;
			cout << "\t\tSplit:\t\tIf you have exactly two cards with exactly same value,\n\t\tyou can double your bet and play with two independent hands" << endl;
			cout << "\t\tSurrender:\tSome casinos allow you to give up and lose only half of the bet\n\t\tbut only as the first decision" << endl;
			cout << endl << endl << "\t\tClick E to return to main menu, Q to exit the game" << endl;
		}

		// Stats screen and its helper functions
		void print_stats()
		{
			clear_screen();

			unsigned long long 	total_games = get_total_games(),
								total_counting_games = get_total_counting_games();

			cout << "\t\tYour stats:" << endl << endl << endl;
			
			cout << "\t\tStandard game" << endl << endl;
			cout << "\t\tTotal:\t" << setw(10) << total_games << endl;
			
			print_win_lose_draw('W', total_games);
			print_win_lose_draw('D', total_games);
			print_win_lose_draw('L', total_games);

			cout << endl << endl << "\t\tHi Lo card counting" << endl << endl;
			cout << "\t\tTotal:\t" << setw(10) << total_counting_games << endl;

			print_win_lose_draw('S', total_counting_games);
			print_win_lose_draw('U', total_counting_games);

			cout << endl;
			cout << "\t\tClick E to return to main menu, Q to exit the game" << endl;
		}
			void print_win_lose_draw(char result, unsigned long long total_games)
			{
				float percent; 
				switch(result)
				{
					case 'W':
					{
						unsigned long long wins = get_wins();
						cout << "\t\tWins:\t" << setw(10) << wins << "  (";
						percent = ((float)wins/total_games)*100;
						break;
					}
					case 'D':
					{
						unsigned long long draws = get_draws();
						cout << "\t\tDraws:\t" << setw(10) << draws << "  (";
						percent = ((float)draws/total_games)*100;
						break;
					}
					case 'L':
					{
						unsigned long long losses = get_losses();
						cout << "\t\tLosses:\t" << setw(10) << losses << "  (";
						percent = ((float)losses/total_games)*100;
						break;
					}
					case 'S':
					{
						unsigned long long wins = get_wins_in_counting();
						cout << "\t\tWins:\t" << setw(10) << wins << "  (";
						percent = ((float)wins/total_games)*100;
						break;
					}
					case 'U':
					{
						unsigned long long losses = get_losses_in_counting();
						cout << "\t\tLosses:\t" << setw(10) << losses << "  (";
						percent = ((float)losses/total_games)*100;
						break;
					}
				}
				if(percent<10)
					cout << setprecision(3) << percent;
				else if(percent<100)
					cout << setprecision(4) << percent;
				else
					cout << "100";
				cout << "%)" << endl;
			}
/* End of output/UI functions */

/* Others */
		// Clear terminal before leaving the game
		void nice_exit()
		{
			clear_screen();
			exit(0);
		}

		// Replace stats in the file with the current stats
		void update_stats_file()
		{
			ofstream statsfile;

			statsfile.open(".stats.bjg");

			for(unsigned short count=0; count<5; count++)
				statsfile << stats[count] << endl;

			statsfile.close();
		}

		// Replace settings in the file with the current settings
		void update_settings_file()
		{
			ofstream settingsfile;

			settingsfile.open(".settings.bjg");

			for(unsigned short count=0; count<5; count++)
				settingsfile << settings[count] << endl;

			settingsfile.close();

		}

		// After finishing each game, update stats -- both in class and in the file
		void update_stats(char game_status)
		{
			switch(game_status)
			{
				case 'W':
					stats[0]++;
					break;
				case 'D':
					stats[1]++;
					break;
				case 'L':
					stats[2]++;
					break;
				case 'S':
					stats[3]++;
					break;
				case 'U':
					stats[4]++;
					break;
			}

			update_stats_file();
		}

		// Update settings as the class attribute and make them persistent in the file
		void update_settings(unsigned long long temp_settings[])
		{
			for(unsigned long long setting=0; setting<5; setting++)
				settings[setting] = temp_settings[setting];
			
			update_settings_file();
		}
/* End of others */

public:
		Blackjack()
		{
			ifstream statsfile, settingsfile;

			statsfile.open(".stats.bjg");
			settingsfile.open(".settings.bjg");

			if(statsfile)
			{
				for(unsigned short count=0; count<5; count++)
					statsfile >> stats[count];
			}
			else
			{
				for(unsigned short count=0; count<5; count++)
					stats[count] = 0;

				update_stats_file();
			}

			statsfile.close();

			if(settingsfile)
			{
				for(unsigned short count=0; count<5; count++)
					settingsfile >> settings[count];
			}
			else
			{
				settings[0] = 1; 
				settings[1] = 5; 
				settings[2] = 50; 
				settings[3] = 0; 
				settings[4] = 1;
				update_settings_file();
			}

			settingsfile.close();

			if(logo_at_startup())
			{
				clear_screen();
				print_logo(logo_at_startup());
				wait(0.1);
			}
		}

		bool main_menu()
		{
			unsigned short current_position = 1;
			char input;

			do
			{
				do
				{
					print_menu();
					wait(0.1);
					print_menu(current_position);
					input = get_char();

					if(input == 'W')
						current_position--;
					else if(input == 'S')
						current_position++;

					if(current_position<1)
						current_position = 7;
					else if(current_position>7)
						current_position = 1;

				}while(input != 'E');

				switch(current_position)
				{
					case 3:
					{
						settings_menu();
						break;
					}
					case 4:
					{
						print_help();
						if(get_char()=='Q')
							nice_exit();
						break;
					}
					case 5:
					{
						print_stats();
						if(get_char()=='Q')
							nice_exit();
						break;
					}
					case 6:
					{
						print_credits();
						if(get_char()=='Q')
							nice_exit();
						break;
					}
					case 7:
					{
						nice_exit();
					}
				}
			}while(current_position>2);

			if(current_position==1)
				return game();
			else
				return training();
			clear_screen();
		}

		// inclusive range --- <min_value, max_value>
		static unsigned short get_random_number(unsigned short min_value, unsigned short max_value)
		{
			srand(time(0));
			return (rand() % (max_value - min_value + 1)) + min_value;
		}

		static void clear_screen()
		{
			#ifdef _WIN32
				system("cls");
			#else
				system("clear");
			#endif
		}

		static char get_char()
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

		static void wait(long double seconds)
		{
			long double miliseconds = seconds * 1000;
			#ifdef _WIN32
				Sleep(miliseconds);
			#else
				usleep(miliseconds*1000);
			#endif
		}
};


int main()
{
	Blackjack bj;
	bool once_again=false;

	do
	{
		once_again = bj.main_menu();
	}while(once_again);
	
}