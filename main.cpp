#include <iostream>
#include <string>
#include <list>
#include <stack>
#include <filesystem>
#include <SFML/Audio.hpp>
#include <thread>
#include <atomic>
#include <condition_variable>

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

// karena pakai thread, jadi perlu condition variable
// dan mutex untuk sinkronisasi antara main thread dengan
// thread lain yang sedang memainkan audio
std::condition_variable cv;
std::mutex cv_m;
bool songEnded = false;

// Variable sound nya dideklarasi di luar fungsi
// supaya bisa digunakan di fungsi lain
sf::Sound sound;

void playAudioFile(const string &audioFilePath, atomic<bool> &stopFlag)
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(audioFilePath))
    {
        std::cerr << "Error loading audio file" << std::endl;
        return;
    }
    sound.setBuffer(buffer);
    sound.play();

    // Wait until the sound is finished playing or stopFlag is set
    while ((sound.getStatus() == sf::Sound::Playing || sound.getStatus() == sf::Sound::Paused) && !stopFlag)
    {
        sf::sleep(sf::milliseconds(100));
    }

    // Only stop and notify if not paused
    if (!stopFlag && sound.getStatus() != sf::Sound::Paused)
    {
        sound.stop();

        {
            lock_guard<mutex> lock(cv_m);
            songEnded = true;
        }
        cv.notify_one();
    }
}

void stopAudio(atomic<bool> &stopFlag)
{
    stopFlag = true;
}

void playNext(list<string>::iterator &current_song, list<string> &playlist, atomic_bool &stopFlag, thread &audioThread, stack<string> &history)
{
    if (current_song != playlist.end())
    {
        stopAudio(stopFlag);
        if (audioThread.joinable())
        {
            audioThread.join();
        }
        ++current_song;
        // Kalau misalnya setelah di-increment,
        // current_song sudah di akhir playlist
        // maka kembalikan current song ke awal
        if (current_song == playlist.end())
        {
            current_song = playlist.begin();
        }
        cout << "Lagu berikutnya: ";
        printMusicName(*current_song, false);
        cout << endl;
        stopFlag = false;
        audioThread = thread(playAudioFile, *current_song, ref(stopFlag));
        history.push(*current_song); // Tambahkan ke history
    }
    else
    {
        cout << "No song is currently selected." << endl;
    }
}

int main()
{
    vector<string> mp3Files = readMp3Files("musics");

    // Pakai list (sifatnya hampir sama dengan doubly linked list)
    list<string> playlist;
    // Pakai stack untuk menyimpan history lagu
    stack<string> history;

    for (size_t i = 0; i < mp3Files.size(); i++)
    {
        playlist.push_back(mp3Files[i]);
    }

    // Inisialisasi variable current_song
    // untuk menunjuk ke lagu yang sedang diputar
    // tapi di awal, belum ada lagu yang diputar
    list<string>::iterator current_song = playlist.end();

    cout << "=============================" << endl;
    cout << "       Music Playlist        " << endl;
    cout << "=============================" << endl;

    cout << "Menu:" << endl;
    cout << "  1. play           - Memutar lagu" << endl;
    cout << "  2. pause          - Pause lagu" << endl;
    cout << "  3. next           - Lagu berikutnya" << endl;
    cout << "  4. previous       - Lagu sebelumnya" << endl;
    cout << "  5. list           - Daftar semua lagu" << endl;
    cout << "  6. history        - Tampilkan lagu yang baru diputar" << endl;
    cout << "  7. quit           - Keluar dari program" << endl;
    cout << "=============================" << endl;

    string command;
    atomic<bool> stopFlag(false);
    std::atomic<bool> exitFlag(false);
    thread audioThread;

    // Thread khusus untuk memonitor apakah lagu sudah selesai
    // atau belum
    // Butuh thread ini, karena supaya main thread (yang menampilkan menu)
    // tidak diganggu
    thread monitorThread([&]()
                         {
        std::unique_lock<std::mutex> lock(cv_m);
        while (!exitFlag)
        {
            cv.wait(lock, [&]{ return songEnded || exitFlag; });
            if (exitFlag) break;
            songEnded = false;
            playNext(current_song, playlist, stopFlag, audioThread, history);
        } });

    while (true)
    {
        cout << "Pilih menu: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            if (sound.getStatus() == sf::Sound::Paused)
            {
                sound.play();
                break;
            }
            else if (sound.getStatus() == sf::Sound::Playing)
            {
                cout << "Lagu ini sedang diputar." << endl;
                break;
            }

            if (current_song == playlist.end())
            {
                // Auto select the first song if no song is selected
                current_song = playlist.begin();
            }

            cout << "Memainkan: ";
            printMusicName(*current_song, false);
            cout << endl;
            stopFlag = false;
            if (audioThread.joinable())
            {
                audioThread.join();
            }
            audioThread = thread(playAudioFile, *current_song, ref(stopFlag));
            history.push(*current_song); // Add to history
            break;
        case 2:
            if (sound.getStatus() == sf::Sound::Playing)
            {
                sound.pause();
            }
            else if (sound.getStatus() == sf::Sound::Paused)
            {
                sound.play();
            }
            break;
        case 3:
            playNext(current_song, playlist, stopFlag, audioThread, history);
            break;
        case 4:
            if (current_song != playlist.begin())
            {
                stopAudio(stopFlag);
                if (audioThread.joinable())
                {
                    audioThread.join();
                }
                --current_song;
                cout << "Lagu sebelumnya: ";
                printMusicName(*current_song, false);
                cout << endl;
                stopFlag = false;
                audioThread = thread(playAudioFile, *current_song, ref(stopFlag));
                history.push(*current_song);
            }
            else
            {
                cout << "Belum ada lagi sebelumnya." << endl;
            }
            break;
        case 5:
            cout << "Pilihan lagu:" << endl;
            printFiles(playlist);
            break;
        case 6:
            printFilesFromStack(history);
            break;
        case 7:
            exitFlag = true;
            cv.notify_one();
            stopAudio(stopFlag);
            if (audioThread.joinable())
            {
                audioThread.join();
            }
            break;
        default:
            cout << "No menus." << endl;
        }

        if (choice == 7)
        {
            break;
        }
    }

    // Join the monitor thread before exiting
    if (monitorThread.joinable())
    {
        monitorThread.join();
    }

    return 0;
}