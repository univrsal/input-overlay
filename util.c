#include "util.h"

/**
 * Not the smartest way but I didn't
 * want to bother with the windows
 * API anymore
 */
const char * key_to_text(int key_code)
{
    switch (key_code)
    {
    case VK_NUMPAD0:
        return "NUMPAD 0";
    case VK_NUMPAD1:
        return "NUMPAD 1";
    case VK_NUMPAD2:
        return "NUMPAD 2";
    case VK_NUMPAD3:
        return "NUMPAD 3";
    case VK_NUMPAD4:
        return "NUMPAD 4";
    case VK_NUMPAD5:
        return "NUMPAD 5";
    case VK_NUMPAD6:
        return "NUMPAD 6";
    case VK_NUMPAD7:
        return "NUMPAD 7";
    case VK_NUMPAD8:
        return "NUMPAD 8";
    case VK_NUMPAD9:
        return "NUMPAD 9";
    case VK_NUMLOCK:
        return "NUM LOCK";
    case VK_F1:
        return "F1";
    case VK_F2:
        return "F2";
    case VK_F3:
        return "F3";
    case VK_F4:
        return "F4";
    case VK_F5:
        return "F5";
    case VK_F6:
        return "F6";
    case VK_F7:
        return "F7";
    case VK_F8:
        return "F8";
    case VK_F9:
        return "F9";
    case VK_F10:
        return "F10";
    case VK_F11:
        return "F11";
    case VK_F12:
        return "F12";
    case VK_F13:
        return "F13";
    case VK_F14:
        return "F14";
    case VK_F15:
        return "F15";
    case VK_F16:
        return "F16";
    case VK_F17:
        return "F17";
    case VK_F18:
        return "F18";
    case VK_F19:
        return "F19";
    case VK_F20:
        return "F20";
    case VK_F21:
        return "F21";
    case VK_F22:
        return "F22";
    case VK_F23:
        return "F23";
    case VK_F24:
        return "F24";
    case 'A':
        return "A";
    case 'B':
        return "B";
    case 'C':
        return "C";
    case 'D':
        return "D";
    case 'E':
        return "E";
    case 'F':
        return "F";
    case 'G':
        return "G";
    case 'H':
        return "H";
    case 'I':
        return "I";
    case 'J':
        return "J";
    case 'K':
        return "K";
    case 'L':
        return "L";
    case 'M':
        return "M";
    case 'N':
        return "N";
    case 'O':
        return "O";
    case 'P':
        return "P";
    case 'Q':
        return "Q";
    case 'R':
        return "R";
    case 'S':
        return "S";
    case 'T':
        return "T";
    case 'U':
        return "U";
    case 'V':
        return "V";
    case 'W':
        return "W";
    case 'X':
        return "X";
    case 'Y':
        return "Y";
    case 'Z':
        return "Z";
    case '0':
        return "0";
    case '1':
        return "1";
    case '2':
        return "2";
    case '3':
        return "3";
    case '4':
        return "4";
    case '5':
        return "5";
    case '6':
        return "6";
    case '7':
        return "7";
    case '8':
        return "8";
    case '9':
        return "9";
    case VK_SPACE:
        return "SPACE";
    case VK_TAB:
        return "TAB";
    case VK_BACK:
        return "BACKSPACE";
    case VK_ESCAPE:
        return "ESC";
    case VK_INSERT:
        return "INSERT";
    case VK_HOME:
        return "HOME";
    case VK_END:
        return "END";
    case VK_DELETE:
        return "DELETE";
    case VK_UP:
        return "UP";
    case VK_DOWN:
        return "DOWN";
    case VK_LEFT:
        return "LEFT";
    case VK_RIGHT:
        return "RIGHT";
    case VK_PRINT:
        return "PRINT";
    case VK_SCROLL:
        return "SCROLL LOCK";
    case VK_PAUSE:
        return "PAUSE";
    default:
        return NULL;
        break;
    }
}

char * append(char * a, char * b)
{
    char* out;
    if (a == NULL || b == NULL)
        return NULL;
    if ((out = (char *)malloc(strlen(a) + strlen(b) + 1)) != NULL)
    {
        strcpy(out, a);
        strcat(out, b);
    }

    a = NULL;
    b = NULL;
    return out;
}