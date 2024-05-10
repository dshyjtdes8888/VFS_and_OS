#include "vfs.h"
#include "job.h"
//#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

const int BUF_SIZE = 2048;			//缓冲区大小
char sendbuf[BUF_SIZE];		//发送缓冲区
char recvbuf[BUF_SIZE];		//接受缓冲区

JobManager jobManager;
vfs vvv;


void initialization()			//初始化操作
{
	//WSADATA是一种数据结构，用来存储被WSAStartup函数调用后返回的Windows sockets数据，包含Winsock.dll执行的数据。需要头文件
	WSADATA wsadata;

	//MAKEWORD声明调用不同的Winsock版本。例如MAKEWORD(2,2)就是调用2.2版
	WORD wsaversion = MAKEWORD(2, 2);


	//WSAStartup函数必须是应用程序或DLL调用的第一个Windows套接字函数
	//可以进行初始化操作，检测winsock版本与调用dll是否一致，成功返回0
	if (WSAStartup(wsaversion, &wsadata) != 0)
	{
		cout << "载入socket库失败" << endl;
	}
	else {
		cout << "初始化成功" << endl;
	}

}

int GetArraySize(const char* arr, int maxSize)
{
	int count = 0;
	for (int i = 0; i < maxSize; i++)
	{
		if (arr[i] != '\0')
			count++;
		else
			break; // 遇到空字符，跳出循环
	}
	return count;
}

