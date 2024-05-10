#include<iostream>
#include<string>
#include<cstring>
#include<stdio.h>
#include<stdlib.h>
#include<iomanip>
#include<fstream>
#include<Windows.h>
#include<cctype>
#include<strstream>
#include<sstream>
#include <pdh.h>
#include <pdhmsg.h>
#include <psapi.h>

#define N 1000
#define disk_size 8
#define D_N 1024
using namespace std;

//��˳�����������̵���Ϣ �����̷ֳ������֣�0~127�Ǵ���Ŀ¼�Ŀռ�  128~255��inode�Ĵ���ռ�  256~1023�Ǵ����ļ����ݵĿռ�
//������1024��ռ�
typedef struct Disk
{
    int state = 0;//���̵�״̬��0Ϊ�գ�1Ϊ��
    string content;
}Disk;
Disk disk[D_N];

//����һ���ڵ����ϸ��Ϣ��Ҳ����FCB��,���ڴ����ļ��Ĵ��̴洢��Ϣ
typedef struct Inode
{
    int dir_num;
    float pos_start = -1;
    float size = 0;//��ָ�ļ����ַ����ȣ������Ǵ��̳���
    string content;
}Inode;
//һ��Ŀ¼��
typedef struct Dentry
{
    int type = 1;//�ļ�����  �ļ�/Ŀ¼  0/1
    int last = -1;
    int num = 0;
    string name = "";
    struct Inode inode;
    string isdir = "<Dir>";
    string time = "";
}Dentry;
//��ǰ����Ŀ¼���ź�
int number = 0;
Dentry dentry[N];
class vfs
{
public:
    //Ѱ���ڴ����д������ʼλ��
    int find_pos(int size)
    {
        int n = size / disk_size;
        if ((size % disk_size) != 0)
        {
            n = n + 1;
        }

        int i = 256;
        while (i < D_N)
        {
            if (disk[i].state == 0)
            {
                int j = i + 1, p = 0;
                for (j; j < i + n; j++)
                {
                    if (disk[j].state != 1)
                    {

                    }
                    else
                    {
                        p = p + 1;
                    }
                }
                if (p == 0)
                {
                    //cout << "����i  " <<i<< endl;
                    return i;

                }
                else
                {
                    i += 1;
                }
            }
            else
            {
                i += 1;
            }
        }
        if (i == D_N)
        {
            cout << "�洢�ռ�������" << endl;
            return -1;
        }
    }
    //��������д���ݣ�дdisk_size���ַ�
    void disk_write(string& con, int num)
    {
        if (con.size() >= disk_size)
        {
            disk[num].content = con.substr(0, disk_size);
            //cout << num << "  " << disk[num].content << endl;
            con = con.substr(disk_size, con.size());
        }
        else
        {
            disk[num].content = con.substr(0, con.size());
        }
        disk[num].state = 1;
    }

