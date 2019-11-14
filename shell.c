#include <stdio.h>
#include <stdlib.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

void loopShell(void);
char *readLine(void);
char **parseInput(char *line);
int execute(char **args);
int launch(char **args);
int lsh_num_builtins();
int cdCMD(char **args);
int exitCMD(char **args);

char *builtin_str[] = {
  "cd",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &cdCMD,
  &exitCMD

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
    printf("youre user> ");
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
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
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
      bufsize += LSH_TOK_BUFSIZE;
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
int execute(char **args)
{
	//printf("execute\n");
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

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

