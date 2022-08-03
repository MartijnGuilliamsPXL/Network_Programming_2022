#include <iostream>
#include "service.h"

using namespace std;

int main()
{
    int choice;
    while(1){
        cout << "choose service to use: 0 = battleship" << endl;
        cin >> choice;
        switch (choice) {
        case 0:
            {
            //start battleship game
            cout << "starting battleship" << endl;
            Service* battleship_obj = new Service();
            battleship_obj->run();
            while(1);
            delete battleship_obj;
            break;
            }
        default:
            cout << "error try again" << endl;
        }
    }

    cout << "Hello World!" << endl;
    return 0;
}
