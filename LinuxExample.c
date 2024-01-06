

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "=====ERROR=====\n\nNieodpowiednia ilosc argumentow wywolania programu -> %s <Natural Number> .\n\n", argv[0]);
        exit(1);
    }

    int inpValue = atoi(argv[1]);

    if (inpValue <= 0) {
        fprintf(stderr, "=====ERROR=====\n\nArgument wywolania programu -> %s Nie jest liczba naturalna .\n\n", argv[0]);
        exit(2);
    }

    if (inpValue < 1 || inpValue > 13) {
        fprintf(stderr, "=====ERROR=====\n\nArgument wywolania programu -> %s Musi byc z przedzialu < 1 ... 13 > .\n\n", argv[0]);
        exit(3);
    }

    if ( inpValue == 1 || inpValue == 2 ){
	return 1;
	}

    pid_t child_one, child_two;
    int stat_one, stat_two;

    child_one = fork();

    if (child_one == 0) 
    {
        int input = inpValue - 1;
        char arg[10];
        sprintf(arg, "%d", input);
        execl(argv[0], argv[0], arg, NULL);
        perror("=====ERROR=====\n\nBlad execl dla child_one");
        exit(1);
    }
	 else if (child_one > 0) 
   	 {
         child_two = fork();
         if (child_two == 0) {
            int input = inpValue - 2;
            char arg[10];
            sprintf(arg, "%d", input);
            execl(argv[0], argv[0], arg, NULL);
            perror("=====ERROR=====\n\nBlad execl dla child_two");
            exit(1);
          }else if (child_two < 0) {
            fprintf(stderr, "=====ERROR=====\n\nChild_two: Fork dont create process\n\n");
            exit(1);
          }
    }else if (child_one < 0) {
        fprintf(stderr, "=====ERROR=====\n\nChild_one: Fork dont create process\n\n");
        exit(1);
    }
    wait(&stat_one);
    wait(&stat_two);

    int r1 = WEXITSTATUS(stat_two);
    int r2 = WEXITSTATUS(stat_one);
    fprintf(stdout, "\n My Pid = %d \t First Child Pid = %d \t arg = %d \t Returned Code : %d", getpid(), child_one, inpValue - 1, WEXITSTATUS(stat_two));
    fprintf(stdout, "\n My Pid = %d \t Second Child Pid = %d \t arg = %d \t Returned Code : %d", getpid(), child_two, inpValue - 2, WEXITSTATUS(stat_one));
    fprintf(stdout, "\n My Pid = %d \t\t\t\t\t\t\t Returned Code : %d\n\n", getpid(), r1+r2);

    return r1+r2;
}