    //�õ���һ��·�����ļ���
    string getfirstname(string& path)
    {
        int i = 1;
        for (i; i < path.size(); i++)
        {
            if (path[i] != '/')
            {
                continue;
            }
            else
            {
                break;
            }
        }
        //    cout << "i is : " << i << endl;
        if (i == path.size() - 1)
        {
            string name = path.substr(1, i);
            path = "";
            return name;
        }
        else
        {
            string name = path.substr(1, i - 1);
            path = path.substr(name.size() + 1, path.size());
            //       cout << "path is : " << path << endl;
            //        cout << "name is : " << name << endl;
            return name;
        }
    }
    //˳�򰤸������ļ�
    int find_file_shun(string path)
    {
        //    cout << "path is : " << path << endl;
        string dir = path;
        int num = -1;
        while (1)
        {
            string name;
            //        cout << "name is : " << name << endl;
            if (dir == "")
            {
                break;
            }
            name = getfirstname(dir);
            int i = 0;
            for (i; i < N; i++)
            {
                if (dentry[i].name == name)
                {
                    if (dentry[i].last == num)
                    {
                        num = dentry[i].num;
                        break;
                    }
                }
            }
            if (i >= N)
            {
                cout << "����ʧ�ܣ�û�и��ļ���" << endl;
                return -1;
            }
        }
        //    cout << "number is : " << num << endl;
        return num;
    }
    //��ʾ��ǰ��·��Ŀ¼
    void show_pos()
    {
        int num = number;
        string paths;
        while (num != -1)
        {

            if (dentry[num].type == 1 && dentry[num].name != "root")
            {
                cout << dentry[num].time + "  " + dentry[num].isdir + "    " +
                    "/" + dentry[num].name << endl;
            }
            if (dentry[num].type == 0)
            {
                cout << dentry[num].time + "  " << dentry[num].inode.size << "    " <<
                    dentry[num].name << endl;
            }
            paths = "/" + dentry[num].name + paths;
            num = dentry[num].last;
        }
        paths = paths + "   ";
        //    cout << "·��Ϊ��" ;
        cout << paths << endl;
    }
    //����·�����������һ���ļ������
    void inputs(string path)
    {
        show_pos();
        //string path;
        //cin >> path;
        number = find_file_shun(path);
    }
    //����ǰnumber�ŵ�Ŀ¼������Ŀ¼���ļ���Ҫɾ��  num=dentry[number].last
    int delate(int num)
    {
        for (int i = 1; i < N; i++)
        {
            if (dentry[i].last == num)
            {
                int d = dentry[i].num;
                //            cout << "d is : " << d << endl;
                dentry[i].type = 1;
                dentry[i].last = -1;
                dentry[i].num = 0;
                dentry[i].inode;
                dentry[i].name = "";
                dentry[i].isdir = "";
                dentry[i].time = "";
                //            cout << " !!! " << endl;
                delate(d);
            }
            else
            {
                //            cout << " ... " << endl;
                if (i == N - 1)
                {
                    return 0;
                }
            }
        }

    }

    //����һ��Ŀ¼
    int make_dir(string name_file)
    {
        /*cout << "������Ҫ�ڵ�ǰĿ¼��Ҫ�������ļ�����" << endl;
        string name_file;
        cin >> name_file;*/
        int i = 0;
        while (dentry[i].name != "")
        {
            i += 1;
        }
        SYSTEMTIME t;
        GetLocalTime(&t);
        string time = to_string(t.wYear) + "/" + to_string(t.wMonth) + "/"
            + to_string(t.wDay) + "    " + to_string(t.wHour) + ":" + to_string(t.wMinute);
        size_t delimiterPos = name_file.find(".txt");
        dentry[i].name = name_file;
        dentry[i].last = number;
        dentry[i].time = time;
        if (delimiterPos != string::npos)
        {
            dentry[i].isdir = "";
        }
        else
        {
            dentry[i].isdir = "<DIR>";;
        }
        dentry[i].num = i;
        return dentry[i].num;
    }
    //����һ��inode�ڵ�,
    void make_file(int num, string content)
    {
        dentry[num].type = 0;
        //cout << "�������ļ����ݣ� " << endl;
        //string content;
        //getchar();
        //getline(cin, content);

        dentry[num].inode.content = content;
        //cout << content << endl;
        dentry[num].inode.size = content.size();
        dentry[num].inode.dir_num = dentry[num].num;
        dentry[num].inode.pos_start = find_pos(dentry[num].inode.size);
        //cout << "find pos is: " << dentry[num].inode->pos_start << endl;
        if (dentry[num].inode.pos_start != -1)
        {
            int size = dentry[num].inode.size;
            int n = size / disk_size;
            if ((size % disk_size) != 0)
            {
                n = n + 1;
            }
            //cout << " ���������Э����" << endl;
            for (int i = dentry[num].inode.pos_start; i < dentry[num].inode.pos_start + n; i++)
            {
                //cout << "disk_write֮ǰ" << endl;
                disk_write(content, i);
            }
        }
        else
        {
            //û�д���ռ�����
        }

        //cout <<"�����ļ�����ʱ��"<< dentry[num].inode.dir_num << " " << dentry[num].inode.pos_start << " " << dentry[num].inode.size << endl;
    }