void PrintSystemStatus() {
	// Query and print task information
	DWORD processIds[1024];
	DWORD processCount;
	if (EnumProcesses(processIds, sizeof(processIds), &processCount)) {
		DWORD taskCount = processCount;
		DWORD runningCount = 0;
		DWORD sleepingCount = 0;
		DWORD stoppedCount = 0;
		DWORD zombieCount = 0;

		for (DWORD i = 0; i < taskCount; i++) {
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
			if (hProcess) {
				DWORD exitCode;
				if (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
					runningCount++;
				}
				else {
					if (exitCode == 0) {
						stoppedCount++;
					}
					else {
						zombieCount++;
					}
				}
				CloseHandle(hProcess);
			}
		}

		cout << "Tasks: " << taskCount << " total, " << runningCount << " running, "
			<< sleepingCount << " sleeping, " << stoppedCount << " stopped, " << zombieCount << " zombie" << endl;
	}

	// Query and print CPU information
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	DWORD processorCount = systemInfo.dwNumberOfProcessors;

	FILETIME idleTime, kernelTime, userTime;
	if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
		ULONGLONG idleTimePrev = (reinterpret_cast<ULONGLONG*>(&idleTime))[0];
		ULONGLONG kernelTimePrev = (reinterpret_cast<ULONGLONG*>(&kernelTime))[0];
		ULONGLONG userTimePrev = (reinterpret_cast<ULONGLONG*>(&userTime))[0];
		Sleep(1000); // Wait for 1 second
		if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
			ULONGLONG idleTimeNow = (reinterpret_cast<ULONGLONG*>(&idleTime))[0];
			ULONGLONG kernelTimeNow = (reinterpret_cast<ULONGLONG*>(&kernelTime))[0];
			ULONGLONG userTimeNow = (reinterpret_cast<ULONGLONG*>(&userTime))[0];

			ULONGLONG idleTimeDiff = idleTimeNow - idleTimePrev;
			ULONGLONG kernelTimeDiff = kernelTimeNow - kernelTimePrev;
			ULONGLONG userTimeDiff = userTimeNow - userTimePrev;

			float cpuUsage = 100.0f * (kernelTimeDiff + userTimeDiff) / (kernelTimeDiff + userTimeDiff + idleTimeDiff * processorCount);

			cout << "%Cpu(s): " << cpuUsage << " us, " << kernelTimeDiff / processorCount << " sy, 0.0 ni, "
				<< idleTimeDiff / processorCount << " id, 0.0 wa, 0.0 hi, 0.0 si, 0.0 st" << endl;
		}
	}

	// Query and print memory information
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatusEx(&memStatus);

	cout << "KiB Mem: " << memStatus.ullTotalPhys / 1024 << " total, "
		<< memStatus.ullAvailPhys / 1024 << " free, " << (memStatus.ullTotalPhys - memStatus.ullAvailPhys) / 1024
		<< " used, " << memStatus.ullAvailPageFile / 1024 << " buff/cache" << endl;

	cout << "KiB Swap: " << memStatus.ullTotalPageFile / 1024 << " total, "
		<< memStatus.ullAvailPageFile / 1024 << " free, " << (memStatus.ullTotalPageFile - memStatus.ullAvailPageFile) / 1024
		<< " used, " << memStatus.ullAvailPhys / 1024 << " avail Mem" << endl;

	ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
	char rootPath[MAX_PATH] = "E:\\";

	if (GetDiskFreeSpaceExA(rootPath, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
	{
		double usedPercent = (1 - (double)totalFreeBytes.QuadPart / totalBytes.QuadPart) * 100;

		cout << "Filesystem 1K-blocks: " << totalBytes.QuadPart / 1024 << "\t"
			<< "Used: " << (totalBytes.QuadPart - totalFreeBytes.QuadPart) / 1024 << "\t"
			<< "Available: " << totalFreeBytes.QuadPart / 1024 << "\t"
			<< "Use%: " << usedPercent << "%\t"
			<< "Mounted on: " << rootPath << endl;
	}
	else
	{
		cout << "Failed to get file system information." << endl;
	}

	cout << "VFS " <<D_N << "  " << disk_size << "  "
		<< 1024 - sizeof(disk->content) << "  " << (sizeof(disk->content) / D_N) * 100 << "%" << " /root" << endl;
	int run_num = 0;
	int suc_num = 0;
	int can_num = 0;
	int sus_num = 0;
	int i = 0;
	for (auto it = jobManager.jobs.begin(); it != jobManager.jobs.end(); )
	{
		if (jobManager.jobs[i].status == "running")
		{
			run_num++;
		}
		if (jobManager.jobs[i].status == "success")
		{
			suc_num++;
		}
		if (jobManager.jobs[i].status == "canceled")
		{
			can_num++;
		}
		if (jobManager.jobs[i].status == "suspend")
		{
			sus_num++;
		}
		++it;
		++i;
	}
	cout << "Job:" << sizeof(jobManager.jobs) << " total, " <<
		run_num << " running, " << suc_num << " success, " << can_num << " canceled, " << sus_num << " suspend" << endl;
}

int main()
{
	SOCKET sockSer;	//创建服务器套接字
	SOCKET sockCli;

	//SOCKADDR_IN是系统封装的一个结构体，具体包含了成员变量：sin_family、sin_addr、sin_zero
	SOCKADDR_IN addrSer;
	SOCKADDR_IN addrCli;


	initialization();


	//socket(int domain, int type, int protocol)
	//domain表示要使用的协议AF_INET指ipv4,type指套接字类型 stream指tcp
	//协议“protocol”一般设置为“0”，就是说在已经确定套接字使用的协议簇和类型时，这个参数的值就为0。
	sockSer = socket(AF_INET, SOCK_STREAM, 0);


	//初始化地址包
	//sin_addr主要用来保存IP地址信息
	//htonl 函数可用于将主机字节顺序中的 IPv4 地址转换为网络字节顺序中的 IPv4 地址。
	addrSer.sin_addr.s_addr = htonl(INADDR_ANY);
	//sin_family主要用来定义是哪种地址族
	addrSer.sin_family = AF_INET;
	//sin_port主要用来保存端口号
	addrSer.sin_port = htons(8000);

	// 在建立套接字文件描述符成功后，需要对套接字进行地址和端口的绑定，才能进行数据的接收和发送操作。
	if (bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		//cout << "套接字绑定失败";
		//调用 WSACleanup 时，将取消此过程中任何线程发出的挂起阻止或异步 Windows 套接字调用，
		//而无需发布任何通知消息，也不发出任何事件对象的信号。
		WSACleanup();
	}
	else {
		//cout << "绑定套接字成功" << endl;
	}

	//设置监听
	cout << "开始连接" << endl;
	//等待连接最大 0x7fffffff 即32位最大值，成功返回0，失败失败返回SOCKET_ERROR
	if (listen(sockSer, SOMAXCONN) != 0) {
		cout << "监听失败！";
		WSACleanup();
	}
	//cout << "监听成功" << endl;


	//接受连接:
	//accept将客户端的信息绑定到一个socket上，
	//也就是给客户端创建一个socket，通过返回值返回给我们客户端的socket
	int addrlen = sizeof(SOCKADDR);
	sockCli = accept(sockSer, (SOCKADDR*)&addrCli, &addrlen);
	if (sockCli == SOCKET_ERROR) {
		cout << "接受连接失败！" << endl;
		WSACleanup();
		return 0;
	}


	cout << "连接建立，准备接受数据" << endl;

	int cc = 0;
	int count = 5;
	while (true)
	{
		if (recv(sockCli, recvbuf, sizeof(recvbuf), 0) == -1) {
			cout << "接受失败！" << endl;
			break;
		}
		else {
			//cout << "客户端消息：" << recvbuf << endl;
		}
		stringstream buffer;
		streambuf* oldCoutBuffer = cout.rdbuf(buffer.rdbuf());
		vvv.read_file();
		string zhiling = "";
		int n = GetArraySize(recvbuf, BUF_SIZE);


		for (int i = 0; i < n; i++)
		{
			zhiling = zhiling + recvbuf[i];
		}
		// 查找分隔符的位置
		size_t delimiterPos = zhiling.find("*");
		string zhilingtype;
		string file_name;
		if (delimiterPos != string::npos)
		{
			// 提取分隔符之前的子字符串
			zhilingtype = zhiling.substr(0, delimiterPos);
			// 提取分隔符之后的子字符串
			file_name = zhiling.substr(delimiterPos + 1);

			//cout << "分隔符之前的子字符串: " << zhilingtype << endl;
			//cout << "分隔符之后的子字符串: " << file_name << endl;
		}
		else
		{
			zhiling = zhiling;
		}
		if (zhiling == "./jdms" && cc == 0)
		{
			cout << "Job and data manager initialization......" << endl;
			cout << "Starting system monitor... ok" << endl;
			cout << "Create new virtual file system in file:/Test Prepare job manager... ok" << endl;
			cout << "Server is running..." << endl;
			cc++;
		}
		else if (zhiling == "./jdms" && cc == 1)
		{
			cout << "Job and data manager initialization......" << endl;
			cout << "Starting system monitor... ok" << endl;
			cout << "Using existing virtual file system in file:/Test Prepare job manager... ok" << endl;
			cout << "Server is running..." << endl;
		}
		else if (zhiling == "./jdmc-server-shutdown")
		{
			cout << "Shutdown jdm server... ok" << endl;
			vvv.out_file();
		}
		else if (zhiling == "./jdmc-server-status")
		{
			count = 5;
			cout << "current:" << endl;
			PrintSystemStatus();
			while (count != 15) {
				Sleep(5000); // Wait for 5 seconds
				cout << count << "  seconds ago:" << endl;
				PrintSystemStatus();
				count += 5;
			}
		}
		//vfs类指令
		else if (zhiling == "./jdmc-vfs-format")
		{
			cout << "format virtual file system in /Test.... ok" << disk_size
				<< "1K - blocks 4" << " Used " << sizeof(disk->content) << " Available " << endl;
			vvv.format_disk();
		}
		else if (zhiling == "./jdmc-vfs-ls")
		{
			cout << "list all directories and files in /root:" << endl;
			vvv.dir();
		}
		else if (zhiling == "./jdmc-vfs-save")
		{
			vvv.save();
		}
		else if (zhilingtype == "./jdmc-vfs-cd")
		{
			vvv.cd(file_name);
		}
		else if (zhilingtype == "./jdmc-vfs-mkdir")
		{
			vvv.make_dir(file_name);
			cout << "make directory /" << file_name << " ok " << endl;
			//vvv.save();
		}
		else if (zhilingtype == "./jdmc-vfs-upload")
		{
			vvv.upload_file(file_name);
			cout << "upload local file to virtual file system... ok" << file_name << "-->" << "/root/" << file_name << endl;
		}
		else if (zhilingtype == "./jdmc-vfs-download")
		{
			vvv.download_file(file_name);
			cout << "download file from virtual file system to local... ok" << "/root/" << file_name << "-->" << file_name << endl;
		}
		else if (zhilingtype == "./jdmc-vfs-openfile")
		{
			vvv.openfile(file_name);
		}
		else if (zhilingtype == "./jdmc-vfs-rmdir")
		{
			vvv.rmdir(file_name);
			cout << "delete file in virtual file system:" << file_name << endl;
		}

		else if (zhilingtype == "./jdmc-vfs-mkfile")
		{
			// 查找分隔符的位置
			size_t delimiterPos2 = file_name.find("+");
			string file_name2;
			string file_content;
			if (delimiterPos2 != string::npos)
			{
				// 提取分隔符之前的子字符串
				file_name2 = file_name.substr(0, delimiterPos2);
				// 提取分隔符之后的子字符串
				file_content = file_name.substr(delimiterPos2 + 1);

				//cout << "分隔符之前的子字符串: " << file_name2 << endl;
				//cout << "分隔符之后的子字符串: " << file_content << endl;
			}
			else
			{
				file_name = file_name;
			}
			vvv.mkfile(file_name2, file_content);
			cout << "make file /" << file_name << " ok " << endl;
		}
		//job类指令
		else if (zhilingtype == "./jdmc-job-create")
		{
		     int jobID= stoi(file_name);
		     jobManager.createJob(jobID, "");
			 jobManager.run(jobID);
		}
		else if (zhiling == "./jdmc-job-status")
		{
		     jobManager.displayJobStatus();
		}
		else if (zhilingtype == "./jdmc-job-suspend")
		{
		     int jobID = stoi(file_name);
		     jobManager.suspendJob(jobID);
        }
		else if (zhilingtype == "./jdmc-job-cancel")
		{
		     int jobID = stoi(file_name);
			 jobManager.cancelJob(jobID);
        }
		else if (zhilingtype == "./jdmc-job-resume")
		{
		     int jobID = stoi(file_name);
		     jobManager.resumeJob(jobID);
		     jobManager.run(jobID);
        }
		else if (zhilingtype == "./jdmc-job-log")
		{
		      int jobID = stoi(file_name);
		      jobManager.logJob(jobID);
		}
		else if (zhiling == "./jdmc-job-clear")
		{
		       jobManager.clearJobs();
        }
		else
		{
			cout << "没有该指令或指令参数不足！" << endl;
		}


		// 恢复原来的输出流缓冲区
		cout.rdbuf(oldCoutBuffer);

		// 从缓冲区中获取输出信息
		string output = buffer.str();

		// 打印输出信息
		cout << "捕获到的输出信息是: " << output;

        cout << "回复信息：" << endl;
		send(sockCli, output.c_str(), sizeof(sendbuf), 0);
	}

	closesocket(sockSer);
	closesocket(sockCli);
	WSACleanup();
	return 0;


}
