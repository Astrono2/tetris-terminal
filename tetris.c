#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include "pieces.h"

int input, rot, piece, nextPiece, x, y, cx, cy, escape, inputCount, rotCount, frameCount, paused;
int points, level, prevLevel, lines;
int newPiece(void);
int sideColLeft(void);
int sideColRight(void);
int lose(int finalPoints, int finalLevel);
int levelColor(int colIndex);
int UI(int currentLine, int isPaused);
int nextPieceLine(int cl);

int main()
{

	cx = 2;
	cy = 1;

	//initializing the grid
	int i;
	int j;
	for(i = 0; i < 20; i++)
	{
		for(j = 0; j < 10; j++)
		{
			grid[i][j] = 0;
			gridDisplay[i][j][0] = '_';
			gridDisplay[i][j][1] = '_';
			gridDisplay[i][j][2] = '\0';
		}
	}

	//initializing first piece
	escape = 0;
	paused = 0;
	points = 0;
	level = 0;
	prevLevel = 0;
	lines = 0;
	srand(time(NULL));
	nextPiece = rand()%7;
	newPiece();
	y--;

	levelColor(level);
	system("cls");

	//game loop
	while(escape == 0)
	{

		//key pressed
		if(kbhit())
		{
			input = _getch();
			if(input == 27) escape = 1;
			else if(input == 'p' && paused == 0){ paused = 1; frameCount = 0; }
			else if(input == 'p' && paused == 1){ paused = 0; frameCount = 0; }
			else if(inputCount < 15 && (input == 224 || input == 0))
			{

				switch(_getch()){
            	case 72:
            		if(paused == 0) inputCount--; //upn't
            		break;
            	case 80:
            		if(paused == 0)
            		{
	            		y++; //down
	            		points++;
	            		inputCount--;
            		}
            		break;
            	case 75:
            		if(paused == 0) x -= sideColLeft(); //returns 1 (x-1 = move left) if there's no collision.
            		break;
            	case 77:
            		if(paused == 0) x += sideColRight(); //returns 1 (x+1 = move right) if there's no collision.
            		break;
            	default:
            		break;
            	}

            	if(paused == 0)
            	{
            		inputCount++;
            		frameCount = 0;
            	}
			}
			else if(input == 'x' && rotCount < 10 && paused == 0)
			{
				rot = (rot+1)%4;
				frameCount = 0;
				rotCount++;
				if(sideColRight() == 0)
				{
					rot--;
					rotCount--;
				}
				else if(sideColLeft() == 0)
				{
					rot--;
					rotCount--;
				}
			}
			else if(input == 'z' && rotCount < 10 && paused == 0)
			{
				rot--;
				frameCount = 0;
				rotCount++;
				if(sideColRight() == 0)
				{
					rot = (rot+1)%4;
					rotCount--;
				}
				else if(sideColLeft() == 0)
				{
					rot = (rot+1)%4;
					rotCount--;
				}
			}
			if(rot < 0) rot = 3;
		}

		//key not pressed in this frame
		else
		{
			if(frameCount == 0 && paused == 0){
				y++;
				inputCount = 0;
				rotCount = 0;
			}
		}

		//game logic
		if(x < 4) //left side of the screen
		{
			int repTwice;
			for(repTwice = 0; repTwice < 2; repTwice++) //repeats the check twice just in case (this catches a bug when rotating against the edge of the screen)
			{
				int subtract = 0;
				for(j = 0; j < 2; j++)
				{
					int count = 0;
					for(i = 0; i < 4; i++)
					{
						if(pieces[piece][rot][i][j] == 1) count++;
					}
					if(count > 0) subtract++;
				}
				if(x-subtract < 0) x++;
			}
		}
		else if(x > 7)
		{
			int repTwice;
			for(repTwice = 0; repTwice < 2; repTwice++)
			{
				int add = 0;
				for(j = 0; j < 2; j++)
				{
					int count = 0;
					for(i = 0; i < 4; i++)
					{
						if(pieces[piece][rot][i][3-j] == 1) count++;
					}
					if(count > 0) add++;
				}
				if(x+add > 10) x--;
			}
		}

		//bottom of the screen
		if(y>17)
		{
			int add = 0;
			for(i = 0; i < 3; i++)
			{
				int count = 0;
				for(j = 0; j < 4; j++)
				{
					if(pieces[piece][rot][3-i][j] == 1) count++;
				}
				if(count > 0) add++;
			}
			if(y+add > 20)
			{
				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 4; j++)
					{
						if(pieces[piece][rot][i][j] == 1) grid[y-cy+i-1][x-cx+j] = 1;
					}
				}
				newPiece();
				inputCount = 0;
				rotCount = 0;
			}
		}

		//piece below
		{
			int contact = 0;
			for(i = 0; i < 4; i++)
			{
				for(j = 0; j < 4; j++)
				{
					if(pieces[piece][rot][i][j] == 1 && grid[y-cy+i][x-cx+j] == 1) contact = 1;
				}
			}
			if(contact == 1)
			{
				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 4; j++)
					{
						if(pieces[piece][rot][i][j] == 1 && y-cy+i > 0) grid[y-cy+i-1][x-cx+j] = 1;
						if(y-cy+i < 0) lose(points, level);
					}
				}
				newPiece();
				inputCount = 0;
				rotCount = 0;
			}
		}

		//lines logic
		{
			int amountOfLines = 0;
			int whatLines[4] = {-1, -1, -1, -1};
			for(i = 0; i < 20; i++)
			{
				int count = 0;
				for(j = 0; j < 10; j++)
				{
					if(grid[i][j] == 1) count++;
				}
				if(count == 10)
				{
					amountOfLines++;
					whatLines[amountOfLines-1] = i;
				}
			}

			if(amountOfLines > 0) //there's one or more lines
			{
				int repTwice;
				for(repTwice = 0; repTwice < 2; repTwice++) //this loop will flash the lines empty and full twice
				{
					system("cls");
					for(i = 0; i < 20; i++)
					{
						if(i == whatLines[0] || i == whatLines[1] || i == whatLines[2] || i == whatLines[3])
						{
							printf("%s", emptyLine);
						}
						else
						{
							for(j = 0; j < 10; j++)
							{
								printf("%s", gridDisplay[i][j]);
							}
						}
						printf("\n");
					}

					fflush(stdout);
					Sleep(100);

					system("cls");
					for(i = 0; i < 20; i++)
					{
						if(i == whatLines[0] || i == whatLines[1] || i == whatLines[2] || i == whatLines[3])
						{
							printf("%s", fullLine);
						}
						else
						{
							for(j = 0; j < 10; j++)
							{
								printf("%s", gridDisplay[i][j]);
							}
						}
						printf("\n");
					}

					fflush(stdout);
					Sleep(100);
				}

				//actually making the lines disappear
				for(j = 0; j < amountOfLines; j++)
				{
					for(i = 0; i < whatLines[j]+1; i++)
					{
						int k;
						for(k = 0; k < 10; k++)
						{
							if(i != whatLines[j]) grid[whatLines[j]-i][k] = grid[whatLines[j]-i-1][k];
							else grid[0][k] = 0;
						}
					}
				}

				lines++;
				points += pointsPerLine[amountOfLines] * (level+1);
			}

			//levels logic (this doesn't really make any sense because of the naming of the variable prevLevel but who gives a fuck at this point)
			if(lines%15 == 0 && prevLevel == level && lines != 0)
			{
				prevLevel = level;
				level++;
				levelColor(level);
			}
			else if(lines%15 == 1)
			{
				prevLevel = level;
			}
		}



		//updating gridDisplay
		if(frameCount == 0)
		{
			for(i = 0; i < 20; i++)
			{
				for(j = 0; j < 10; j++)
				{
					if(grid[i][j] == 1 || (i-y+cy >= 0 && i-y+cy < 4 && j-x+cx >= 0 && j-x+cx < 4 && pieces[piece][rot][i-y+cy][j-x+cx] == 1))
					{
						gridDisplay[i][j][0] = '[';
						gridDisplay[i][j][1] = ']';
						gridDisplay[i][j][2] = '\0';
					}
					else
					{
						gridDisplay[i][j][0] = '_';
						gridDisplay[i][j][1] = '_';
						gridDisplay[i][j][2] = '\0';
					}
				}
			}
		}

		//drawing
		if(frameCount == 0 && escape == 0 && paused == 0)
		{
			system("cls");
			for(i = 0; i < 20; i++)
			{
				for(j = 0; j < 10; j++)
				{
					printf("%s", gridDisplay[i][j]);
				}

				UI(i, paused);

				printf("\n");
			}
		}

		else if(frameCount == 0 && escape == 0 && paused == 1)
		{
			system("cls");
			printf("The game has been paused.\nPress 'P' again to resume.\n\n");
			printf("CONTROLS:\n	LEFT:                LEFT ARROW KEY\n	RIGHT:               RIGHT ARROW KEY\n	DOWN:                DOWN ARROW KEY\n	CLOCKWISE TURN:      'X' KEY\n	ANTI-CLOCKWISE TURN: 'Z' KEY\n	PAUSE:               'P' KEY\n	CLOSE THE GAME:       ESCAPE KEY\n");
			printf("\nIf you want to have a good time I recommend not spamming keys, as the game will start lagging.");
			printf("\n\n\n\n\n\n\n\nTrademark 2019 The Montani International Corporation & Associates.");
		}

		if(paused == 0) frameCount = (frameCount+1)%(20000-20*level); else frameCount = 1;

	}

	return 0;
}



