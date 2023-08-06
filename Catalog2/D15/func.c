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
    
    pid_t pid;
    int status;
    clear();
    keypad(stdscr, FALSE);
    endwin();
    #ifdef DEBUG
        for(i = 0; exec_cmd[i] != NULL; ++i){
            printf("%s ", exec_cmd[i]);
        }
        printf("\n");
    #endif
    pid = fork();
    if(pid == 0){
        int fides[2];
        pipe(fides);
        int st = 0;
        int i2;
        int count_process = 1;
        pid_t pid_iter;
        for(i = 0; exec_cmd[i] != NULL; ++i){
            if(strcmp("|", exec_cmd[i]) == 0){
                exec_cmd[i] = NULL;
                pid_iter = fork();
                if(pid_iter == 0){
                    dup2(fides[0], 0);
                    dup2(fides[1], 1);
                    status = execvp(*(exec_cmd+st), exec_cmd+st);
                    printf("invalid command %s, status = %d\n",*(exec_cmd+st), status);
                    exit(count_process);
                }
                st = i+1;
                count_process++;
            }
        }
        pid_iter = fork();
        if(pid_iter == 0){
            dup2(fides[0], 0);
            status = execvp(*(exec_cmd+st), exec_cmd+st);
            printf("invalid command %s, status = %d\n",*(exec_cmd+st), status);
            exit(count_process);
        }
        for(i2 = 0; i2 <= count_process; ++i2){
            wait(&i);
        }
        free(exec_cmd);
        exit(1);
    }
    wait(&status);
    #ifdef DEBUG
        printf("command - %s, status = %d\n",exec_cmd[0], WEXITSTATUS(status));
    #endif
    printf("continue...");
    scanf("%*c");
    free(exec_cmd);
    return i;
}