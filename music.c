#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h> // for srand() and rand()
#include "util.h"
#include "musicController.h"


/* If we are compiling on Windows, compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char *readline(char *prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

/* Fake add_history function */
void add_history(char *unused) {}

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif



// Function prototypes
void printMenu();
void displayPlaylist(Song *playlist);
void addSongToPlaylist(Song **playlist);
void musicController(Song *playlist);
void savePlaylist(Song *playlist, char *filename);
Song *loadPlaylist(char *filename);
void removeSongFromPlaylist(Song **playlist);
void printTableHeader();
void printTableRow(Song *song, int index);
void printTableFooter();
void clearTable();

// Main function
int main()
{

    /* Print version and Exit information */
    puts("CLI Music Player Version 0.0.0.0.1");
    puts("Press Ctrl+z to Exit\n");

    // Initialize the playlist to NULL
    Song *playlist = NULL;
    char *filename = "playlist.txt";

    // Load the playlist from a file, if it exists
    playlist = loadPlaylist(filename);

    // Loop until the user chooses to quit
    int choice = 0;
    do
    {
        printMenu();
        printf("\n\033[38;2;255;165;0m ");
        char *option = readline("Enter your choice: ");
        int choice = atoi(option);
        /* Add history */
        add_history(option);
        /* Free retrieved input */
        free(option);

        switch (choice)
        {
        case 1:
            displayPlaylist(playlist);
            break;
        case 2:
            addSongToPlaylist(&playlist);
            break;
        case 3:
            removeSongFromPlaylist(&playlist);
            break;
        case 4:
            musicController(playlist);
            break;
        case 5:
            savePlaylist(playlist, filename);
            printf("\nPlaylist saved to file '%s'.\n", filename);
            printf("Goodbye!\n");
            exit(0);
            break;
        default:
            printf("\033[1;31mInvalid choice. Try again.\n\n");
        }
    } while (choice != 5);

    return 0;
}

char *getFilePath(char *dir_path, int choice);
void addSongToPlaylist(Song **playlist)
{   
    // Get the music directory path from the user
    char *dir_path = readline("Enter the path of the music directory: ");
    /* Add history */
    add_history(dir_path);

    // Open the music directory
    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        printf("\n\033[1;31mError: Could not open music directory.\n");
        return;
    }

    // Display the list of songs in the music directory
    printf("\n\033[1;33mSongs in directory:\n");
    struct dirent *entry;
    int i = 1;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            printf("\n\033[1;35m%d. \n\033[1;36m%s\n\033[0m\n", i, entry->d_name);
            i++;
        }
    }
    int already_added = 0;
    // Get the user's choice of song
    char *choice_str;
    int choice;
    do
    {
        choice_str = readline("Enter the number of the song you want to add: ");
        /* Add history */
        add_history(choice_str);
        choice = atoi(choice_str);
        free(choice_str);

        // Check if the input is valid
        if (choice < 1 || choice >= i)
        {
            printf("\n\033[1;31mError: Invalid input. Please enter a number between 1 and %d.\n", i - 1);
        }
        else
        {
            // Check if the selected song has already been added to the playlist

            Song *current = *playlist;
            while (current != NULL)
            {
                if (strcmp(current->filepath, getFilePath(dir_path, choice)) == 0)
                {
                    already_added = 1;
                    break;
                }
                current = current->next;
            }
            if (already_added)
            {
                printf("\n\033[1;31mError: This song has already been added to the playlist.\n");
            }
        }
    } while (choice < 1 || choice >= i || already_added);

    // Find the file path of the chosen song/* condition */
    char filepath[200];
    strcpy(filepath, getFilePath(dir_path, choice));

    // Get the song title and artist from the user
    char *title;
    int title_exists;
    do
    {
        title = readline("Enter the song title: ");
        /* Add history */
        add_history(title);

        // Check if the title already exists in the playlist
        title_exists = 0;
        Song *current = *playlist;
        while (current != NULL)
        {
            if (strcmp(current->title, title) == 0)
            {
                title_exists = 1;
                break;
            }
            current = current->next;
        }
        if (title_exists)
        {
            printf("\n\033[1;31mError: A song with this title already exists in the playlist. Please enter a different title.\n");
        }
    } while (title_exists);
    char *artist = readline("Enter the artist name: ");
    Song *newSong = (Song *)malloc(sizeof(Song));
    strcpy(newSong->title, title);
    strcpy(newSong->artist, artist);
    strcpy(newSong->filepath, filepath);
    /* Add history */
    add_history(title);
    add_history(artist);
    newSong->next = NULL;

    // Add the new song to the end of the playlist
    if (*playlist == NULL)
    {
        *playlist = newSong;
        newSong->prev = NULL;
    }
    else
    {
        Song *current = *playlist;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newSong;
        newSong->prev = current;
    }

    printf("\033[1;36m%s\033[0m by \033[1;36m%s\033[0m has been added to the playlist.\n", title, artist);

    // Free the memory allocated for the strings
    free(title);
    free(artist);

    // Close the directory
    closedir(dir);
}
void musicController(Song *playlist)
{
    if (playlist == NULL)
    {
        printf("\033[1;31mPlaylist is empty.\033[0m\n");
        return;
    }

    int choice = 0;
    Song *current = playlist;
    Song *previous = NULL;

    // initialize table
    clearTable();
    printTableHeader();

    while (choice != 3)
    {
        clearTable();

        // print current song in green
        printTableRow(current, 0);

        // print next songs in gray
        Song *next = current->next;
        int i = 1;
        while (next != NULL && i < MAX_TABLE_ROWS)
        {
            printTableRow(next, i);
            next = next->next;
            i++;
        }

        printTableFooter();

        // Start a new thread to play the audio file
        pthread_t audio_thread;
        pthread_create(&audio_thread, NULL, play_audio, (void *)current->filepath);
        bool is_playing = true; // set flag to indicate audio is playing

        // Wait for user input
        do
        {
            printf("Enter\n1 = to move to the next song\n2 = to move to the previous song\n3 = to stop playback: ");
            scanf("%d", &choice);
        } while (choice != 1 && choice != 2 && choice != 3);

        // Stop the audio and join the thread before moving to the next song
        if (is_playing)
        {
            system("killall ffplay");         // stop the audio
            pthread_join(audio_thread, NULL); // wait for the thread to finish
            is_playing = false;               // set flag to indicate audio is no longer playing
        }

        // Move to the next or previous song, or stop playback
        if (choice == 1)
        {
            if (current->next != NULL)
            {
                previous = current;
                current = current->next;
            }
            else
            {
                printf("End of playlist reached!!!.\n");
                choice = 3;
            }
        }
        else if (choice == 2)
        {
            if (previous != NULL)
            {
                current = previous;
                previous = previous->prev;
            }
            else
            {
                printf("Beginning of playlist reached!!!.\n");
                choice = 3;
            }
        }
        else
        {
            printf("Stop.\n");
        }
    }

    // clear table
    clearTable();
}