    //���ļ�ϵͳ������������洢��ַ�ռ�
    void out_file()
    {
        for (int p = 128; p < 256; p++)
        {
            disk[p].state = 0;
            disk[p].content = "";
        }
        ofstream OutFile("Test.txt");
        //    OutFile << "�����洢����"<<endl;
        for (int i = 0; i < N; i++)//Ŀ¼
        {
            string dir_text, last, num, type;
            strstream sl, sn, st;
            sl << dentry[i].last;
            sl >> last;
            sn << dentry[i].num;
            sn >> num;
            st << dentry[i].type;
            st >> type;

            dir_text = " " + last + " " + num + " " + type + " " + dentry[i].name;
            for (int j = 0; j < 128; j++)//��Ŀ¼��ϢҲ�洢��������
            {
                if (disk[j].state == 0)
                {
                    disk[j].content = dir_text;
                    //    cout << "dir_text is : " << dir_text << endl;
                    disk[j].state = 1;
                    break;
                }
            }
            //���ļ���inode��Ϣ�洢��������
            if (dentry[i].type == 0)
            {
                //cout << "i is: " << i << endl;
                string inode, pos, size, dir_num;
                strstream sp, ss, sd;
                //cout << "�洢inode��Ϣ" << endl;
                //cout << dentry[i].inode.pos_start << endl;
                int pos_s = dentry[i].inode.pos_start;
                sp << pos_s;
                sp >> pos;
                //cout << "pos is: " << pos << endl;
                //cout << dentry[i].inode.size<<endl;
                int size_s = dentry[i].inode.size;
                ss << size_s;
                ss >> size;
                //cout << "size is: " << size << endl;
                //cout << dentry[i].inode.dir_num << endl;
                int dir_s = dentry[i].inode.dir_num;
                sd << dir_s;
                sd >> dir_num;
                //cout << "dir_num is: " << endl;
                inode = " " + pos + " " + size + " " + dir_num;
                for (int p = 128; p < 256; p++)
                {
                    if (disk[p].state == 0)
                    {
                        disk[p].content = inode;
                        disk[p].state = 1;
                        break;
                    }
                }
            }
        }
        for (int i = 0; i < D_N; i++)
        {
            OutFile << disk[i].content << endl;
        }
        OutFile.close();
    }
    //��õ�һ������
    string get_first(string& path)
    {
        int i = 1;
        if (path == " ")
        {
            string name = "";
            path = "";
            return name;
        }
        for (i; i < path.size(); i++)
        {
            if (path[i] != ' ')
            {
                continue;
                if (i == path.size() - 1)//��������ַ���û�пո��ʱ��
                {
                    i = i + 1;
                }
            }
            else
            {
                break;
            }
        }
        //cout << "i is : " << i << endl;
        if ((i >= path.size() - 1) || (path == " "))
        {
            string name = path.substr(1, i);
            path = " ";
            //cout << "i is dayudengyu:  " << endl;
            return name;
        }
        else
        {
            string name = path.substr(1, i - 1);
            if (name.size() + 1 == path.size())//ĩβ���ַ���û�пո�����
            {
                path = " ";
                return name;
            }
            else
            {
                path = path.substr(name.size() + 1, path.size());
                //cout << "path is : " << path << endl;
                //cout << "name is : " << name << endl;
                return name;
            }
        }
    }
    //�������ڵ����ݶ����ڴ���
    void read_file()
    {
        ifstream ins("Test.txt");
        if (!ins.is_open())
        {
            cout << "���ļ�ʧ�ܣ�" << endl;
            dentry[0].name = "root";
            dentry[0].last = -1;
            dentry[0].num = 0;
            dentry[0].inode;
        }
        else
        {
            for (int j = 0; j < D_N; j++)
            {
                if (j >= 256)//��ȡ�����ļ���������
                {
                    string con;
                    getline(ins, con);
                    //cout << "con is: " << con << endl;
                    if (con != "")
                    {
                        disk[j].content = con;
                        //cout << disk[j].content << endl;
                        disk[j].state = 1;
                    }
                    //cout << "disk is : " << con << " " << j << endl;
                }
                if ((j >= 128) && (j < 256))//��ȡinode����ϸ����
                {
                    string con;
                    getline(ins, con);
                    //cout << "con is : " << con << " " << j << endl;
                    if (con.size() == 0)
                    {

                    }
                    else
                    {
                        Inode inodes;
                        istringstream pos(get_first(con));
                        pos >> inodes.pos_start;
                        istringstream size(get_first(con));
                        size >> inodes.size;
                        istringstream dir_num(get_first(con));
                        dir_num >> inodes.dir_num;
                        //��inode��Ŀ¼����һ��
                        for (int q = 0; q < N; q++)
                        {
                            if (dentry[q].num == inodes.dir_num)
                            {
                                dentry[q].inode = inodes;
                            }
                        }
                    }
                }
                if (j < 128)//��ȡ����Ŀ¼��������
                {
                    string context;
                    getline(ins, context);
                    if ((context == " ") || (context == ""))
                    {
                        //cout << "ȫ������Ϊ��" << endl;
                    }
                    else
                    {
                        int nums = 0;
                        istringstream last(get_first(context));
                        istringstream num(get_first(context));
                        num >> nums;
                        istringstream type(get_first(context));


                        last >> dentry[nums].last;
                        //cout << dentry[nums].last << " ";
                        dentry[nums].num = nums;
                        //cout << dentry[nums].num << " ";
                        dentry[nums].name = get_first(context);
                        //cout << dentry[nums].name << " ";
                        type >> dentry[nums].type;
                        //cout << dentry[nums].type << " ";
                        //cout<< j << endl;
                    }
                    dentry[0].name = "root";
                }
            }
        }
        ins.close();
    }

