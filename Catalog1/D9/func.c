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

#include "func.h"

void sig_winch(int signo){
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}
int print_direct_set_select(struct PanelFile *panel, int colorpair_box){
    WINDOW *win = panel->window;
    DIR *dir = panel->dir;
    int pos = panel->pos_select;

    struct dirent *entry, *pred_entry;
    struct stat statbuf;
    int i = 0, i2 = 0;
    int con = -1;
    wclear(win); 
    wattron(win, COLOR_PAIR(colorpair_box));
    box(win, '|', '-');
    wmove(win, 2, 1);
    while ( (entry = readdir(dir)) != NULL) {
        if(i >= panel->pos_start){
            
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
            ++i2;
            wmove(win, 2+i2, 1);
            pred_entry = entry;
            if(i == panel->pos_end){
                if(con == -1){
                    wmove(win, 2+i2-1, 1);
                    wattron(win, COLOR_PAIR(1));
                    wprintw(win, "%s",pred_entry->d_name);
                    con = i-1;
                }
                break;
            }
        }
        ++i;
    }
    if(con == -1){
        wmove(win, 2+i2-1, 1);
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
            wrefresh(stdscr);
            return entry->d_name;
        }
        ++i;
    }
    refresh();
    return NULL;
}
