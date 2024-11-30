#include <iostream>
#include <string>
#include <list>
#include <stack>
#include <filesystem>
#include <SFML/Audio.hpp>
#include <thread>
#include <atomic>

namespace fs = std::filesystem;
using namespace std;

void printMusicName(const string &musicPath, const bool withSymbol)
{
    if (withSymbol)
    {
        cout << "• ";
    }

    cout << fs::path(musicPath).stem().string() << endl;
}

// Print all files in the list
void printFiles(const list<string> &files)
{
    for (const auto &file : files)
    {
        printMusicName(file, true);
    }
}

void printFilesFromStack(const stack<string> &files)
{
    stack<string> temp = files;
    if (temp.empty())
    {
        cout << "History is empty!" << endl;
        return;
    }

    while (!temp.empty())
    {
        printMusicName(temp.top(), true);
        temp.pop();
    }

    cout << endl;
}

// Get all mp3 files in the directory
vector<string> readMp3Files(const string &directoryPath)
{
    vector<string> mp3Files;

    try
    {
        for (const auto &entry : fs::directory_iterator(directoryPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".mp3")
            {
                mp3Files.emplace_back(entry.path().string());
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        cerr << "Error accessing directory: " << e.what() << std::endl;
    }

    return mp3Files;
}

void playAudioFile(const string &audioFilePath, atomic<bool> &stopFlag)
{
    static sf::Sound sound; // Make sound static to keep it in scope
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(audioFilePath))
    {
        std::cerr << "Error loading audio file" << std::endl;
        return;
    }
    sound.setBuffer(buffer);
    sound.play();

    // Wait until the sound is finished playing or stopFlag is set
    while (sound.getStatus() == sf::Sound::Playing && !stopFlag)
    {
        sf::sleep(sf::milliseconds(100));
    }
    sound.stop();
}

void stopAudio(atomic<bool> &stopFlag)
{
    stopFlag = true;
}

int main()
{
    vector<string> mp3Files = readMp3Files("musics");

    list<string> playlist; // Doubly linked list for playlist
    stack<string> history; // Stack for recently played

    for (size_t i = 0; i < mp3Files.size(); i++)
    {
        playlist.push_back(mp3Files[i]);
    }

    list<string>::iterator current_song = playlist.end(); // Initially, no current song

    cout << "=============================" << endl;
    cout << "       Music Playlist        " << endl;
    cout << "=============================" << endl;

    cout << "Available songs:" << endl;
    printFiles(playlist);

    cout << "Commands:" << endl;
    cout << "  play           - Play the current song" << endl;
    cout << "  next           - Go to the next song" << endl;
    cout << "  previous       - Go to the previous song" << endl;
    cout << "  list           - List all songs in the playlist" << endl;
    cout << "  history        - Show recently played songs" << endl;
    cout << "  quit           - Exit the program" << endl;
    cout << "=============================" << endl;

    string command;
    atomic<bool> stopFlag(false);
    thread audioThread;

    while (true)
    {
        cout << "Pilih menu: ";
        cin >> command;

        if (command == "play")
        {
            if (current_song == playlist.end())
            {
                // Auto select the first song if no song is selected
                current_song = playlist.begin();
            }

            cout << "Playing: ";
            printMusicName(*current_song, false);
            stopFlag = false;
            if (audioThread.joinable())
            {
                audioThread.join();
            }
            audioThread = thread(playAudioFile, *current_song, ref(stopFlag));
            history.push(*current_song); // Add to history
        }
        else if (command == "next")
        {
            if (current_song != playlist.end())
            {
                stopAudio(stopFlag); // Stop the current song
                if (audioThread.joinable())
                {
                    audioThread.join();
                }
                ++current_song;
                if (current_song == playlist.end())
                {
                    current_song = playlist.begin(); // Wrap around to the beginning
                }
                cout << "Next song: ";
                printMusicName(*current_song, false);
                stopFlag = false;
                audioThread = thread(playAudioFile, *current_song, ref(stopFlag));
                history.push(*current_song); // Add to history
            }
            else
            {
                cout << "No song is currently selected." << endl;
            }
        }
        else if (command == "previous")
        {
            // ... (Move current_song to previous. Handle wrapping around if at beginning.)...
        }
        else if (command == "list")
        {
            cout << "Available songs:" << endl;
            printFiles(playlist);
        }
        else if (command == "history")
        {
            printFilesFromStack(history);
        }
        else if (command == "quit")
        {
            stopAudio(stopFlag); // Stop the current song
            if (audioThread.joinable())
            {
                audioThread.join();
            }
            break; // Exit the loop
        }
        else
        {
            cout << "Invalid command." << endl;
        }
    }

    return 0;
}