#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <conio.h>
#include <iomanip>
#include <thread>
#include <chrono>
using namespace std;

#define FILENAME "data/3k.txt"

vector<string> split(const string &s, char delim)
{
    vector<string> result;
    stringstream ss (s);
    string item;

    while(getline(ss, item, delim))
    {
        result.push_back(item);    
    }

    return result;
}

vector<string> initWords(string filename)
{
    string word;
    string wordstr;
    vector<string> words;

    ifstream WORDS(filename);

    while(getline(WORDS, word))
    {
        wordstr.append(word + " ");
    }

    words = split(wordstr, ' ');

    WORDS.close();

    return words;
}

string randomSentence(vector<string> &words)
{
    srand(time(0));

    string result;
    int wordsAmt = (rand() % 10) + 5;
    long int totalWords = words.size();

    for (int i = 0; i < wordsAmt; ++i)
    {
        result += words[(rand()%totalWords)];
        result += " ";
    }
    result.pop_back();

    return result;
}

double checkAcc(const string &sent, const string &usr)
{
    double correct = 0, total = 0;

    if (sent.length() <= usr.length())
    {
        total = sent.length();
        for (int i = 0; i < int(sent.length()); ++i)
        {
            if (sent[i] == usr[i])
            {
                correct += 1;
            }
        }
    }
    else if (sent.length() > usr.length())
    {
        total = sent.length();
        for (int i = 0; i < int(usr.length()); ++i)
        {
            correct += 1;
        }
    }

    return (correct / total) * 100;
}

void clearScreen()
{
    #ifdef _WIN32
        system("clear");
    #else
        system("cls");
    #endif
}

void displaySentence(const string &sentence)
{
    clearScreen();
    string screen(sentence.length(), '-');
    screen.insert(screen.begin(), '\n');    
    
    screen += "\n" + sentence + "\033[2A\r";
    printf("%s", screen.c_str());
}

void getUserInput(const string &sentence, string &usrsentence, string &actualusrsentence, int &count)
{
    while (usrsentence.length() < sentence.length())
    {
        char ch = _getch();

        if (ch == 13)
        {
            return;
        }
        if (ch == 27)
        {
            count = 0;
            usrsentence.clear();
            actualusrsentence.assign(sentence.length(), '*');
            displaySentence(sentence);
            continue;
        }
        if (ch != 8)
        {
            usrsentence += ch;
            count += 1;
        }
        if(count <= int(actualusrsentence.length()))
        {
            if (actualusrsentence[count-1] == '*') 
                actualusrsentence[count-1] = ch;
        }
        if (ch == 8 && usrsentence.length() > 0)
        {
            count -= 1;
            usrsentence.pop_back();
            printf("\b ");
        }

        if (sentence[count-1] == ch)
        {
            if (sentence[count-1] != actualusrsentence[count-1])
                printf("\e[0;36m%c\e[0m", ch);   
            else
                printf("%c", ch);
        }
        else if (sentence[count-1] != ch)
            printf("\e[0;31m%c\e[0m", ch);           
    }
}

void displayStats(double accuracy, double actualAccuracy, const string &timeStat, float WPMStat)
{
    cout << endl << endl << endl
    << fixed << setprecision(2) 
    << "Accuracy:             " << accuracy << " %" << endl 
    << "Actual Accuracy:      " << actualAccuracy << " %" << endl
    << "Time:                 " << timeStat << endl
    << fixed << setprecision (1)
    << "WPM:                  " << WPMStat << endl;
}

void countdown(int seconds)
{
    while (seconds >= 1)
    {
        printf("\033[H");
        cout << endl << endl << endl << "\033[2K" << "Time Remaining : " << seconds;
        
        this_thread::sleep_for(chrono::seconds(1));
        
        seconds--;
    }
    printf("\033[H");
    cout << endl << endl << endl << "\033[2K" << "GO!";
    printf("\033[H");
}

string CalculateTime(chrono::_V2::system_clock::time_point start, chrono::_V2::system_clock::time_point end) 
{
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    auto totalSeconds = chrono::duration_cast<chrono::seconds>(duration);
    auto millisecs = duration.count() % 1000;

    int mins = totalSeconds.count() / 60;
    int secs = totalSeconds.count() % 60;

    stringstream result;
    result << mins << ":" << setw(2) << setfill('0') << secs << "." << setw(3) << setfill('0') << millisecs;

    return result.str();
}

