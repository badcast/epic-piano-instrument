#include <exception>
#include <iostream>

#include "PianoWorld.h"

using namespace std;
#if WIN32
int WinMain(void *, void *, void *, int)
#else
int main()
#endif
{
    RoninSimulator::Init();

    PianoWorld world;
    std::string keyName;

    keyName = Input::GetKeyName(KeyboardCode::KB_UNKNOWN+1);
    if(keyName != "None")
        cout << ("Test 1 fail") << endl;

    keyName = Input::GetKeyName(KeyboardCode::KB_KP_DIVIDE);
    if(keyName != "Divide")
        cout << ("Test 2 fail") << endl;

    keyName = Input::GetKeyName(KeyboardCode::KB_KP_PERIOD);
    if(keyName != "Period")
        cout << ("Test 3 fail") << endl;

    keyName = Input::GetKeyName((KeyboardCode::KB_LCTRL - 1));
    if(keyName != "None")
        cout << ("Test 4 fail") << endl;

    keyName = Input::GetKeyName(KeyboardCode::KB_LCTRL);
    if(keyName != "LCtrl")
        cout << ("Test 5 fail") << endl;

    keyName = Input::GetKeyName(KeyboardCode::KB_RGUI);
    if(keyName != "RGUI")
        cout << ("Test 6 fail") << endl;

    Resolution resolution {1024, 768};
    RoninSimulator::Show(resolution, false);

    RoninSimulator::SetDebugMode(true);

    if(RoninSimulator::LoadWorld(&world) == true)
    {
        RoninSimulator::Simulate();
    }

    RoninSimulator::Finalize();

    return 0;
}
