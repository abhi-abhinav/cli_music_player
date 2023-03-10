#include <stdio.h>
#include <stdlib.h>

// Define a struct to represent a song
typedef struct Song {
	char title[100];
	char artist[100];
	char genre[50];
	char filepath[200];
	struct Song *next;
} Song;

// Function prototypes
void printMenu();

// Main function
int main() {

	// Initialize the playlist to NULL
	Song *playlist = NULL;

	// Loop until the user chooses to quit
	int choice = 0;
	do {
		printMenu();
		scanf("%d", &choice);

	} while (choice != 4);

	return 0;
}

// Print the menu of options for the user
void printMenu() {
    printf("\n"
           "|------------------------------|\n"
	   "|      Cli Music Player        |\n"
           "|-------+----------------------|\n"
           "| Sl.no |       Menu           |\n"
           "|-------+----------------------|\n"
           "| 1.    | Display playlist     |\n"
           "| 2.    | Add song to playlist |\n"
           "| 3.    | Play playlist        |\n"
           "| 4.    | Quit                 |\n"
           "|-------+----------------------|\n"
           "Enter your choice: ");
}