void resetValues(string &sentence, string &usrsentence, string &actualusrsentence, int &count, string &timeStat, double &accuracy, double &actualAccuracy, float &WPMStat, vector<string> &words)
{
    sentence = randomSentence(words);
    usrsentence = "";
    actualusrsentence.assign(sentence.length(), '*');
    count = 0;
    timeStat = "";
    accuracy = 0.0f;
    actualAccuracy = 0.0f;
    WPMStat = 0.0f;
}

string getPresentTime()
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* local_tm = localtime(&now_c);

    stringstream ss;
    ss << setw(2) << setfill('0') << local_tm->tm_mday << "."
       << setw(2) << setfill('0') << (local_tm->tm_mon + 1) << "."
       << (local_tm->tm_year + 1900) << "-"
       << setw(2) << setfill('0') << local_tm->tm_hour << ":"
       << setw(2) << setfill('0') << local_tm->tm_min << ":"
       << setw(2) << setfill('0') << local_tm->tm_sec;

    return ss.str();
}

void saveStats(double &accuracy, double &actualAccuracy, const string &timeStat, float &WPMStat)
{
    string line;
    ofstream STATS;
    STATS.open("stats.json", ios::app);

    string date = getPresentTime();
    // if (getline(STATS, line))
    STATS << "{" << endl
    << "\t\"" << date << "\": {" << "," << endl
    << "\t\t\"accuracy\":" << "\"" << fixed << setprecision(2) << accuracy << "\"" << endl
    << "\t\t\"actualAccuracy\":" << " \"" << actualAccuracy << "\"" << "," << endl
    << "\t\t\"time\":" << "\"" << timeStat << " \"" << "," << endl
    << "\t\t\"WPM\":" << "\"" << setprecision(1) <<  WPMStat << " \"" << "," << endl
    << "\t}," << endl
    << "}" << endl;
    STATS.close();
}

void gameLoop(string &sentence, string &usrsentence, string &actualusrsentence, int &count, string &timeStat, double &accuracy, double &actualAccuracy, float &WPMStat, bool &running, short &save, vector<string> &words, int &delay)
{
    if (delay == 0)
    {
        printf("Welcome to TermType! Input a number (1-...) of seconds for a countdown...    ");
        cin >> delay;
    }

    displaySentence(sentence);

    countdown(delay);
    auto startTime = chrono::high_resolution_clock::now();

    getUserInput(sentence, usrsentence, actualusrsentence, count);

    auto endTime = chrono::high_resolution_clock::now();
    timeStat = CalculateTime(startTime, endTime);

    accuracy = checkAcc(sentence, usrsentence);
    actualAccuracy = checkAcc(sentence, actualusrsentence);

    float mins = chrono::duration_cast<chrono::duration<float, ratio<60>>>(endTime - startTime).count();
    if (mins != 0 && usrsentence.length() > sentence.length()/5 && mins >= 0.016f)
        WPMStat = (float(sentence.length()) / 5.0f) / chrono::duration_cast<chrono::duration<float, ratio<60>>>(endTime - startTime).count();
    else
        WPMStat = 0;

    displayStats(accuracy, actualAccuracy, timeStat, WPMStat);

    if (save != 2)
    {
        printf("\nSave statistics to file (2 - Always | 1 - Yes | 0 - No)?");
        cin >> save;
    }
    if (save > 0)
    {
        saveStats(accuracy, actualAccuracy, timeStat, WPMStat); 
    }
    char ch = getch();
    if (ch == '0' || ch == 27)
        running = 0;

    resetValues(sentence, usrsentence, actualusrsentence, count, timeStat, accuracy, actualAccuracy, WPMStat, words);
}

int main()
{
    vector<string> words = initWords(FILENAME);
    string sentence = randomSentence(words);
    string usrsentence, actualusrsentence;
    string timeStat;
    int count = 0;
    int delay = 0;
    float WPMStat = 0.0f;
    double accuracy = 0.0f;
    double actualAccuracy = 0.0f;

    bool running = 1;
    short save = 0;

    actualusrsentence = string(sentence.length(), '*');

    while (running)
    {
        gameLoop(sentence, usrsentence, actualusrsentence, count, timeStat, accuracy, actualAccuracy, WPMStat, running, save, words, delay);
    }

    return 0;
}