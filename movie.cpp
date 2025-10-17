#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

// -------------------- Movie Class --------------------
class Movie {
public:
    string title, duration, genre, language;
    vector<string> showTimes;
    int rows = 5, cols = 8;
    vector<vector<char>> seats;

    Movie() { seats.resize(rows, vector<char>(cols, 'O')); }

    Movie(string t, string d, string g, string l, vector<string> st) {
        title = t; duration = d; genre = g; language = l; showTimes = st;
        seats.resize(rows, vector<char>(cols, 'O'));
    }

    void displaySeats() {
        cout << "\n=============== SCREEN =================\n\n";
        cout << "        ";
        for (int c = 0; c < cols; c++)
            cout << setw(3) << c + 1 << " ";
        cout << "\n";

        for (int i = 0; i < rows; i++) {
            cout << "   " << char('A' + i) << "    ";
            for (int j = 0; j < cols; j++)
                cout << setw(3) << seats[i][j] << " ";
            cout << endl;
        }

        cout << "\nSeats: O = Available, X = Booked\n";
        cout << "Pricing: A row = Platinum (Rs.200), B row = Gold (Rs.150), Others = Silver (Rs.120)\n";
    }

    bool bookSeat(char row, int col) {
        int r = row - 'A', c = col - 1;
        if (r >= 0 && r < rows && c >= 0 && c < cols && seats[r][c] == 'O') {
            seats[r][c] = 'X';
            return true;
        }
        return false;
    }

    int seatPrice(char row) {
        if (row == 'A') return 200;
        if (row == 'B') return 150;
        return 120;
    }

    void resetSeats() {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                seats[i][j] = 'O';
    }
};

// -------------------- Payment Class --------------------
class Payment {
public:
    double amount;
    string mode;

    void makePayment(double amt) {
        amount = amt;
        int choice;
        cout << "\nSelect payment mode:\n1. UPI\n2. Card\n3. Cash\nEnter choice: ";
        cin >> choice;
        switch (choice) {
        case 1: mode = "UPI"; break;
        case 2: mode = "Card"; break;
        case 3: mode = "Cash"; break;
        default: mode = "Unknown"; break;
        }
        cout << "Payment of Rs." << amount << " successful via " << mode << "!\n";
    }
};

// -------------------- Booking Class --------------------
class Booking {
public:
    Movie* movie;
    vector<string> bookedSeats;
    string showTime;
    Payment payment;

