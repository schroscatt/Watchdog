/** @brief Process
 *  Receive signals and handle
 *  @author Sevde Sarıkaya
 *  @date January 2020
 *  Compile: g++ process.cpp -o process
 *  ./process  process_output_path tempID
 */

using namespace std;
#include <string>
#include <iostream>
#include <map>
#include <unistd.h>
#include <csignal>
#include <bits/stdc++.h>


int tempID; //* process ID (P#)
string process_output_path; //* where to write output

/**
 * @brief Write to the process output file in the append mode.
 * @param s the string written to the output file.
 */
void writeFile(string s) {
    ofstream myfile;
    myfile.open(process_output_path, ios::app);
    myfile << s;
    myfile.close();
}

/**
 *  @brief Handle the signals 
 *  If signal is SIGTERM, then kill the process.
 *  Otherwise, write the signal to the process output file.
 *  @param signum Signal number
 */
void signalHandler(int signum) {

    if (signum == 15) {
        writeFile("P" +  to_string(tempID)  + " received signal " + to_string(signum) + ", terminating gracefully\n");
        exit(signum);
    }

    else {
        writeFile("P" +  to_string(tempID)  + " received signal " + to_string(signum) + "\n");
    }

}

    
/**
  @brief 
  */
int main(int argc, char* argv[]){

    //Take process output path and process id as arguments.
    process_output_path = argv[0];
    tempID = stoi(argv[1]);

    /**  Construct signals to receive and handle. 
     *   Signals to handle: SIGINT, SIGHUP, SIGILL, SIGTRAP, SIGBUS, SIGFPE, SIGSEGV, SIGXCPU, SIGTERM
     */
    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTRAP, signalHandler);
    signal(SIGBUS, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGXCPU, signalHandler);
    signal(SIGTERM, signalHandler);

    /** Sleep until a new signal is received. 
     */
    writeFile("P" + to_string(tempID)  + " is waiting for a signal\n");

    while (true) { }
    return 0;
}