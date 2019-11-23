#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <string.h>
#include <errno.h>

#define BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

void loopShell(void);
char *readLine(void);
char **parseInput(char *line);
int execute(char **args);
int launch(char **args);
int lsh_num_builtins();
int cdCMD(char **args);
int exitCMD(char **args);
int tree();

char *builtin_str[] = {
  "cd",
  "exit",
  "tree",
  "list"
};

int (*builtin_func[]) (char **) = {
  &cdCMD,
  &exitCMD,
  &tree,
  &list
};

int main(int argc, char **argv)
{

  loopShell();


  return EXIT_SUCCESS;
}
int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}  
int cdCMD(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

void loopShell(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("~op user~ ");
    line = readLine(); //inputs line first 
    args = parseInput(line);//parse
    status = execute(args);

    free(line);
    free(args);
  } while (status);
}

char *readLine(void)
{
//	printf("line read\n");
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  getline(&line, &bufsize, stdin);
  return line;
}

char **parseInput(char *line)
{
//	printf("parse\n");
  int bufsize = BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
int launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status,0);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int clear_command(){
    char *myargs[2];
    myargs[0] = strdup("clear");
    myargs[1] = NULL;
    return launch(myargs);
}

int ls_command(){
    char *myargs[3];
    myargs[0] = strdup("ls");
    myargs[1] = strdup("-l");  
    myargs[2] = NULL;
    return launch(myargs);
}

void write_to_file(){
    char path[1035];
    FILE * output_file = fopen("t1.txt","w");
    if(output_file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }

    FILE * command = popen("/bin/ls -l","r");
    if(command == NULL){
        printf("Failed to run command\n");
        exit(1);
    }
    while(fgets(path,sizeof(path),command) != NULL){
        fprintf(output_file,"%s",path);
    }

    pclose(command);
    fclose(output_file);
}

int rename_command(){
    char *myargs[4];
    myargs[0] = strdup("mv");
    myargs[1] = strdup("t1.txt");
    myargs[2] = strdup("tree.txt");
    myargs[3] = NULL;
    return launch(myargs);
}

int execute(char **args)
{
	//printf("execute\n");
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  // if(strcmp(args[0],"list") == 0){
  //   clear_command();
  //   ls_command();
  //   write_to_file();
  //   rename_command();
  //   return 1;
  // }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return launch(args);
}
int exitCMD(char **args)
{
	printf("exiting\n");
  return 0;
}
int list(**args){
    clear_command();
    ls_command();
    write_to_file();
    rename_command();
    return 1;
}

int tree(**args){
    FILE *f1, *f2, *f3;
    //create directory with write permission
    if (mkdir("Dir0", 0777) == -1) {
            printf("Error : %s\n ", strerror(errno)); 
    }else{
            printf("Directory 0 created\n");
          }
    chdir("Dir0");
    f1 = fopen ("t1.txt", "w");
    f2 = fopen ("t2.txt", "w");
    f3 = fopen ("t3.txt", "w");
    printf("Files created\n");
    if (mkdir("Dir1", 0777) == -1){
            printf("Error : %s\n ", strerror(errno)); 
    }else{
            printf("Directory 1 created\n");
          }
  return 1;


}
