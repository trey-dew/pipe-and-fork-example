#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <limits>
#include <string>
#include <stdio.h>
#include <fstream>
#include <sys/types.h>
#include <sstream>
#include <stdlib.h>
#include <chrono>

#define msgSize 10

using namespace std;

int main()
{
    //aks user for number of child process
    char childProcesses = 0; 
    while(childProcesses != '1' && childProcesses != '2' && childProcesses != '4')
    {
        cout << "Select how many child processes: 1, 2, or 4: " << endl;
        cin.get(childProcesses);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    //asks user for which file they want to use
    char fileIndex = 0; 
    while(fileIndex != '1' && fileIndex != '2' && fileIndex != '3')
    {
        cout << "Select which file you want to use: 1, 2, or 3: " << endl;
        cin.get(fileIndex);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    //sets up the files and data sizes
    string line;
    ifstream file1("file1.dat");
    ifstream file2("file2.dat");
    ifstream file3("file3.dat");

    int data1 [1000];
    int data2 [10000];
    int data3 [100000];

    //reads from file1 and stores it in the data1 array
    int index = 0;
    while(file1.good())
    {
        string line;
        file1 >> line;
        stringstream strstream(line);
        strstream >> data1[index];
        index++;
    }
    //reads from file2 and stores it in the data2 array
    index = 0;
    while(file2.good())
    {
        string line;
        file2 >> line;
        stringstream strstream(line);
        strstream >> data2[index];
        index++;
    }  
    //reads from file3 and stores it in the data3 array
    index = 0;
    while(file3.good())
    {
        string line;
        file3 >> line;
        stringstream strstream(line);
        strstream >> data3[index];
        index++;
    }
    //child process if one is requested
    if(childProcesses == '1') 
    {
        int fds[2], n;
        pipe(fds);
        pid_t pid;
        pid = fork();
        //fork was unsuccesful
        if(pid == -1) 
        {
            cout << "error creating fork";
            exit(EXIT_FAILURE);
        }
        //child process begins
        if (pid == 0) 
        {
            close(fds[0]);
            int total = 0;

            if (fileIndex == '1')
            {
                for(int i = 0; i < sizeof(data1) / sizeof(data1[0]); i++)
                {
                    total += data1[i];
                }

                write(fds[1], &total, sizeof(total));
                close(fds[1]);
            }
            else if (fileIndex == '2')
            {
                for(int i = 0; i < sizeof(data2) / sizeof(data2[0]); i++)
                {
                    total += data2[i];
                }
                stringstream strStream;
                strStream << total;

                write(fds[1], &total, sizeof(total));
                close(fds[1]);
            }
            else if (fileIndex == '3')
            {
                for(int i = 0; i < sizeof(data3) / sizeof(data3[0]); i++)
                {
                    total += data3[i];
                }
                stringstream strStream;
                strStream << total;

                write(fds[1], &total, sizeof(total));
                close(fds[1]);
            }
            exit(EXIT_SUCCESS);
        }
        //parent process begins if there is one child
        else 
        {
            close(fds[1]);
            int total;
            int result;
            // gets the time for how long the process took
            chrono::steady_clock::time_point start = chrono::steady_clock::now();
            wait(NULL);                    
            chrono::steady_clock::time_point end = chrono::steady_clock::now();
            
            read(fds[0], &total, sizeof(int));
            close(fds[0]);
            
            cout << "child results: " << total << endl;
            
            cout << "Final result: " << total << endl;

            cout << "Time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << endl;

            exit(EXIT_SUCCESS);
        }
    }        
    // child process if there are 2 requested
    if(childProcesses == '2') 
    {
        //prepares varaibles
        chrono::steady_clock::time_point start, end;
        long final;
        int total1;
        int total2;
        int count = 2;

        int fds[count][2];
        pid_t pid[count];

        pipe(fds[0]);
        pipe(fds[1]);
        
        for(int i = 0; i < count; i++)
        {           
            pid[i] = fork();
            if(pid[i] == -1)
            {
                cout << "error creating fork";
                
            }
            //creates the child process
            if(pid[i] == 0) 
            {
                //create child process for file 1
                if (fileIndex == '1')
                {
                    int total = 0;
                    //takes the first half of the data set for the first file
                    if(i == 0)
                    {
                        for(int j = 0; j < sizeof(data1) / sizeof(data1[0]) / 2; j++)
                        {
                            total += data1[j];
                        }
                        
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);                       
                    }
                    //takes the second half of the data set for the first file
                    else
                    {
                        for(int j = 0; j < sizeof(data1) / sizeof(data1[0]) / 2; j++)
                        {
                            total += data1[j + 500];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                }
                // child process if its file 2
                else if(fileIndex == '2')
                {
                    int total = 0;
                    if(i == 0)
                    {
                        //takes the first half of the data set for the second file
                        for(int j = 0; j < sizeof(data2) / sizeof(data2[0]) / 2; j++)
                        {
                            total += data2[j];
                        }
                        
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else
                    {
                        //takes the second half of the data set for the second file
                        for(int j = 0; j < sizeof(data2) / sizeof(data2[0]) / 2; j++)
                        {
                            total += data2[j + 5000];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                }
                //creates child process for file 3
                else if(fileIndex =='3')
                {
                    int total = 0;
                    if(i == 0)
                    {
                        //takes the first half of the data set for the third file
                        for(int j = 0; j < sizeof(data3) / sizeof(data3[0]) / 2; j++)
                        {
                            total += data3[j];
                        }
                        
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else
                    {
                        //takes the second half of the data set for the third file
                        for(int j = 0; j < sizeof(data3) / sizeof(data3[0]) / 2; j++)
                        {
                            total += data3[j + 50000];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                }    
            }
            //creates parent process
            else 
            {          
                if (i == 0)
                {
                    start = chrono::steady_clock::now();
                }
                else if (i == 1)
                {
                    wait(NULL);
                    
                    read(fds[0][0], &total1, sizeof(int));
                    read(fds[1][0], &total2, sizeof(int));
                    end = chrono::steady_clock::now();

                    long result = total1 + total2;
                    cout << "child 1 results: " << total1 << endl;

                    cout << "child 2 results: " << total2 << endl;

                    cout << "Final result: " << result << endl;

                    cout << "Time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << endl;
                }
            }
        } 
    }
    //child process if 4 are requested
    if(childProcesses == '4') 
    {
        chrono::steady_clock::time_point start, end;
        long final;
        int total1;
        int total2;
        int total3;
        int total4;
        int count = 4;

        int fds[count][2];
        pid_t pid[count];

        pipe(fds[0]);
        pipe(fds[1]);
        pipe(fds[2]);
        pipe(fds[3]);
        for(int i = 0; i < count; i++)
        {
            pid[i] = fork();
            if(pid[i] == -1)
            {
                cout << "error creating fork";  
            }
            
            if(pid[i] == 0) 
            {
                //child process if file 1 was selected
                if (fileIndex == '1')
                {
                    int total = 0;
                    if(i == 0)
                    {
                        //takes the first fourth of the first file
                        for(int j = 0; j < sizeof(data1) / sizeof(data1[0]) / 4; j++)
                        {
                            total += data1[j];
                        }
                        
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 1)
                    {
                        //takes the second fourth of the first file
                        for(int j = 0; j < sizeof(data1) / sizeof(data1[0]) / 4; j++)
                        {
                            total += data1[j + 250];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 2)
                    {
                        //takes the third fourth of the first file
                        for(int j = 0; j < sizeof(data1) / sizeof(data1[0]) / 4; j++)
                        {
                            total += data1[j + 500];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 3)
                    {
                        //takes the final fourth of the first file
                        for(int j = 0; j < sizeof(data1) / sizeof(data1[0]) / 4; j++)
                        {
                            total += data1[j + 750];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                }
                //child process if file 2 was selected
                else if(fileIndex == '2')
                {
                    int total = 0;
                    if(i == 0)
                    {
                        //takes the first fourth of the second file
                        for(int j = 0; j < sizeof(data2) / sizeof(data2[0]) / 4; j++)
                        {
                            total += data2[j];
                        }
                        
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 1)
                    {
                        //takes the second fourth of the second file
                        for(int j = 0; j < sizeof(data2) / sizeof(data2[0]) / 4; j++)
                        {
                            total += data2[j + 2500];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 2)
                    {
                        //takes the third fourth of the second file
                        for(int j = 0; j < sizeof(data2) / sizeof(data2[0]) / 4; j++)
                        {
                            total += data2[j + 5000];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 3)
                    {
                        //takes the final fourth of the second file
                        for(int j = 0; j < sizeof(data2) / sizeof(data2[0]) / 4; j++)
                        {
                            total += data2[j + 7500];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                }
                //child process if file 3 was selected
                else if(fileIndex =='3')
                {
                    int total = 0;
                    if(i == 0)
                    {
                        //takes the first fourth of the third file
                        for(int j = 0; j < sizeof(data3) / sizeof(data3[0]) / 4; j++)
                        {
                            total += data3[j];
                        }
                        
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 1)
                    {
                        //takes the second fourth of the third file
                        for(int j = 0; j < sizeof(data3) / sizeof(data3[0]) / 4; j++)
                        {
                            total += data3[j + 25000];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 2)
                    {
                        //takes the third fourth of the third file
                        for(int j = 0; j < sizeof(data3) / sizeof(data3[0]) / 4; j++)
                        {
                            total += data3[j + 50000];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                    else if(i == 3)
                    {
                        //takes the final fourth of the third file
                        for(int j = 0; j < sizeof(data3) / sizeof(data3[0]) / 4; j++)
                        {
                            total += data3[j + 75000];
                        }
                    
                        write(fds[i][1], &total, sizeof(total));
                        exit(i);
                    }
                }
                
            }
            //creates parent process
            else 
            {          
                if (i == 0)
                {
                    start = chrono::steady_clock::now();
                }
                else if(i ==3)
                {
                    wait(NULL);
                    
                    read(fds[0][0], &total1, sizeof(int));
                    read(fds[1][0], &total2, sizeof(int));
                    read(fds[2][0], &total3, sizeof(int));
                    read(fds[3][0], &total4, sizeof(int));

                    end = chrono::steady_clock::now();

                    long result = total1 + total2 + total3 + total4;

                    cout << "child 1 results: " << total1 << endl;

                    cout << "child 2 results: " << total2 << endl;

                    cout << "child 3 results: " << total3 << endl;

                    cout << "child 4 results: " << total4 << endl;

                    cout << "Final result: " << result << endl;

                    cout << "Time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << endl;
                }
            }
        }  
    }
}