#include <iostream>
using namespace std;
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")

const int BUF_SIZE = 2048;			//����������
char sendbuf[BUF_SIZE];		//���ͻ�����
char recvbuf[BUF_SIZE];		//���ܻ�����

void initialization()
{

	WSADATA wsadata;

	WORD wsaversion = MAKEWORD(2, 2);

	if (WSAStartup(wsaversion, &wsadata) != 0)
	{
		cout << "����socket��ʧ��" << endl;
	}
	else {
		cout << "��ʼ���ɹ�" << endl;
	}

}

int main()
{
	SOCKET sockCli;	//�����������׽���

	//SOCKADDR_IN��ϵͳ��װ��һ���ṹ�壬��������˳�Ա������sin_family��sin_addr��sin_zero
	SOCKADDR_IN addrSer;

	initialization();

	//�����׽���
	sockCli = socket(AF_INET, SOCK_STREAM, 0);


	//��ʼ����ַ��
	//addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	inet_pton(AF_INET, "127.0.0.1", &addrSer.sin_addr.s_addr);
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(8000);



	// �ڽ����׽����ļ��������ɹ�����Ҫ���׽��ֽ��е�ַ�Ͷ˿ڵİ󶨣����ܽ������ݵĽ��պͷ��Ͳ�����
	if (connect(sockCli, (SOCKADDR*)&addrSer, sizeof(addrSer)) == SOCKET_ERROR)
	{
		cout << "����������ʧ��" << endl;
		WSACleanup();
	}
	else {
		cout << "���������ӳɹ�" << endl;
	}


	//��������:
	while (true)
	{
		cout << "�����뷢����Ϣ��" << endl;
		cout << "$" ;
		cin >> sendbuf;

		int sendlen = send(sockCli, sendbuf, sizeof(sendbuf), 0);
		if (sendlen < 0) {
			cout << "����ʧ�ܣ�" << endl;
			break;
		}

		int recvlen = recv(sockCli, recvbuf, sizeof(sendbuf), 0);
		if (recvlen < 0) {
			cout << "����ʧ�ܣ�" << endl;
			break;
		}
		else {
			//cout << "�������Ϣ:" << recvbuf << endl;
			cout << recvbuf << endl;
			string recv = "";
			for (int i = 0; i < 8; i++)
			{
				recv = recv + recvbuf[i];
			}
			if (recv == "Shutdown")
			{
				break;
			}
		}
	}
	closesocket(sockCli);
	WSACleanup();
	return 0;
}
