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

#include "func.h"

void Fill_PanelFile(struct PanelFile *panel, int sizey, int sizex, int posy, int posx, char *dir_default){
    panel->window = newwin(sizey, sizex, posy, posx);
    box(panel->window, '|', '-');
    wrefresh(panel->window);
    panel->pos_select = 0;
    panel->dir = opendir(dir_default);
    panel->pos_start = 0;
    panel->pos_end = MAX_STR_PRINT;
}

int main(){
    initscr();
    start_color();	
    const int sizey_conp = 5;
    const int sizex_conp = 50;
    const int posy_ = 1;
    const int posx_conp = 1;
    init_pair(3,  COLOR_BLACK, COLOR_BLUE);

    init_pair(1,  COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    
    signal(SIGWINCH, sig_winch);
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    refresh();
    printw("File manager(demo)\n");
    char *dir_default = "./";
    struct PanelFile left_panel, right_panel;
    Fill_PanelFile(&left_panel, 30, 50, 1, 1, dir_default);
    Fill_PanelFile(&right_panel, 30, 50, 1, 55, dir_default);
    move(PRINT_INFO,1);
    printw("[ copy(1), cut(2), paste(3), delete(4) ]\n");
    move(PRINT_INFO+1,1);
    printw("buffer:\n"); 
    refresh();
    int key;
    int run_main_while = 1;
    int pos_set = 2;
    getcwd(left_panel.name_dir_current, FILENAME_MAX);
    getcwd(right_panel.name_dir_current, FILENAME_MAX);
    print_direct_set_select(&left_panel, 3);
    print_direct_set_select(&right_panel, 2);
    
    char *dir_buffer = NULL;
    struct PanelFile *select_panel = &left_panel;
    int panel = 0;
    char buffer[PATH_MAX];
    int con_buffer = 0;
    while(run_main_while){
        key = getch();
        switch(key){
            #ifdef DEBUG
            case 113:
                run_main_while = 0;     
                break;
            #endif
            case KEY_ESC:
                run_main_while = 0;     
                break;
            case MKEY_HOME:
                select_panel->pos_select = 0;
                select_panel->pos_end = MAX_STR_PRINT;
                select_panel->pos_start = 0;
                break;
            case KEY_UP:
                if(select_panel->pos_select > 0){
                    --select_panel->pos_select;
                    if(select_panel->pos_start > select_panel->pos_select){
                        --select_panel->pos_end;
                        --select_panel->pos_start;
                    }
                }
                break;
            case KEY_DOWN:

                ++select_panel->pos_select;
                if(select_panel->pos_end <= select_panel->pos_select){
                    ++select_panel->pos_end;
                    ++select_panel->pos_start;
                }
                break;
            case MKEY_ENTER:
                dir_buffer = set_dir_current(select_panel->dir, select_panel->pos_select);
                chdir(dir_buffer);
                getcwd(select_panel->name_dir_current, FILENAME_MAX);
                select_panel->pos_select = 0;
                break;
            case KEY_TAB:
                if(panel){
                    panel = 0;
                    select_panel->dir = opendir(select_panel->name_dir_current);
                    print_direct_set_select(select_panel, 2);
                    refresh();
                    select_panel = &left_panel;
                    select_panel->dir = opendir(select_panel->name_dir_current);
                    print_direct_set_select(select_panel, 3);
                }
                else{
                    panel = 1;
                    select_panel->dir = opendir(select_panel->name_dir_current);
                    print_direct_set_select(select_panel, 2);
                    refresh();
                    select_panel = &right_panel;
                    select_panel->dir = opendir(select_panel->name_dir_current);
                    print_direct_set_select(select_panel, 3);
                }
                continue;
                break;
            case MKEY_COPY:
                dir_buffer = set_dir_current(select_panel->dir, select_panel->pos_select);
                if(strcmp(dir_buffer, "..") == 0)
                    continue;
                move(PRINT_INFO+1, 1);
                printw("buffer: %s\n", dir_buffer);
                realpath(dir_buffer, buffer);
                #ifdef DEBUG
                    move(DEBUG_LINE2, 1);
                    printw("MKEY_COPY: dir_buffer = %s, buffer = %s\n",dir_buffer, buffer );
                    
                    
                #endif
                break;
            case MKEY_CUT:
                dir_buffer = set_dir_current(select_panel->dir, select_panel->pos_select);
                if(strcmp(dir_buffer, "..") == 0)
                    continue;
                move(PRINT_INFO+1, 1);
                printw("buffer: %s\n", dir_buffer);
                realpath(dir_buffer, buffer);
                #ifdef DEBUG
                    move(DEBUG_LINE2, 1);
                    printw("MKEY_CUT: dir_buffer = %s, buffer = %s\n",dir_buffer, buffer );
                    
                    
                #endif
                break;
            default:
                #ifdef DEBUG
                    move(DEBUG_LINE1, 1);
                    printw("key = %d\n", key);
                #endif
                break;
        }
        #ifdef DEBUG
            move(DEBUG_LINE1, 20);
            printw("pos = %d\n",select_panel->pos_select);
        #endif
        select_panel->dir = opendir(select_panel->name_dir_current);
        select_panel->pos_select = print_direct_set_select(select_panel, 3);
        refresh();
    }
    delwin(left_panel.window);
    delwin(right_panel.window);
    closedir(left_panel.dir);
    closedir(right_panel.dir);
    endwin();
    exit(EXIT_SUCCESS);
}
