int newPiece(void)
{
	rot = 0;
	x = 4;
	y = 0;
	piece = nextPiece;
	nextPiece = rand()%7;

	return 0;
}

int sideColLeft(void)
{
	int i, j;
	int contact = 0;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(pieces[piece][rot][i][j] == 1 && grid[y-cy+i][x-cx+j-1] == 1 && y-cy+i >= 0) contact = 1;
		}
	}
	return 1-contact; //returns 1 if no contact, 0 if contact
}

int sideColRight(void)
{
	int i, j;
	int contact = 0;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(pieces[piece][rot][i][j] == 1 && grid[y-cy+i][x-cx+j+1] == 1 && y-cy+i >= 0) contact = 1;
		}
	}
	return 1-contact; //returns 1 if no contact, 0 if contact
}

int lose(int finalPoints, int finalLevel)
{
	char *suffix;
	system("cls");
	printf("You just lost the game lmao, Shikaka.\n");
	printf("You got %d points.\n", finalPoints);

	if(finalLevel%10 == 1 && finalLevel != 11) suffix = "st";
	else if(finalLevel%10 == 2 && finalLevel != 12) suffix = "nd";
	else if(finalLevel%10 == 3 && finalLevel != 13) suffix = "rd";
	else suffix = "th";

	printf("You got to the %d%s level.", finalLevel, suffix);
	printf("\n\n\n\n\n\n\n\nTrademark 2019 The Montani International Corporation & Associates.\n");
	system("pause");

	escape = 1;

	return 0;
}