    void upload_file(string name_file)
    {
        // cout << "������Ҫ�ڵ�ǰĿ¼�»�ȡ�ı����ļ�·����" << endl;
         //string name_file;
         //cin >> name_file;
         // ���ļ�
        ifstream file(name_file);
        if (!file.is_open()) {
            cout << "�޷����ļ���" << name_file << std::endl;
        }
        //�ڵ�ǰĿ¼����ͬ���ļ�
        int i = 0;
        while (dentry[i].name != "")
        {
            i += 1;
        }
        SYSTEMTIME t;
        GetLocalTime(&t);
        string time = to_string(t.wYear) + "/" + to_string(t.wMonth) + "/"
            + to_string(t.wDay) + "    " + to_string(t.wHour) + ":" + to_string(t.wMinute);
        size_t delimiterPos = name_file.find(".txt");
        dentry[i].time = time;
        dentry[i].name = name_file;
        dentry[i].last = number;
        dentry[i].num = i;

        dentry[dentry[i].num].type = 0;
        //cout << "�������ļ����ݣ� " << endl;
        //getchar();
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

        dentry[dentry[i].num].inode.content = content;
        //cout << content << endl;
        dentry[dentry[i].num].inode.size = content.size();
        dentry[dentry[i].num].inode.dir_num = dentry[dentry[i].num].num;
        dentry[dentry[i].num].inode.pos_start = find_pos(dentry[dentry[i].num].inode.size);
        //cout << "find pos is: " << dentry[num].inode->pos_start << endl;
        if (dentry[dentry[i].num].inode.pos_start != -1)
        {
            int size = dentry[dentry[i].num].inode.size;
            int n = size / disk_size;
            if ((size % disk_size) != 0)
            {
                n = n + 1;
            }
            //cout << " ���������Э����" << endl;
            for (int i2 = dentry[dentry[i].num].inode.pos_start; i2 < dentry[dentry[i].num].inode.pos_start + n; i2++)
            {
                //cout << "disk_write֮ǰ" << endl;
                disk_write(content, i2);
            }
        }
        else
        {
            //û�д���ռ�����
        }

        // �ر��ļ�
        file.close();

        //cout <<"�����ļ�����ʱ��"<< dentry[num].inode.dir_num << " " << dentry[num].inode.pos_start << " " << dentry[num].inode.size << endl;
    }

