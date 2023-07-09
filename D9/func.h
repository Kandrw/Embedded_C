#include <curses.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>

#define MKEY_ENTER 10
#define KEY_ESC 27
#define KEY_TAB 9
#define MAX_STR_PRINT 26
#define MKEY_HOME 262

#define MKEY_COPY 49
#define MKEY_CUT 50
#define MKEY_PASTE 51
#define MKEY_DELETE 52

#define PRINT_INFO 32

#define DEBUG
#define DEBUG_LINE1 35
#define DEBUG_LINE2 37


struct PanelFile{
    WINDOW *window;
    char name_dir_current[FILENAME_MAX];
    DIR *dir;
    int pos_select;
    int pos_start; // по умолчанию 0
    int pos_end;
};

void sig_winch(int signo);
int print_direct_set_select(struct PanelFile *panel, int colorpair_box);
int count_file_dir(DIR *dir);
char *set_dir_current(DIR *dir, int pos);