int levelColor(int colIndex)
{
	switch(colIndex%10)
	{
		case 0:
			system("color 9");
			break;
		case 1:
			system("color A");
			break;
		case 2:
			system("color D");
			break;
		case 3:
			system("color 6");
			break;
		case 4:
			system("color 7");
			break;
		case 5:
			system("color 2");
			break;
		case 6:
			system("color 1");
			break;
		case 7:
			system("color E");
			break;
		case 8:
			system("color 4");
			break;
		case 9:
			system("color C");
			break;
	}
}

int UI(int currentLine, int isPaused)
{
	printf("	");
	switch(currentLine)
	{
		case 0:
			printf("*-------------------*");
			break;
		case 1:
			printf("|     LEVEL: %02d     |", level);
			break;
		case 2:
			printf("| POINTS: %09d |", points);
			break;
		case 3:
			printf("|     LINES: %02d     |", lines);
			break;
		case 4:
			printf("*-------------------*");
			break;
		case 6:
			printf("     NEXT PIECE:");
			break;
		case 7:
			printf("     *----------*");
			break;
		case 8:
			printf("     ");
			nextPieceLine(0);
			break;
		case 9:
			printf("     ");
			nextPieceLine(1);
			break;
		case 10:
			printf("     ");
			nextPieceLine(2);
			break;
		case 11:
			printf("     ");
			nextPieceLine(3);
			break;
		case 12:
			printf("     *----------*");
			break;
		default:
			break;
	}
}

int nextPieceLine(int cl)
{
	int k;
	printf("| ");
	for(k = 0; k < 4; k++)
	{
		if(pieces[nextPiece][0][cl][k] == 1) printf("[]");
		else printf("__");
	}
	printf(" |");
	return 0;
}
