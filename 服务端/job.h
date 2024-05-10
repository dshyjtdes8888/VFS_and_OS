#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <sstream>
#include <Windows.h>

using namespace std;

struct Job {
    int jobID;
    int PID;
    string program;
    string status;
    int progress;
    int now_pro;

    Job(int id, int pid, const string& prog) : jobID(id), PID(pid), program(prog), status("created"), progress(0), now_pro(0) {}
};

class JobManager {
public:
    vector<Job> jobs; //多作业存储
    unordered_map<int, int> jobIDtoIndex; //作业ID匹配

public:
    void createJob(int jobID, const string& program) {
        int PID = 520 + jobID;
        Job job(jobID, PID, program);
        jobs.push_back(job);
        jobIDtoIndex[jobID] = jobs.size() - 1;
        cout << "create job and run... ok" << endl;
        cout << "JobID:" << jobID << " PID:" << PID << endl;
    }
    void run(int jobID)
    {
        int t = jobIDtoIndex[jobID];
        if (jobs[t].status == "created" || jobs[t].status == "running")
        {
            // 模拟作业执行过程的线程
            thread jobThread([this, jobID]() {
                // 模拟作业执行过程
                int index = jobIDtoIndex[jobID];
                jobs[index].status = "running";
                for (int i = 0; i <= 100 - jobs[index].now_pro; i += 2) {
                    jobs[index].progress = i + jobs[index].now_pro;
                    this_thread::sleep_for(chrono::seconds(1));
                }
                if (jobs[index].progress == 100 && jobs[index].status == "running")
                {
                    jobs[index].status = "success";
                }
                });
            jobThread.detach(); // 分离线程，使其在后台运行
        }
        else
        {
            terminate();
        }
    }

    void displayJobStatus() {
        cout << "job status in jdm server:" << endl;
        cout << "JobID PID status program" << endl;
        for (const auto& job : jobs) {
            if (job.status == "running"|| job.status == "success")
            {
                cout << job.jobID << " " << job.PID << " " << job.status << " " << job.progress << "% " << job.program << endl;
            }
            else
            {
                cout << job.jobID << " " << job.PID << " " << job.status << " " << job.now_pro << "% " << job.program << endl;
            }
        }
    }

    void suspendJob(int jobID) {
        int index = jobIDtoIndex[jobID];
        jobs[index].status = "suspend";
        jobs[index].now_pro = jobs[index].progress;
        jobs[index].progress = 0;
        cout << "suspend job " << jobID << " ... ok" << endl;
    }

    void cancelJob(int jobID) {
        int index = jobIDtoIndex[jobID];
        jobs[index].status = "canceled";
        jobs[index].now_pro = jobs[index].progress;
        jobs[index].progress = 0;
        cout << "cancel job " << jobID << " ... ok" << endl;
    }

    void resumeJob(int jobID) {
        int index = jobIDtoIndex[jobID];
        jobs[index].status = "running";
        cout << "resume job " << jobID << " ... ok" << endl;
    }

    void logJob(int jobID) {
        int index = jobIDtoIndex[jobID];
        cout << "job:create job " << jobID << endl;
        cout << "fork:sub process ID " << jobs[index].PID << endl;
        cout << "exec:" << jobs[index].program << endl;

        for (int i = 0; i < 5; i++) {
            cout << "message:sleep 1 second" << endl;

            //jobs[index].progress = jobs[index].progress-6+i;
            if (jobs[index].status == "running")
            {
                cout << "progress ratio:" << jobs[index].progress << "%" << endl;
                Sleep(1000);
            }
            else
            {
                cout << "progress ratio:" << jobs[index].now_pro << "%" << endl;
                Sleep(1000);
            }

        }
        cout << "job:" << jobs[index].status << endl;
    }

    void clearJobs() {
        for (auto it = jobs.begin(); it != jobs.end(); ) {
            if (it->status == "success" || it->status == "canceled") {
                it = jobs.erase(it);
            }
            else {
                ++it;
            }
        }
        cout << "clear finished and canceled jobs ... ok" << endl;
    }


};

/*int main() {
    JobManager jobManager;

    while (true) {
        JobManager jobManager;

        while (true) {
            string input;
            cout << "Enter command: ";
            getline(cin, input);

            stringstream ss(input);
            string command;
            ss >> command;

            if (command == "create") {
                int jobID;
                ss >> jobID;
                jobManager.createJob(jobID, "");
            }
            else if (command == "run")
            {
                int jobID;
                ss >> jobID;
                jobManager.run(jobID);
            }
            else if (command == "status") {
                jobManager.displayJobStatus();
            }
            else if (command == "suspend") {
                int jobID;
                ss >> jobID;
                jobManager.suspendJob(jobID);
            }
            else if (command == "cancel") {
                int jobID;
                ss >> jobID;
                jobManager.cancelJob(jobID);
            }
            else if (command == "resume") {
                int jobID;
                ss >> jobID;
                jobManager.resumeJob(jobID);
                jobManager.run(jobID);
            }
            else if (command == "log") {
                int jobID;
                ss >> jobID;
                jobManager.logJob(jobID);
            }
            else if (command == "clear") {
                jobManager.clearJobs();
            }
            else if (command == "quit") {
                break;
            }
            else {
                cout << "Invalid command!" << endl;
            }
        }

        return 0;
    }
}*/