    void book(Movie* m, string selectedTime) {
        movie = m;

        // Show current seat layout at the start of booking
        cout << "\nCurrent seat layout for \"" << movie->title << "\" at " << selectedTime << ":\n";
        movie->displaySeats();  // shows previously booked seats as 'X'

        int n;
        cout << "How many seats do you want to book? "; 
        cin >> n;
        if(cin.fail() || n < 1) {
            cin.clear(); cin.ignore(1000,'\n');
            cout << "Invalid number of seats.\n";
            return;
        }
        cin.ignore();

        cout << "Enter seat numbers separated by spaces (e.g., A1 A2 A3): ";
        string line; getline(cin, line);
        stringstream ss(line);
        string seat;
        double total = 0;
        vector<string> finalSeats;

        int count = 0;
        while (ss >> seat && count < n) {
            if (seat.length() < 2) { 
                cout << seat << " is invalid. Skipping.\n"; 
                continue; 
            }
            char row = toupper(seat[0]);
            int col = 0;
            try { col = stoi(seat.substr(1)); } 
            catch (...) { cout << seat << " is invalid. Skipping.\n"; continue; }

            if (row < 'A' || row >= 'A' + movie->rows || col < 1 || col > movie->cols) { 
                cout << seat << " is out of range. Skipping.\n"; 
                continue; 
            }
            if (!movie->bookSeat(row, col)) { 
                cout << seat << " is already booked. Skipping.\n"; 
                continue; 
            }

            finalSeats.push_back(seat);
            total += movie->seatPrice(row);
            count++;
        }

        if(finalSeats.empty()) {
            cout << "No valid seats booked. Cancelling.\n";
            return;
        }

        // Confirm booking
        cout << "\nYou are booking seats: ";
        for(auto &s : finalSeats) cout << s << " ";
        cout << "\nTotal amount: Rs." << total << "\nProceed to payment? (y/n): ";
        char confirm; cin >> confirm;
        if(tolower(confirm) != 'y') {
            cout << "Booking cancelled.\n";
            // Revert seats
            for(auto &s : finalSeats) {
                char r = s[0]; int c = stoi(s.substr(1));
                movie->seats[r-'A'][c-1] = 'O';
            }
            return;
        }

        payment.makePayment(total);

        ofstream fout("bookings.txt", ios::app);
        fout << movie->title << "," << selectedTime << ",";
        for (auto& s : finalSeats) fout << s << " ";
        fout << "," << payment.amount << "," << payment.mode << "\n";
        fout.close();

        cout << "\nBooking Confirmed!\n";
        // movie->displaySeats(); // removed as requested
         
        cout << "====== Booking Summary ======\n";
        cout << "Movie: " << movie->title << "\n";
        cout << "Show Time: " << selectedTime << "\n";
        cout << "Seats: ";
        for(auto &s : finalSeats) cout << s << " ";
        cout << "\nTotal Paid: Rs." << payment.amount << "\n";
        cout << "Payment Mode: " << payment.mode << "\n";
       
    }
};

// -------------------- Load Movies from txt --------------------
void loadMovies(vector<Movie>& movies) {
    ifstream fin("movies.txt");
    string line;
    while (getline(fin, line)) {
        stringstream ss(line);
        string t, d, g, l, times;
        getline(ss, t, ','); getline(ss, d, ','); getline(ss, g, ','); getline(ss, l, ','); getline(ss, times, ',');

        vector<string> showTimes;
        stringstream ts(times);
        string st;
        while (getline(ts, st, '|')) showTimes.push_back(st);

        movies.push_back(Movie(t, d, g, l, showTimes));
    }
    fin.close();
}

// -------------------- Main --------------------
int main() {
    vector<Movie> movies;
    loadMovies(movies);

    Booking booking;
    int choice;

   cout << "=====================================\n";
   cout << "        WELCOME TO MOVIEMATE         \n";
   cout << "   Your friendly cinema companion!   \n";
   cout << "=====================================\n";

    do {
        cout << "\n====== Main Menu ======\n1. Show Movies\n2. Book Tickets\n3. Exit\nEnter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            cout << "\nAvailable Movies:\n";
            for (int i = 0; i < movies.size(); i++)
                cout << i + 1 << ". " << movies[i].title << " (" << movies[i].genre << ", " 
                     << movies[i].duration << ", " << movies[i].language << ")\n";
            break;
        }
        case 2: {
            cout << "\nSelect Movie (1-" << movies.size() << "): "; int m; cin >> m;
            if (m < 1 || m > movies.size()) { cout << "Invalid selection.\n"; break; }
            cin.ignore();

            cout << "\nAvailable Show Timings for " << movies[m - 1].title << ":\n    ";
            for (int i = 0; i < movies[m - 1].showTimes.size(); i++) {
            cout << i + 1 << ". " << movies[m - 1].showTimes[i];
            if (i != movies[m - 1].showTimes.size() - 1) cout << " | ";}
            cout << endl;

            cout << "Select showtime (1-" << movies[m - 1].showTimes.size() << "): ";
            int t; cin >> t; if (t < 1 || t > movies[m - 1].showTimes.size()) { cout << "Invalid timing.\n"; break; }

            booking.book(&movies[m - 1], movies[m - 1].showTimes[t - 1]);
            break;
        }
        case 3: cout << "Exiting system. Thank you!\n"; break;
        default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);

    return 0;
}
