#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

int main(){
    pid_t pid1,pid2;
    const char* iterations_arg = "50";
    const char* iterations_env = "75";
    pid1 = fork();
    if(pid1<0){
        perror("fork failed for first process");
        return 1;
    }
    if(pid1==0){
        execl("./Lab-03x","Lab-03x",iterations_arg,nullptr);
        perror("execl failed for first process");
        return 1;
    }
    pid2 = fork();
    if(pid2<0){
        perror("fork failed for second process");
        return 1;
    }
    if(pid2==0){
        setenv("ITER_NUM",iterations_env,1);
        execl("./Lab-03x","Lab-03x",nullptr);
        perror("execl failed for second process");
        return 1;
    }
    int status;
    waitpid(pid1,&status,0);
    waitpid(pid2,&status,0);

    std::cout<<"Both processes have finished"<<std::endl;
    return 0;
}
