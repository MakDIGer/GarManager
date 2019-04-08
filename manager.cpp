/*
 * manager.cpp
 * 
 * Copyright 2015-2018 Nikolay <Nikolay@REX>
 * 
 * Version 1.42
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include "windows.h"
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>
#include <ctime> 

using namespace std;

string files[128], day, buffer_line;
int copy_mode;

class pcs
{
	public:
		string garantsrv;
		string start;
		pcs(string garantsrv_new, string start_new);
};

pcs::pcs (string garantsrv_new, string start_new)
{
	garantsrv = garantsrv_new;
	start = start_new;
} 

void extrDIR(string indir)
{
	char dir1[128];
	
	buffer_line = indir+"*";
	strncpy(dir1, buffer_line.c_str(), buffer_line.length()+1);
	
	int count = 0;
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;
	hf=FindFirstFile(dir1, &FindFileData);
	if (hf!=INVALID_HANDLE_VALUE)
	{
		do
		{
			files[count] = FindFileData.cFileName; count++;
		}
		while (FindNextFile(hf,&FindFileData)!=0);
		FindClose(hf);
	}
}

void clearDIR(string folder1)
{
	char dir1[128];
	int x;
	
	for (x=2;x<128;x++) 
	{
		files[x] = "";
	}
	
	extrDIR(folder1);
	for (int i=2;i<128;i++)
	{
		if (files[i] == "")
		{
			i = 128; 
		} else {
					buffer_line = folder1 + files[i];
					strncpy(dir1, buffer_line.c_str(), buffer_line.length()+1);
					remove(dir1);
				}
	}
}

void input_date()
{
	cout << "Enter the date of the next day(DD-MM-YEAR): ";
	cin >> day; cout << endl;
	if (day == "" or day.length() != 10)
	{
		cout << "Error in this date!!!" << endl;
		input_date();
	}
}

void input_copy_mode()
{
	cout << "Enter the number mode of copying: " << endl;
	cout << "0 - none" << endl;
	cout << "1 - 11;" << endl;
	cout << "2 - Rex, 11;" << endl;
	cout << "3 - Rex, Monster, 11;" << endl;
	cout << "4 - Rex, Monster, 11, 10;" << endl;
	cout << "Only number, please: ";
	cin >> copy_mode; cout << endl;
	if (copy_mode > 4 or copy_mode <0)
	{
		cout << "Error!!!" << endl;
		input_copy_mode();
	}
}

int check_file(string dir1, string ext_file)
{
	int x;
	
	for (x=2;x<128;x++) 
	{
		files[x] = "";
	}
	extrDIR(dir1);
	for (int i=2;i<128;i++)
	{
		if (files[i]=="")
		{
			i=128;
		} else 
		{
			if (files[i].substr(files[i].length()-3, 3) == ext_file)
			{
				cout << "There are file *." << ext_file << " at the directory " << dir1 << endl;
				return 1;
			}
		}
	}
	return 0;
}

int check_process(string buffer_line)
{
	char process_name[64];
	strncpy(process_name, buffer_line.c_str(), buffer_line.length()+1);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe);

	if (strcmp(pe.szExeFile, process_name) == 0)
		{
			return 1;
		}
	while (Process32Next(hSnapshot, &pe))
		{
			if (strcmp(pe.szExeFile, process_name) == 0)
				{
					return 1;
				}
		}
		return 0;
}

void run_cmd(string buffer_line)
{
	char command[256];
	
	strncpy(command, buffer_line.c_str(), buffer_line.length()+1);
	system(command);
}

void checking_process(string process_name_check)
{
if (check_process(process_name_check))
	{
		cout << process_name_check << "::The process is working. Please, wait." << endl;
		Sleep(600000);
		checking_process(process_name_check);
	}
	else
	{
		cout << process_name_check << "::The process has finished." << endl;
	}	
}

void download(string day)
{
	cout << endl;
	buffer_line = "E:\\Garant-FS801\\download.exe -auto -to " + day + " -dsc 90 -revision -nofinalmessage" + day;
	cout << buffer_line << endl;
	run_cmd(buffer_line);
	cout << "Waiting for the finish of downloading" << endl;
	Sleep(300000);
	
	cout << "Checking the downloding" << endl;
	checking_process("f1download.run");
	Sleep(300000);
	
	if (check_file("E:\\Garant-FS801\\delta\\","zip"))
	{
		cout << "There is ZIP file." << endl;
	} else
	{
		cout << "There isn't ZIP file." << endl;
		download(day);
	}
}

string time_now()
{
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	return asctime (timeinfo);
}

void copydir(string folder1, string folder2)
{
	int x,y;
	char szPath1[128], szPath2[128],zvsim[128];
	string address_file1, address_file2;
	
	for (y=2;y<128;y++) 
	{
		files[y] = "";
	}

	extrDIR(folder1);
	cout << folder1 << endl;
	
	for (x = 2; x<128; ++x)
		{
			if (files[x] == "")
			{
				x = 128;
			}
			else
			{
				address_file1 = folder1 + files[x];
				address_file2 = folder2 + files[x];
				strncpy(szPath1, address_file1.c_str(), address_file1.length() + 1 );
				strncpy(szPath2, address_file2.c_str(), address_file2.length() + 1 );
				
				cout << szPath1 << " " << szPath2 << endl;
				
				if(!CopyFile(szPath1, szPath2, FALSE))
					{
						cout << "Couldn't copy file." << endl;
					}
			}
		}
}

int copy_file(string file1, string file2)
{
	char szPath1[128], szPath2[128],zvsim[128];
	
	strncpy(szPath1, file1.c_str(), file1.length() + 1 );
	strncpy(szPath2, file2.c_str(), file2.length() + 1 );
				
				cout << szPath1 << " " << szPath2 << endl;
				
				if(!CopyFile(szPath1, szPath2, FALSE))
					{
						cout << "Couldn't copy file." << endl;
					}
}

void copy_process(string addr_data, string pc_name, string addr_garantsrv, string addr_start)
{
	cout << endl;
	cout << time_now();
	cout << "Copying into " << pc_name << endl;
	clearDIR(addr_garantsrv);
	copydir(addr_data, addr_garantsrv);
	if (addr_start != "")
	{
		copy_file("E:\\Garant.SRV\\start.txt",addr_start);
	}
}

int copy_etalon(int copy_mode)
{
	string data1,data2,data;
	
	if (copy_mode == 0)
	{
		cout << "Copying cancelled, because mode of copying selected 0." << endl;
		return 1;
	}
	
	pcs monster("\\\\monster\\Garant.Srv\\F801\\","\\\\monster\\Garant.Srv\\start.txt");
	pcs rex("\\\\rex\\Garant.Srv\\F801\\","\\\\rex\\Garant.Srv\\start.txt");
	pcs rr10("\\\\rr10\\Garant.Srv\\F801\\","\\\\rr10\\Garant.Srv\\start.txt");
	pcs usepc1("E:\\Garant.SRV\\F801\\","E:\\Garant.SRV\\start.txt");
	
	data = "";
	data1 = "E:\\Garant-FS801\\data1\\";
	data2 = "E:\\Garant-FS801\\data2\\";
	
	if (check_file(data1,"ndt"))
	{
		data = data1;
	} 
	if (check_file(data2,"ndt"))
	{
		data = data2;
	}
	if (data == "")
	{
		cout << "Warning! There isn't etalon!!!" << endl;
		return 1;
	}
	copy_process(data, "usepc1", usepc1.garantsrv, "");
	
	if (copy_mode > 1)
	{
		copy_process(data, "rex", rex.garantsrv, rex.start);
	}
	if (copy_mode > 2)
	{
		copy_process(data, "monster", monster.garantsrv, monster.start);
	}
	if (copy_mode > 3)
	{
		copy_process(data, "rr10", rr10.garantsrv, rr10.start);
	}
	return 0;
}

void dataupd()
{
	cout << "E:\\Garant-FS801\\dataupd.exe -auto  -nofinalmessage" << endl;
	run_cmd("E:\\Garant-FS801\\dataupd.exe -auto  -nofinalmessage");
	Sleep(300000);
	
	cout << "Checking the updating" << endl;
	checking_process("f1dataupd.run");
	Sleep(300000);
}

int main()
{
	string line1, line2;
	
	cout << "..............:::Manager of the circle:::............." << endl;
	cout << ".................... Version 1.42 ...................." << endl;
	cout << "......................................................" << endl;
	cout << time_now();
	
	input_date();
	//input_copy_mode();
	
	//cout << "Turn on the internet connection." << endl;
	//run_cmd("rasdial Internet userpc1 20usern2pc1");
	
	//cout << "Deleting files at the Garant.srv directory............" << endl;
	//clearDIR("E:\\garant.srv\\F801\\");
	
	cout << "Deleting files at the delta directory................." << endl;
	clearDIR("E:\\Garant-FS801\\delta\\Archive\\");
	clearDIR("E:\\Garant-FS801\\delta\\");
	
	cout << "Dowloading the updates for Etalon....................." << endl;
	cout << time_now();
	download(day);
		
	//cout << "Turn off the internet connection." << endl;
	//run_cmd("rasdial Internet /DISCONNECT");
	
	cout << "Updating of the etalon" << endl;
	cout << time_now();
	dataupd();
	
	//cout << "Copying the etalon into other pc's" << endl;
	//if (copy_etalon(copy_mode))
	//{
	//	cout << "Etalon wasn't copying!" << endl;
	//	cout << "Circle has been launched unsuccessfully." << endl;
	//	cout << time_now();
	//	run_cmd("pause");
	//	return 0;
	//} else {
	//	cout << "Circle has been launched successfully." << endl;
	//	cout << time_now();
	//	run_cmd("C:\\MTurtle\\combin.exe");
	//	}
	run_cmd("C:\\MTurtle\\Combin.exe");
	run_cmd("pause");
	return 0;
}

