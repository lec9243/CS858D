#include <iostream>
#include <fstream>

// using std::cout; using std::ofstream;
// using std::endl; using std::string;
using namespace std;

int main()
{
    string filename("tmp.txt");
    ofstream file_out;

    file_out.open(filename, ios_base::app);
    file_out << "Some random text to append." << endl;
    cout << "Done !" << endl;

    file_out.close();

    ofstream fileagain;
    fileagain.open(filename, ios_base::app);
    fileagain << "ADD MORE!" << endl;
    fileagain << "AGAIN!" << endl;
    fileagain.close();

    string try_str = "abcdef";
    cout << try_str.substr(3, 8);
    cout << "" << endl;
    cout << try_str.substr(5, 8) << "!" << endl;
    cout << try_str.substr(6, 8) << "!" << endl;
    cout << try_str.substr(7, 8) << "!" << endl;

    string try_str2 = "abcd";
    cout << min(try_str.size(), try_str2.size());

    return EXIT_SUCCESS;
}
