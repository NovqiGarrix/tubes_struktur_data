#include <iostream>
#include <string>
#include <list>
#include <stack>

using namespace std;

int main()
{
    list<string> playlist; // Doubly linked list for playlist
    stack<string> history; // Stack for recently played

    list<string>::iterator current_song = playlist.end(); // Initially, no current song

    cout << "=============================" << endl;
    cout << "       Music Playlist        " << endl;
    cout << "=============================" << endl;
    cout << "Commands:" << endl;
    cout << "  add <no. of song>     - Add a song to the playlist" << endl;
    cout << "  play           - Play the current song" << endl;
    cout << "  next           - Go to the next song" << endl;
    cout << "  previous       - Go to the previous song" << endl;
    cout << "  list           - List all songs in the playlist" << endl;
    cout << "  history        - Show recently played songs" << endl;
    cout << "  quit           - Exit the program" << endl;
    cout << "=============================" << endl;

    string command;
    while (true)
    {
        cout << "Pilih menu: ";
        cin >> command;

        if (command == "add")
        {
            // ... (Get song title input and add it to playlist) ...
        }
        else if (command == "play")
        {
            if (current_song != playlist.end())
            {
                cout << "Playing: " << *current_song << endl;
                history.push(*current_song); // Add to history
            }
            else
            {
                cout << "Playlist is empty!" << endl;
            }
        }
        else if (command == "next")
        {
            // ... (Move current_song to next. Handle wrapping around if at end.)...
        }
        else if (command == "previous")
        {
            // ... (Move current_song to previous. Handle wrapping around if at beginning.)...
        }
        else if (command == "list")
        {
            // ... (Iterate and print the playlist)...
        }
        else if (command == "history")
        {
            // ... (Print the history stack.  Be careful not to modify the stack) ...
        }
        else if (command == "quit")
        {
            break; // Exit the loop
        }
        else
        {
            cout << "Invalid command." << endl;
        }
    }

    return 0;
}