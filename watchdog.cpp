/** @brief Watchdog 
  * @author Sevde Sarıkaya
  * @date January 2020
  * Compile: g++ watchdog.cpp -o watchdog
  * ./watchdog processNum process_output_path watchdog_output_path
  */
using namespace std;
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <csignal>
#include <bits/stdc++.h>

int num_of_process;
map<pid_t,int> pidMap;
map<int, pid_t> idMap;
int namedPipe;
char * process_output_path;
char * watchdog_output_path ;


/**
  * @brief Write the input to the named pipe.
  * @param s the string written to the pipe
  * Format: "P# <pid>"
  * Example: "P1 3564"
 */
void writePipe(string s) {
    char temp[30];
    for(int j = 0; j < s.length(); j++) {
        temp[j] = s[j];
    }
    write(namedPipe, temp, 30);

}

/**
 * @brief Write to the watchdog output file in the append mode.
 * @param s the string written to the output file.
 */
void writeFile(string s) {
    ofstream myfile;
    myfile.open(watchdog_output_path, ios::app);
    myfile << s;
    myfile.close();
}



/**
 * @brief Kill all children by sending SIGTERM signal.
 * Then remove the PIDs of these processes from the maps.
 */
void killProcesses() {
    for (auto pid : idMap) {
        kill(pid.second , SIGTERM);
	    pidMap.erase(pid.second);
        idMap.erase(pid.first);
        sleep(1);
    }
 
}


/**
 * @brief Create process
 * @param tempID the process number such as P0 or P1
 */
void createProcess(int tempID) {

    /** Create a child by fork then wait for awhile to get the processes in the order.
     */
    pid_t childpid = fork();
    sleep(1);

    /**
    *  If current process is a child, execute the process by sending the arguments (process output path, id of the process)
    */
    if (childpid == 0) {
	    string temp = to_string(tempID);
	    char * tempI = new char[temp.length()];
	    strcpy(tempI, temp.c_str());
        execl("./process", process_output_path, tempI, NULL);
    }

    /** 
    * If current process is the parent, write the information about child process to the file
    */
    else {
        writeFile("P" + to_string(tempID) + " is started and it has a pid of " + to_string(childpid) + "\n");
        pidMap[childpid] = tempID;
        idMap[tempID] = childpid;
        string s = "P" + to_string(tempID) + " " + to_string(childpid);
        writePipe(s);

    }
}


/**
 * @brief Create all child processes by calling createProcess
 * @see createProcess
 */
void createAll() {

     for(int i = 1; i <= num_of_process; i++) {
        createProcess(i);
     }
}

/**
 * @brief Handle the SIGTERM signal by killing all processes and finish the watchdog.  
 * @param signum Signal number
 */
void sigtermExit(int signum) {
    sleep(1);
    killProcesses();
    writeFile("Watchdog is terminating gracefully\n");
    exit(signum);
}

/**
 * @brief 
 */
int main(int argc, char* argv[]){

    /** take number of process needed to be created 
     */
    num_of_process = stoi(argv[1]);
    process_output_path =  argv[2];
    watchdog_output_path = argv[3];

    ofstream myfile(watchdog_output_path);
    myfile.close();

    ofstream processFile(process_output_path);
    processFile.close();

    /** Open the named pipe to write
     */
    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    namedPipe = open(myfifo,O_WRONLY);


    /** Get the PID of watchdog
     */
    pid_t pidParent = getpid();
    string s = "P0 " + to_string(pidParent);
    writePipe(s);

    /**  Create all child process
     */
    createAll();

    /**  Construct signal object to get the SIGTERM signal
     */
    signal(SIGTERM, sigtermExit);

    while(true) {
        /**
         * Wait until a child process terminates
         * then with its process id, open the process again.
         * If the terminated process is P1, then kill all children and open them again.
         * Write these operations to the file.
         */
        int pid = wait(NULL);
        int tempID = pidMap[pid];
	    if(tempID != 0) {
            pidMap.erase(pid);
            idMap.erase(tempID);
            if(tempID == 1) {
                writeFile("P1 is killed, all processes must be killed\nRestarting all processes\n");
                killProcesses();
                createAll();
            }
            else {
                writeFile("P" + to_string(tempID) + " is killed\nRestarting P" + to_string(tempID) + "\n");
                createProcess(tempID);
            }
	    }
    }
}

