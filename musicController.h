#define MAX_TABLE_ROWS 10
bool is_playing = false; // flag variable to indicate if audio is playing
pthread_t audio_thread;  // thread for playing audio
// Audio function
void *play_audio(void *filepath)
{
    char *filename = (char *)filepath;
    char command[500];
    //   sprintf(command, "mpv --no-video --no-terminal \"%s\"", filename);
    sprintf(command, "ffplay -autoexit -nodisp -loglevel panic  \"%s\"", filename);
    system(command);
    is_playing = false; // set flag to indicate audio is no longer playing
    return NULL;
}
void printTableHeader()
{
    printf("\033[1;37m| %-3s | %-30s | %-20s |\033[0m\n", "#", "Title", "Artist");
    printf("\033[1;37m|====|================================|====================|\033[0m\n");
}

void printTableRow(Song *song, int index)
{
    if (song == NULL)
    {
        printf("| %-3s | %-30s | %-20s |\n", "", "", "");
    }
    else
    {
        if (index == 0)
        {
            printf("\033[1;32m");
        }
        else
        {
            printf("\033[0;37m");
        }
        printf("| %-3d| %-30s | %-20s |\n", index + 1, song->title, song->artist);
    }
}

void printTableFooter()
{
    printf("\033[1;37m|====|================================|======================|\033[0m\n");
}

void clearTable()
{
    system("clear");
}



// Controllers
