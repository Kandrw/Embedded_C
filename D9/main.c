#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>//path max

#define MKEY_ENTER 10

void sig_winch(int signo){
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}
int print_direct_set_select(WINDOW *win, DIR *dir, int pos){
    struct dirent *entry, *pred_entry;
    struct stat statbuf;
    int i = 0;
    int con = -1;
    wclear(win); 
    wattron(win, COLOR_PAIR(3));
    box(win, '|', '-');
    wmove(win, 2, 1);
    while ( (entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0)
            continue;
        lstat(entry->d_name, &statbuf);
        if(i == pos){
            wattron(win, COLOR_PAIR(1));
            con = i;
        }
        else{
            wattron(win, COLOR_PAIR(2));
        }
        if (S_ISDIR(statbuf.st_mode)) 
            wprintw(win, "%s/",entry->d_name);
        else{
            wprintw(win, "%s",entry->d_name);
        }
        ++i;
        wmove(win, 2+i, 1);
        pred_entry = entry;
    }
    if(con == -1){
        wmove(win, 2+i-1, 1);
        wattron(win, COLOR_PAIR(1));
        wprintw(win, "%s",pred_entry->d_name);
        con = i-1;
    }
    
    wrefresh(win);
    return con;
}
int count_file_dir(DIR *dir){
    struct dirent *entry;
    int i = 0;
    while ( (entry = readdir(dir)) != NULL) {
        ++i;
    }
    seekdir(dir, 0);
    return i;
}
char *set_dir_current(DIR *dir, int pos){
    struct dirent *entry;
    int i = 0;
    seekdir(dir, 0);
    while ( (entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0)
            continue;
        if(i == pos){
            seekdir(dir, 0);
            move( 16, 65);
            printw("--%s\n", entry->d_name);
            wrefresh(stdscr);
            return entry->d_name;
        }
        ++i;
    }
    move(25, 65);
    printw("Error: set_dir_current - %d\n", pos);
    refresh();
    return NULL;
}


int main(){
    initscr();
    start_color();	
    const int sizey_conp = 5;
    const int sizex_conp = 50;
    const int posy_ = 1;
    const int posx_conp = 1;
    init_pair(3,  COLOR_WHITE, COLOR_BLACK);
    init_pair(1,  COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    
    signal(SIGWINCH, sig_winch);
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    refresh();
    printw("File manager(demo)\n");
    WINDOW *panel_file = newwin(30, 50, 1, 1);
    box(panel_file, '|', '-');
    wrefresh(panel_file);   
    refresh();
    int key;
    int run_main_while = 1;
    DIR *dir;
    struct dirent *entry;
    char *dir_default = "./";
    dir = opendir(dir_default);
    int pos_set = 2;
    char *dir_current = dir_default;
    char dir_buffer[FILENAME_MAX];
    getcwd(dir_buffer, FILENAME_MAX);
    print_direct_set_select(panel_file, dir, pos_set);
    while(run_main_while){
        key = getch();
        switch(key){
            case 113:
                run_main_while = 0;     
                break;
            case KEY_UP:
                if(pos_set > 0)
                    --pos_set;
                break;
            case KEY_DOWN:
                    ++pos_set;
                break;
            case MKEY_ENTER:
                dir_current = set_dir_current(dir, pos_set);
                chdir(dir_current);
                getcwd(dir_buffer, FILENAME_MAX);
                pos_set = 0;
                break;
            default:
                break;
        }
        move(35, 1);
        printw(" %s |cf = %d, pos = %d\n",dir_buffer, count_file_dir(dir), pos_set);
        dir = opendir(dir_buffer);
        
        if(!dir){
            move(14, 65);
            printw("Error: not dir - %s\n", dir_current);
            break;
        }
        pos_set = print_direct_set_select(panel_file, dir, pos_set);
        refresh();
    }
    //key = getch();
    closedir(dir);
    delwin(panel_file);

    endwin();
    exit(EXIT_SUCCESS);
}
