    string GetFilenameFromPath(const string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash != string::npos) {
            return path.substr(lastSlash + 1);
        }
        return path;
    }

    void download_file(string path)
    {
        //cout << "������Ҫ���ص����ص��ļ�·����" << endl;
        //string path;
        //cin >> path;
        int num = find_file_shun(path);
        if ((num == -1) || (dentry[num].type == 1))
        {

        }
        else
        {
            string con;
            int disk_num = dentry[num].inode.pos_start;
            int size = dentry[num].inode.size;
            int n = size / disk_size;
            if (size % disk_size != 0)
            {
                n = n + 1;
            }
            for (int i = 0; i < n; i++)
            {
                con = con + disk[disk_num + i].content;
            }
            cout << con << endl;
            string filename = GetFilenameFromPath(path);
            // �������ļ�������д��ģʽ
            ofstream file(filename);
            if (!file.is_open()) {
                cout << "�޷������ļ���" << filename << endl;
            }
            // ������д���ļ�
            file << con;
            // �ر��ļ�
            file.close();
        }
    }

    void cd(string name)
    {
        //cout << "������·����" << endl;
        inputs(name);
    }

    void dir()
    {
        int i = 0, num = number;
        while (i < N)
        {
            if (dentry[i].last == num)
            {
                number = i;
                show_pos();
            }
            i += 1;
        }
        //cout << endl;
        number = num;
    }
  
    void mkfile(string name_file, string file_content)
    {
        int num = make_dir(name_file);
        make_file(num, file_content);
        //cout << "mkfile��������pos: " << dentry[num].inode.pos_start << endl;
    }

    void rmdir(string name)//ɾ��Ŀ¼��Ŀ¼�µ������ļ�
    {
        //cout << "������Ҫɾ����·���ļ���" << endl;
        inputs(name);
        //    cout << "����·��֮���number: " << number << endl;
        dentry[number].name = "";
        dentry[number].last = -1;
        dentry[number].num = 0;
        dentry[number].inode;
        dentry[number].type = 1;
        delate(number);
        number = 0;
    }
    void save()
    {
        for (int i = 0; i < N; i++)
        {
            if (dentry[i].type == 0)
            {
                //cout << "save������ͷ " <<endl;
                //cout<< dentry[i].inode.pos_start << endl;
                //cout << "222" << endl;

            }
        }
        out_file();
    }


    void openfile(string path)
    {
        //cout << "������Ҫ�򿪵�·���ļ���" << endl;
        //string path;
        //cin >> path;
        int num = find_file_shun(path);
        if ((num == -1) || (dentry[num].type == 1))
        {

        }
        else
        {
            cout << "�ļ������ǣ�" << endl;
            string con;
            int disk_num = dentry[num].inode.pos_start;
            int size = dentry[num].inode.size;
            int n = size / disk_size;
            if (size % disk_size != 0)
            {
                n = n + 1;
            }
            for (int i = 0; i < n; i++)
            {
                con = con + disk[disk_num + i].content;
            }
            cout << con << endl;
        }
    }
    void format_disk()
    {
        delate(0);
    }
};