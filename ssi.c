
/* Linux-style operating system implementing 
/* various functions as well as exploring concepts
/* involving foreground and background processes.
/* 
/* Created By: Leif St Bright
*/

#include <signal.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define HOSTNAME_MAX 256

/* background process struct */
typedef struct bg_process{
    pid_t pid;
    char process[512];
    struct bg_process *next;

}bg_process;
bg_process *head = NULL;

pid_t for_pid = 0;

/* handles when ctrl-c is pressed */
void ctrl_c_handler(int sig){
    (void)sig;
     
    if (for_pid > 0){
        kill(for_pid, SIGINT);
    }
    else{
        printf("\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

/* add new process to linked list of processes*/
void add_process(pid_t new_pid, const char *process_command){
    bg_process *new_p = malloc(sizeof(bg_process));
    new_p->pid = new_pid;
    new_p->next = head;
    head = new_p;
    strncpy(new_p->process, process_command, sizeof(new_p->process));
    
}

/* remove given process from list and return its pid */
bg_process* get_process(pid_t new_pid){
    bg_process *current = head;
    bg_process *prev = NULL;
    while(current != NULL){
        if (current->pid == new_pid){
            if (prev == NULL){
                head = current->next;
            }
            else{
                prev->next = current->next;
            }
            
            return current;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
} 


/* Tokenize user input dynamically */ 
char **tokenize(char *user_input){
    size_t size = 8;
    char **word_array = malloc(size * sizeof(char *));
    size_t index = 0;
    char *token = strtok(user_input, " ");
    while(token != NULL){
        if (index >= size - 1){
            size *= 2;
            word_array = realloc(word_array, size * sizeof(char *));
        }
        word_array[index++] = token;
        token = strtok(NULL, " ");
    }
    word_array[index] = NULL;
    return word_array;
}
/* change directory */
void change_dir(char *dir){
    char *path = NULL;
    
    if ((dir == NULL) || (strcmp(dir, "~") == 0) || (strcmp(dir, "$HOME") == 0)){
        dir = getenv("HOME");
    }  
    else if (dir[0] == '~' && dir[1] == '/'){
        char *home = getenv("HOME");
        size_t len = strlen(home) + strlen(dir);
        path = malloc(len);
        if(!path){
            perror("malloc error for change_dir");
            return;
        }
        strcpy(path, home);
        strcat(path, dir + 1);
    
    }
    else{
        path = dir;
    }
    if (chdir(path) != 0){
        perror("invalid path");
    }
    if (dir[0] == '~' && dir[1] == '/'){
        free(path);    
    }
}


int main(void){
    
    signal(SIGINT, ctrl_c_handler);
    
    char *username = getlogin();
    char hostname[HOSTNAME_MAX + 1];
    size_t len;
    
    while(1){

        /* Check that shell will display properly */
        char *cwd = getcwd(NULL, 0);
        if (username == NULL){
            perror("error in getlogin");
            exit(1);
        }
        if (gethostname(hostname, sizeof(hostname)) != 0){
            perror("error in gethostname");
            exit(1);
        }
        if (!cwd){
            perror("error in getcwd");
            exit(1);
        } 
        
        pid_t done;
        int wstatus;
        
        /* Check if a background process has terminated */
        while((done = waitpid(-1, &wstatus, WNOHANG)) > 0){
            bg_process *term_process = get_process(done);
            if (term_process != NULL){
                printf("%d: %s has terminated.\n", term_process->pid, term_process->process);
                free(term_process);
                rl_on_new_line();
                rl_redisplay(); 
            } 
        }
        
        
        
        len = strlen(cwd) + strlen(hostname) + strlen(username) + 6;
        char *prompt = malloc(len);
        if (!prompt){
            perror("malloc error with prompt");
            free(cwd);
            break;
        }

        /* Display and read input*/
        snprintf(prompt, len, "%s@%s:%s > ", username, hostname, cwd);
        fflush(stdout);
        char *input = readline(prompt);
        free(prompt);
                
        if (!input){
            printf("\n");
            free(cwd);
            break;
        }
        
        // No input (enter)
        if(strlen(input) == 0){
            free(input);
            free(cwd);
            continue;
        }
        
            
        char **words = tokenize(input);
        
        char *arg1 = words[0];
        char *arg2 = words[1];
        
        /* Decide what to execute from user input*/
        if (strcmp(arg1, "cd") == 0){
            change_dir(arg2);            
        }
        else if (strcmp(arg1, "pwd") == 0){
            printf("%s\n", cwd);
            free(cwd);
        }
        else if (strcmp(arg1, "bg") == 0){
            if (words[1] == NULL){
                printf("not enough arguments");
                continue;
            }
            
            pid_t pid = fork();
            if (pid == 0){
                setpgid(0,0);
                execvp(words[1], &words[1]);
                perror("error in execvp");
                exit(1);
            }
            else if (pid > 0 ){                
                add_process(pid, input + 3);
                
                
                rl_on_new_line();
                rl_redisplay();
            }
            else{
                perror("error in fork");
            }
        }

        else if (strcmp(arg1, "bglist") == 0){
            bg_process *current = head;
            int count = 0;
            while(current != NULL){
                count++;
                printf("%d: %s %d \n", current->pid, current->process, count);
                current = current->next;
            }
            printf("Total Background jobs: %d\n", count);
            
        }
        /* else will execute foreground commands such as ls */
        else {
            pid_t pid = fork();
            if(pid == 0){
 
                execvp(arg1, words);
                perror(arg1);
                exit(1);
            }
            else if (pid > 0){
                for_pid = pid;
                int result;
                waitpid(pid, &result, 0);
                for_pid = 0;
            }
        }
        
        free(input);
        free(words);
    }
    
    return 0;
}
