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

//用顺序表来管理磁盘的信息 将磁盘分成两部分：0~127是储存目录的空间  128~255是inode的储存空间  256~1023是储存文件内容的空间
//磁盘有1024块空间
typedef struct Disk
{
    int state = 0;//磁盘的状态，0为空，1为满
    string content;
}Disk;
Disk disk[D_N];

//这是一个节点的详细信息（也就是FCB）,用于储存文件的磁盘存储信息
typedef struct Inode
{
    int dir_num;
    float pos_start = -1;
    float size = 0;//是指文件的字符长度，并不是磁盘长度
    string content;
}Inode;
//一个目录项
typedef struct Dentry
{
    int type = 1;//文件类型  文件/目录  0/1
    int last = -1;
    int num = 0;
    string name = "";
    struct Inode inode;
    string isdir = "<Dir>";
    string time = "";
}Dentry;
//当前所处目录的信号
int number = 0;
Dentry dentry[N];
class vfs
{
public:
    //寻找在磁盘中储存的起始位置
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
                    //cout << "返回i  " <<i<< endl;
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
            cout << "存储空间已满！" << endl;
            return -1;
        }
    }
    //往磁盘中写数据，写disk_size个字符
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

    //得到第一个路径的文件名
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
    //顺序挨个查找文件
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
                cout << "查找失败，没有该文件！" << endl;
                return -1;
            }
        }
        //    cout << "number is : " << num << endl;
        return num;
    }
    //显示当前的路径目录
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
        //    cout << "路径为：" ;
        cout << paths << endl;
    }
    //输入路径并返回最后一个文件的序号
    void inputs(string path)
    {
        show_pos();
        //string path;
        //cin >> path;
        number = find_file_shun(path);
    }
    //将当前number号的目录下所有目录和文件都要删除  num=dentry[number].last
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

    //创建一个目录
    int make_dir(string name_file)
    {
        /*cout << "请输入要在当前目录下要创建的文件名：" << endl;
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
    //创建一个inode节点,
    void make_file(int num, string content)
    {
        dentry[num].type = 0;
        //cout << "请输入文件内容： " << endl;
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
            //cout << " 向磁盘区域协数据" << endl;
            for (int i = dentry[num].inode.pos_start; i < dentry[num].inode.pos_start + n; i++)
            {
                //cout << "disk_write之前" << endl;
                disk_write(content, i);
            }
        }
        else
        {
            //没有储存空间的情况
        }

        //cout <<"创建文件结束时："<< dentry[num].inode.dir_num << " " << dentry[num].inode.pos_start << " " << dentry[num].inode.size << endl;
    }

    //将文件系统的内容输出到存储地址空间
    void out_file()
    {
        for (int p = 128; p < 256; p++)
        {
            disk[p].state = 0;
            disk[p].content = "";
        }
        ofstream OutFile("Test.txt");
        //    OutFile << "连续存储区域"<<endl;
        for (int i = 0; i < N; i++)//目录
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
            for (int j = 0; j < 128; j++)//将目录信息也存储到磁盘中
            {
                if (disk[j].state == 0)
                {
                    disk[j].content = dir_text;
                    //    cout << "dir_text is : " << dir_text << endl;
                    disk[j].state = 1;
                    break;
                }
            }
            //将文件的inode信息存储到磁盘中
            if (dentry[i].type == 0)
            {
                //cout << "i is: " << i << endl;
                string inode, pos, size, dir_num;
                strstream sp, ss, sd;
                //cout << "存储inode信息" << endl;
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
    //获得第一个参数
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
                if (i == path.size() - 1)//当最后是字符，没有空格的时候
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
            if (name.size() + 1 == path.size())//末尾是字符，没有空格的情况
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
    //将磁盘内的数据读到内存中
    void read_file()
    {
        ifstream ins("Test.txt");
        if (!ins.is_open())
        {
            cout << "打开文件失败！" << endl;
            dentry[0].name = "root";
            dentry[0].last = -1;
            dentry[0].num = 0;
            dentry[0].inode;
        }
        else
        {
            for (int j = 0; j < D_N; j++)
            {
                if (j >= 256)//读取磁盘文件内容数据
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
                if ((j >= 128) && (j < 256))//读取inode的详细数据
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
                        //将inode与目录连在一起
                        for (int q = 0; q < N; q++)
                        {
                            if (dentry[q].num == inodes.dir_num)
                            {
                                dentry[q].inode = inodes;
                            }
                        }
                    }
                }
                if (j < 128)//读取磁盘目录内容数据
                {
                    string context;
                    getline(ins, context);
                    if ((context == " ") || (context == ""))
                    {
                        //cout << "全部内容为空" << endl;
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
        // cout << "请输入要在当前目录下获取的本地文件路径：" << endl;
         //string name_file;
         //cin >> name_file;
         // 打开文件
        ifstream file(name_file);
        if (!file.is_open()) {
            cout << "无法打开文件：" << name_file << std::endl;
        }
        //在当前目录创建同名文件
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
        //cout << "请输入文件内容： " << endl;
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
            //cout << " 向磁盘区域协数据" << endl;
            for (int i2 = dentry[dentry[i].num].inode.pos_start; i2 < dentry[dentry[i].num].inode.pos_start + n; i2++)
            {
                //cout << "disk_write之前" << endl;
                disk_write(content, i2);
            }
        }
        else
        {
            //没有储存空间的情况
        }

        // 关闭文件
        file.close();

        //cout <<"创建文件结束时："<< dentry[num].inode.dir_num << " " << dentry[num].inode.pos_start << " " << dentry[num].inode.size << endl;
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
        //cout << "请输入要下载到本地的文件路径：" << endl;
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
            // 创建新文件并打开以写入模式
            ofstream file(filename);
            if (!file.is_open()) {
                cout << "无法创建文件：" << filename << endl;
            }
            // 将内容写入文件
            file << con;
            // 关闭文件
            file.close();
        }
    }

    void cd(string name)
    {
        //cout << "请输入路径：" << endl;
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
        //cout << "mkfile函数最后的pos: " << dentry[num].inode.pos_start << endl;
    }

    void rmdir(string name)//删除目录及目录下的所有文件
    {
        //cout << "请输入要删除的路径文件：" << endl;
        inputs(name);
        //    cout << "输入路径之后的number: " << number << endl;
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
                //cout << "save函数开头 " <<endl;
                //cout<< dentry[i].inode.pos_start << endl;
                //cout << "222" << endl;

            }
        }
        out_file();
    }


    void openfile(string path)
    {
        //cout << "请输入要打开的路径文件：" << endl;
        //string path;
        //cin >> path;
        int num = find_file_shun(path);
        if ((num == -1) || (dentry[num].type == 1))
        {

        }
        else
        {
            cout << "文件内容是：" << endl;
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