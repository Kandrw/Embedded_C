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

#include <sys/wait.h>

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

    seekdir(dir, 0);
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
                    if (S_ISDIR(statbuf.st_mode)) 
                        wprintw(win, "%s/", pred_entry->d_name);
                    else{
                        wprintw(win, "%s", pred_entry->d_name);
                    }
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
        if (S_ISDIR(statbuf.st_mode)) 
            wprintw(win, "%s/", pred_entry->d_name);
        else{
            wprintw(win, "%s", pred_entry->d_name);
        }
        con = i-1;
    }
    seekdir(dir, 0);
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

int count_space_str(char *str){
    int i = 0;
    int n = 0;
    while(str[i]){
        if(i > 0){
            if(str[i] == ' ' && str[i-1] != ' ')
                n++;
        }
        ++i;
    }
    return n;
}
int generate_process(char *buffer_command){
    char *iter_str;
    int i = 0;
    char **exec_cmd = NULL;
    int count_cmd = 0;
    count_cmd = count_space_str(buffer_command);
    count_cmd += 2;
    exec_cmd = (char**)malloc(sizeof(char*) * count_cmd);
    exec_cmd[i++] = strtok(buffer_command, " ");
    while((iter_str = strtok(NULL, " ")) != NULL){
        exec_cmd[i++] = iter_str;
    }
    exec_cmd[i] = NULL;
    #ifdef DEBUG
        move(DEBUG_LINE3, 20);
        for(i = 0; exec_cmd[i] != NULL; ++i){
            printw("%s ", exec_cmd[i]);
        }
    #endif
    pid_t pid;
    int status;
    char c;
    int link[2];

    clear();
    keypad(stdscr, FALSE);
    endwin();
    pid = fork();
    if(pid == 0){
        //status = execv("/bin/zsh", exec_cmd);
        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        status = execvp(exec_cmd[0], exec_cmd);
        printf("invalid command %s, status = %d\n",exec_cmd[0], status);
        free(exec_cmd);
        exit(1);
    }
    wait(&status);
    printf("Restart\n");
    printf("command - %s, status = %d\n",exec_cmd[0], WEXITSTATUS(status));
    scanf("%*c", &c);
    free(exec_cmd);
    return i;
}