//Development Environment: MacOS Mojave 10.14 with clang++ Also works on ITU SSH Servers
//To compile: g++ -std=c++11  dancers.cpp -pthread or on Mac clang++ also works.
//To link: g++ -o dancers.cpp -pthread
//To run: ./a.out leaders followers
//Example: ./a.out 4 4


#include<iostream>
#include<thread>
#include<unistd.h>
#include<vector>
#include<mutex>

using namespace std;

#define dance() (usleep(500000))

class Semaphore{

    public:
    std::mutex S1;

    Semaphore(int value) {
        if(value == 0) {
            S1.lock();
        }
    }

    void wait() {
        S1.lock();
    }

    void signal() {
        S1.unlock();
    }
};

int leaders = 0;
int followers = 0;

Semaphore mutexx(1);
Semaphore leaderQueue(0);
Semaphore followerQueue(0);
Semaphore rendezvous(0);


void leadersfunc(int i);
void followersfunc(int i);

int main(int argc, char** argv) {
    int numleaders, numfollowers;

    numleaders = stoi(argv[1]);
    numfollowers = stoi(argv[2]);

    std::thread t[numleaders+numfollowers];

    int leaderindex = numleaders;
    int followerindex = numfollowers;

    int leaderindex2 = numleaders;                      //Two same indexes for two same while loop one is declaring
                                                        //Other one just make them join
    int followerindex2 = numfollowers;

    while(leaderindex-1 >= 0 || followerindex-1>= 0) {

        if(leaderindex-1 >= 0) {
            t[leaderindex-1] = thread(leadersfunc, leaderindex-1);
            leaderindex--;
        }
        if(followerindex-1 >= 0) {
            t[numleaders+followerindex-1] = thread(followersfunc, followerindex-1);
            followerindex--;
        }

    }

    while(leaderindex2-1 >= 0 || followerindex2-1 >= 0) {
        srand (time(NULL));

        if(leaderindex2-1 >= 0) {
            int timer = rand() % 2000000;
            usleep(timer);
            t[leaderindex2-1].join();
            leaderindex2--;
        }
        if(followerindex2-1 >= 0) {
            int timer = rand() % 2000000;
            usleep(timer);
            t[numleaders+followerindex2-1].join();
            followerindex2--;
        }

    }
}

void leadersfunc(int i) {

    mutexx.wait();

    if(followers > 0) {
        cout << "Leader " << i+1 << ": " << followers <<  " followers are waiting, so I signal the next follower in the queue." << "\n";
        followers--;
        followerQueue.signal();
    }
    else {
        cout << "Leader " << i+1 << ": No available follower, so I wait. There are other " << leaders << "leaders waiting. " << "\n";
        leaders++;
        mutexx.signal();
        leaderQueue.wait();
    }

    cout << "Leader " << i+1 << ": We are dancing together now." << "\n";

    dance();
    rendezvous.wait();
    mutexx.signal();

    cout << "Leader " << i+1 << ": I leave now. " << "\n";
}

void followersfunc(int i) {

    mutexx.wait();

    if(leaders > 0) {
        cout << "Follower " << i+1 << ": " << leaders << " are waiting, so I signal the next leader in the queue." << "\n";
        leaders--;
        leaderQueue.signal();
    }
    else {
        cout << "Follower " << i+1 << ": No available leader, so I wait. There are other " << followers << " followers waiting." << "\n";
        followers++;
        mutexx.signal();
        followerQueue.wait();
    }

    cout << "Follower " << i+1 << ": We are dancing together now." << "\n";

    dance();
    rendezvous.signal();

    cout << "Follower " << i+1 << ": I leave now. " << "\n";
}

