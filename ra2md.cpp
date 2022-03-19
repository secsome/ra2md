#include <Always.h>

#include <Conquer.h>
#include <Init.h>
#include <Uninit.h>

HINSTANCE hInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    ::hInstance = hInstance;
    Init_Global();

    if (Init_Parse_Command_Line(__argc, __argv))
    {
        Log_Begin();
        Init_Random();
        Init_Keys();
        Init_MixFiles();
        Init_Language();

        Uninit_MixFiles();
        Log_Close();
    }

    return 0;
}
