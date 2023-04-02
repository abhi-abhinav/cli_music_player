#include <time.h> // for srand() and rand()
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <editline/readline.h>
#include <editline/history.h>
#define MAX_SONGS 1000       // maximum number of songs in the playlist
#define MAX_TITLE_LENGTH 100 // maximum length of a song title
// Define a struct to represent a song
typedef struct Song
{
    char title[100];
    char artist[100];
    char filepath[200];
    char playlist[100];
    struct Song *next;
    struct Song *prev;
} Song;

void savePlaylist(Song *playlist, char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("\nError: Could not open file for writing.\n");
        return;
    }

    Song *current = playlist;
    while (current != NULL)
    {
        fprintf(fp, "%s;%s;%s\n", current->title, current->artist, current->filepath);
        current = current->next;
    }

    fclose(fp);
}

Song *loadPlaylist(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("\nPlaylist file not found. Starting with an empty playlist.\n");
        return NULL;
    }

    Song *playlist = NULL;
    char line[200];
    while (fgets(line, sizeof(line), fp))
    {
        char *title = strtok(line, ";");
        char *artist = strtok(NULL, ";");
        char *filepath = strtok(NULL, ";");
        filepath[strcspn(filepath, "\n")] = 0; // Remove the trailing newline character

        Song *newSong = (Song *)malloc(sizeof(Song));
        strcpy(newSong->title, title);
        strcpy(newSong->artist, artist);
        strcpy(newSong->filepath, filepath);
        newSong->next = playlist;
        playlist = newSong;
    }

    fclose(fp);
    return playlist;
}

void removeSongFromPlaylist(Song **playlist)
{
    if (*playlist == NULL)
    {
        printf("\033[1;31mPlaylist is empty!!!\033[0m\n");
    }
    else
    {
        // Get the song title to remove from the playlist
        char *title = readline("Enter the title of the song to remove from the playlist: ");
        /* Add history */
        add_history(title);

        // Search for the song with the given title
        Song *current = *playlist;
        Song *prev = NULL;
        while (current != NULL)
        {
            if (strcmp(current->title, title) == 0)
            {
                // Remove the song from the playlist
                if (prev == NULL)
                {
                    *playlist = current->next;
                }
                else
                {
                    prev->next = current->next;
                }
                free(current);
                printf("\n\033[1;32m'%s' has been removed from the playlist.\n", title);
                return;
            }
            prev = current;
            current = current->next;
        }

        // Song not found
        printf("\n\033[1;31mError: '%s' not found in the playlist.\n", title);
    }
}

// Print the menu of options for the user
void printMenu()
{
    printf("\033[1;35mcli\033[0m \033[1;32m >> \033[0m");
    printf("\n"
           "\033[1;34m|------------------------------|\033[0m\n"     // Blue text for top separator
           "\033[1;36m|      Cli Music Player        |\033[0m\n"     // Cyan text for title
           "\033[1;34m|-------+----------------------|\033[0m\n"     // Blue text for middle separator
           "\033[1;37m| Sl.no |       Menu           |\033[0m\n"     // White text for column headers
           "\033[1;34m|-------+----------------------|\033[0m\n"     // Blue text for middle separator
           "\033[1;37m| 1.    | Display playlist     |\033[0m\n"     // White text for menu options
           "\033[1;37m| 2.    | Add song to playlist |\033[0m\n"     // White text for menu options
           "\033[1;37m| 3.    | Remove song          |\033[0m\n"     // White text for menu options
           "\033[1;37m| 4.    | Play playlist        |\033[0m\n"     // White text for menu options
           "\033[1;37m| 5.    | Quit                 |\033[0m\n"     // White text for menu options
           "\033[1;34m|-------+----------------------|\033[0m\n\n"); // Blue text for bottom separator
}

char *getFilePath(char *dir_path, int choice)
{
    DIR *dir = opendir(dir_path);
    struct dirent *entry;
    int i = 1;
    char *filepath = (char *)malloc(200 * sizeof(char));
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            if (i == choice)
            {
                strcpy(filepath, dir_path);
                strcat(filepath, "/");
                strcat(filepath, entry->d_name);
                closedir(dir);
                return filepath;
            }
            i++;
        }
    }
    closedir(dir);
    return NULL;
}

// Display the contents of the playlist
void displayPlaylist(Song *playlist)
{
    if (playlist == NULL)
    {
        printf("\n\033[1;31mPlaylist is empty.\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*-*-\n\n");
    }
    else
    {
        printf("\033[1mPlaylist:\033[0m\n");
        printf("\033[1;36m+--------------------------------+----------------------+\033[0m\n");
        printf("\033[1;33m| %-30s | %-20s |\033[0m\n", "Title", "Artist");
        printf("\033[1;36m+--------------------------------+----------------------+\033[0m\n");
        Song *current = playlist;
        while (current != NULL)
        {
            printf("| %-30s | %-20s |\n", current->title, current->artist);
            current = current->next;
        }

        printf("\033[1;36m+--------------------------------+----------------------+\033[0m\n");
    }
}