

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 100

/********************************************************
 * struct of jobs that include the command ans id.
 *******************************************************/
typedef struct jobs{
    char *command;
    int id;
    struct jobs *next;

} jobs;



/******************************************************
 * The function split the input to word (argv)
 * @param line
 * @param argv
 * @return
 ******************************************************/
void splitInput(char *line, char **argv);


/*******************************************************
 * The function add the command(process=jobs)
 * that runs in the background to the list of jobs.
 * @param command
 * @param command1
 * @param id
 * @param head
 ********************************************************/
void addToList(char *command, char *command1, int id, struct jobs **head);


/********************************************************
 * The function print the commands (process=jobs)
 * that until runs in the background.
 * @param head
 * @param status
 ********************************************************/
void printList(jobs **head, int status);


/*********************************************************
 * The function free the list of jobs.
 * @param jobsCommand
 *********************************************************/
void freeList(struct jobs *jobsCommand);


int main(void) {

    jobs* list_jobs = NULL;
    while (1) {
        char  input_string[SIZE];
        char  *argv[SIZE];
        char  *gdir,*dir,*to;
        char* last[SIZE];
        char buf[SIZE];
        pid_t pid;
        int status;
        int background = 0;


        printf("prompt>");
        fgets(input_string, 1000, stdin);
        if(input_string[0] =='\n'){
            continue;
        }
        // If press enter
        if (input_string[strlen(input_string) - 1] == '\n'){
            input_string[strlen(input_string) - 1] = '\0';
        }

        //split the input
        splitInput(input_string, argv);


        //if the input is "exit"
        if (!strcmp(argv[0], "exit")){
            freeList(list_jobs);
            printf("%d\n",getpid());
            exit(0);
        }

        //if the input is "cd"
        if (!strcmp(argv[0], "cd")){
            printf("%d\n",getpid());
            int j =0;
            while(last[j]!=NULL){
                j++;
            }

            gdir = getcwd(buf, sizeof(buf));
            last[j] = gdir;
            dir = strcat(gdir, "/");
            if(!strcmp(argv[1], "~")){      //cd ~
                to = "/home";
            }
            else if(!strcmp(argv[1], "-")){ //cd -
                if(j>0) {
                    to = last[j-1];
                }
                else{
                    to = gdir;
                }
            }
            else{
                to = strcat(dir, argv[1]);
            }
            chdir(to);
            continue;
        }

        //if the input is "jobs": print the jobs that in background
        if(!strcmp(argv[0],"jobs")){
            printList(&list_jobs, status);
            continue;
        }

        int i=0;
        while (argv[i] != NULL) {
            i++;
        }
        //if the input include "&" the process runs in the background.
        if(!strcmp(argv[i-1], "&")){
            argv[i-1] = NULL;
            background = 1;
        }


        pid = fork();
        if (pid == 0) {
            printf("%d\n",getpid());
            if (execvp(argv[0], argv) < 0) {
                fprintf(stderr, "Error in system call");
                exit(1);
            }

            continue;
        }
        else {
            if (!background) {
                waitpid(pid, &status, 0);

            } else {
                addToList(argv[0], argv[1], pid, &list_jobs); //if background-add to list of jobs.
            }
        }

        sleep(1);

    }

}

void splitInput(char *line, char **argv){

    while (*line != '\0') {
        while (*line == ' ' || *line == '\t' || *line == '\n')

            *line++ = '\0';
        *argv++ = line;

        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') {
            line++;
        }
    }
    *argv = '\0';
}

void addToList(char *command, char *command2, int id, struct jobs **head) {

        struct jobs *new_node;
        new_node = malloc(sizeof(struct jobs));
        //check if malloc successful
        if(new_node == NULL){
            fprintf(stderr, "Memory allocation failed");
            return;
        }


        if (command2 != NULL) {
            size_t length = strlen(command);
            command[length] = ' '; // add a empty between command and command2
        }
        new_node->command = malloc(sizeof(char *) * strlen(command) + 1);


        //check if malloc successful
        if(new_node->command == NULL){
                fprintf(stderr, "Memory allocation failed");
                return;
        }

        strcpy(new_node->command, command);
        new_node->id = id;
        new_node->next = *head;
        *head = new_node;

}


void printList(jobs **head, int status) {
    jobs *current = (*head);
    while (current != NULL) {
        if(waitpid(current->id, &status, WNOHANG) != 0){ //if the process does not runs in the background
            current = current->next;
        }
        else { //print the id and the command of process that runs in the background
            printf("%d %s\n", current->id, current->command);
            current = current->next;
        }
    }
}


void freeList(struct jobs *jobsCommand){

    if(jobsCommand!=NULL){
        free(jobsCommand->command);
        free(jobsCommand);
    }
}

