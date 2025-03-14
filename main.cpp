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

// Add the new function to reload the playlist
void reloadPlaylist(list<string> &playlist, vector<string> &mp3Files, list<string>::iterator &current_song,
                    atomic<bool> &stopFlag, thread &audioThread, stack<string> &history,
                    sf::Sound &sound, std::condition_variable &cv, std::mutex &cv_m, bool &songEnded)
{
    // Save current song name if one is playing
    string current_song_name;
    bool was_playing = false;
    if (current_song != playlist.end())
    {
        current_song_name = *current_song;
        was_playing = (sound.getStatus() == sf::Sound::Playing);
    }

    // Stop any playing audio before modifying playlist
    if (sound.getStatus() == sf::Sound::Playing)
    {
        stopFlag = true;
        if (audioThread.joinable())
        {
            audioThread.join();
        }
    }

    // Reload playlist
    mp3Files = readMp3Files("musics");
    playlist.clear();
    for (const auto &file : mp3Files)
    {
        playlist.push_back(file);
    }

    // Restore current song position if possible
    if (!playlist.empty())
    {
        if (!current_song_name.empty())
        {
            // Try to find the previously playing song
            auto it = find(playlist.begin(), playlist.end(), current_song_name);
            current_song = (it != playlist.end()) ? it : playlist.begin();
        }
        else
        {
            // If no song was playing, point to the beginning
            current_song = playlist.begin();
        }

        // Resume playback if it was playing before
        if (was_playing)
        {
            stopFlag = false;
            audioThread = thread(playAudioFile, *current_song, ref(stopFlag));
            history.push(*current_song);
        }
    }
    else
    {
        current_song = playlist.end();
    }

    cout << "Playlist reloaded." << endl;
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
    cout << "  7. download       - Download lagu dari youtube" << endl;
    cout << "  8. quit           - Keluar dari program" << endl;
    cout << "  9. reload         - Reload playlist" << endl;
    cout << "=============================" << endl;

    atomic<bool> stopFlag(false);
    atomic<bool> exitFlag(false);
    // bool exitFlag = false;
    thread audioThread;
    thread downloadThread;
    // Thread khusus untuk memonitor apakah lagu sudah selesai
    // atau belum
    // Butuh thread ini, supaya main thread (yang menampilkan menu)
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
        string download_url;

        int choice;
        cin >> choice;
        if (cin.fail())
        {
            cin.clear();                                                   // Clear the error flags
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input. Hanya menerima angka sesuai dengan nomor menu." << endl;
            continue;
        }

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
                cout << "Lagu di-pause." << endl;
            }
            else if (sound.getStatus() == sf::Sound::Paused)
            {
                sound.play();
                cout << "Lagi diputar kembali." << endl;
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
            // mp3Files = readMp3Files("musics");
            // playlist.clear();
            // for (const auto &file : mp3Files)
            // {
            //     playlist.push_back(file);
            // }
            printFiles(playlist);
            break;
        case 6:
            printFilesFromStack(history);
            break;
        case 7:
        {
            cout << "Masukkan link youtube: ";
            cin >> download_url;

            cout << "Downloading... Please wait" << endl;

            bool downloadComplete = false; // Add downloadComplete flag

            {
                int downloadStatus = 0;

                // Start download in a separate thread
                thread downloadThread([&]()
                                      {
                                          downloadStatus = system(("yt-dlp -x --audio-format mp3 --audio-quality 0 -o \"musics/%(title)s.%(ext)s\" " + download_url).c_str());
                                          downloadComplete = true; // Set flag when done
                                      });

                // Display loading animation while downloading
                while (!downloadComplete)
                {
                    cout << "\rDownloading... /" << flush;
                    this_thread::sleep_for(chrono::milliseconds(250));
                    cout << "\rDownloading... -" << flush;
                    this_thread::sleep_for(chrono::milliseconds(250));
                    cout << "\rDownloading... \\" << flush;
                    this_thread::sleep_for(chrono::milliseconds(250));
                    cout << "\rDownloading... |" << flush;
                    this_thread::sleep_for(chrono::milliseconds(250));
                }
                cout << "\r" << string(20, ' ') << "\r" << flush; // Clear loading animation

                downloadThread.join();
            }
            break;
        }
        case 8:
            exitFlag = true;
            cv.notify_one();
            stopAudio(stopFlag);
            if (audioThread.joinable())
            {
                audioThread.join();
            }

            cout << "Exiting..." << endl;

            // Add break to exit the while loop
            break;
        case 9:
            reloadPlaylist(playlist, mp3Files, current_song, stopFlag, audioThread, history, sound, cv, cv_m, songEnded);
            break;

        default:
            cout << "Tidak ada menu." << endl;
            break;
        }

        // Add check after switch to break from while loop
        if (exitFlag)
            break;
    }

    // Join the monitor thread before exiting
    if (monitorThread.joinable())
    {
        monitorThread.join();
    }

    return 0;
}