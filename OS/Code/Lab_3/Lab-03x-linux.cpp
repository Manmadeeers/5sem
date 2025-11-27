#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>

using namespace std;

int main(int argc, char*argv[]){
	int loops = 0;
	if(argc>1){
		loops = atoi(argv[1]);
	}
	else{
		char*envVar = getenv("ITER_NUM");
		if(envVar){
			loops = atoi(envVar);
		}
	}

	if(loops<=0){
		cerr<<"Error: loops amount is not specified\n";
		exit(1);
	}

	pid_t pid = getpid();
	cout<<"loops amount: "<<loops<<endl;

	for(int i=1;i<=loops;++i){
		cout<<"i: "<<i<<" PID: "<<pid<<endl;
		usleep(500000);
	}

	cout<<"loops finished"<<endl;

	return 0;
}
