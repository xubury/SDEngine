#ifndef SD_KEYCDOE_HPP
#define SD_KEYCDOE_HPP

#include "Core/Export.hpp"
#include "Utility/Base.hpp"
#include <cstdint>

namespace SD {

namespace Keymod {

enum {
    None = 0x0000,
    LShift = 0x0001,
    RShift = 0x0002,
    LCtrl = 0x0040,
    RCtrl = 0x0080,
    LAlt = 0x0100,
    RAlt = 0x0200,
    LGui = 0x0400,
    RGui = 0x0800,
    Num = 0x1000,
    Caps = 0x2000,
    Mode = 0x4000,
    Reserved = 0x8000,

    Ctrl = LCtrl | RCtrl,
    Shift = LShift | RShift,
    Alt = LAlt | RAlt,
    Gui = LGui | RGui
};

}

#define SCANCODE_MASK (1 << 30)
#define SCANCODE_TO_KEYCODE(X) (static_cast<uint16_t>(X) | SCANCODE_MASK)

enum class Scancode : uint16_t {

    UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,

    D1 = 30,
    D2 = 31,
    D3 = 32,
    D4 = 33,
    D5 = 34,
    D6 = 35,
    D7 = 36,
    D8 = 37,
    D9 = 38,
    D0 = 39,

    Return = 40,
    Escape = 41,
    Backspace = 42,
    Tab = 43,
    Space = 44,

    Minus = 45,
    Equals = 46,
    LeftBracket = 47,
    RightBracket = 48,
    BackSlash = 49, /**< Located at the lower left of the return
                     *   key on ISO keyboards and at the right end
                     *   of the QWERTY row on ANSI keyboards.
                     *   Produces REVERSE SOLIDUS (backslash) and
                     *   VERTICAL LINE in a US layout, REVERSE
                     *   SOLIDUS and VERTICAL LINE in a UK Mac
                     *   layout, NUMBER SIGN and TILDE in a UK
                     *   Windows layout, DOLLAR SIGN and POUND SIGN
                     *   in a Swiss German layout, NUMBER SIGN and
                     *   APOSTROPHE in a German layout, GRAVE
                     *   ACCENT and POUND SIGN in a French Mac
                     *   layout, and ASTERISK and MICRO SIGN in a
                     *   French Windows layout.
                     */
    NonUSHash = 50, /**< ISO USB keyboards actually use this code
                     *   instead of 49 for the same key, but all
                     *   OSes I've seen treat the two codes
                     *   identically. So, as an implementor, unless
                     *   your keyboard generates both of those
                     *   codes and your OS treats them differently,
                     *   you should generate BACKSLASH
                     *   instead of this code. As a user, you
                     *   should not rely on this code because SDL
                     *   will never generate it with most (all?)
                     *   keyboards.
                     */
    Semicolon = 51,
    Apostrophe = 52,
    Grave = 53, /**< Located in the top left corner (on both ANSI
                 *   and ISO keyboards). Produces GRAVE ACCENT and
                 *   TILDE in a US Windows layout and in US and UK
                 *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                 *   and NOT SIGN in a UK Windows layout, SECTION
                 *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                 *   layouts on ISO keyboards, SECTION SIGN and
                 *   DEGREE SIGN in a Swiss German layout (Mac:
                 *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                 *   DEGREE SIGN in a German layout (Mac: only on
                 *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                 *   French Windows layout, COMMERCIAL AT and
                 *   NUMBER SIGN in a French Mac layout on ISO
                 *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                 *   SIGN in a Swiss German, German, or French Mac
                 *   layout on ANSI keyboards.
                 */
    Comma = 54,
    Period = 55,
    Slash = 56,

    Capslock = 57,

    F1 = 58,
    F2 = 59,
    F3 = 60,
    F4 = 61,
    F5 = 62,
    F6 = 63,
    F7 = 64,
    F8 = 65,
    F9 = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,

    Printscreen = 70,
    Scrolllock = 71,
    Pause = 72,
    Insert = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    Home = 74,
    PageUp = 75,
    Delete = 76,
    End = 77,
    PageDown = 78,
    Right = 79,
    Left = 80,
    Down = 81,
    Up = 82,

    NumLockClear = 83, /**< num lock on PC, clear on Mac keyboards */
    KPDivide = 84,
    KPMultiply = 85,
    KPMinus = 86,
    KPPlus = 87,
    KPEnter = 88,
    KP1 = 89,
    KP2 = 90,
    KP3 = 91,
    KP4 = 92,
    KP5 = 93,
    KP6 = 94,
    KP7 = 95,
    KP8 = 96,
    KP9 = 97,
    KP0 = 98,
    KPPeriod = 99,

    NonUSBackslash = 100, /**< This is the additional key that ISO
                           *   keyboards have over ANSI ones,
                           *   located between left shift and Y.
                           *   Produces GRAVE ACCENT and TILDE in a
                           *   US or UK Mac layout, REVERSE SOLIDUS
                           *   (backslash) and VERTICAL LINE in a
                           *   US or UK Windows layout, and
                           *   LESS-THAN SIGN and GREATER-THAN SIGN
                           *   in a Swiss German, German, or French
                           *   layout. */
    Application = 101,    /**< windows contextual menu, compose */
    Power = 102,          /**< The USB document says this is a status flag,
                           *   not a physical key - but some Mac keyboards
                           *   do have a power key. */
    KPEquals = 103,
    F13 = 104,
    F14 = 105,
    F15 = 106,
    F16 = 107,
    F17 = 108,
    F18 = 109,
    F19 = 110,
    F20 = 111,
    F21 = 112,
    F22 = 113,
    F23 = 114,
    F24 = 115,
    Execute = 116,
    Help = 117,
    Menu = 118,
    Select = 119,
    Stop = 120,
    Again = 121, /**< redo */
    Undo = 122,
    Cut = 123,
    Copy = 124,
    Paste = 125,
    Find = 126,
    Mute = 127,
    VolumeUp = 128,
    VolumeDown = 129,
    /* not sure whether there's a reason to enable these */
    /*     LOCKINGCAPSLOCK = 130,  */
    /*     LOCKINGNUMLOCK = 131, */
    /*     LOCKINGSCROLLLOCK = 132, */
    KPComma = 133,
    KPEqualSAS400 = 134,

    InternationaL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    InternationaL2 = 136,
    InternationaL3 = 137, /**< Yen */
    InternationaL4 = 138,
    InternationaL5 = 139,
    InternationaL6 = 140,
    InternationaL7 = 141,
    InternationaL8 = 142,
    InternationaL9 = 143,
    Lang1 = 144, /**< Hangul/English toggle */
    Lang2 = 145, /**< Hanja conversion */
    Lang3 = 146, /**< Katakana */
    Lang4 = 147, /**< Hiragana */
    Lang5 = 148, /**< Zenkaku/Hankaku */
    Lang6 = 149, /**< reserved */
    Lang7 = 150, /**< reserved */
    Lang8 = 151, /**< reserved */
    Lang9 = 152, /**< reserved */

    AltErase = 153, /**< Erase-Eaze */
    SysReq = 154,
    Cancel = 155,
    Clear = 156,
    Prior = 157,
    Return2 = 158,
    Separator = 159,
    Out = 160,
    Oper = 161,
    ClearAgain = 162,
    CrSel = 163,
    ExSel = 164,

    KP00 = 176,
    KP000 = 177,
    ThousandsSeparator = 178,
    DecimalSeparator = 179,
    CurrencyUnit = 180,
    CurrencySubunit = 181,
    KPLeftParen = 182,
    KPRightParen = 183,
    KPLeftBrace = 184,
    KPRightBrace = 185,
    KPTab = 186,
    KPBackspace = 187,
    KPA = 188,
    KPB = 189,
    KPC = 190,
    KPD = 191,
    KPE = 192,
    KPF = 193,
    KPXor = 194,
    KPPower = 195,
    KPPercent = 196,
    KPLess = 197,
    KPGreater = 198,
    KPAmpersand = 199,
    KPDblampersand = 200,
    KPVerticalbar = 201,
    KPDBLVerticalbar = 202,
    KPColon = 203,
    KPHash = 204,
    KPSpace = 205,
    KPAt = 206,
    KPExclam = 207,
    KPMemStore = 208,
    KPMemRecall = 209,
    KPMemClear = 210,
    KPMemAdd = 211,
    KPMemSubtract = 212,
    KPMemMultiply = 213,
    KPMemDivide = 214,
    KPPlusMinus = 215,
    KPClear = 216,
    KPClearEntry = 217,
    KPBinary = 218,
    KPOctal = 219,
    KPDecimal = 220,
    KPHexadecimal = 221,

    LCtrl = 224,
    LShift = 225,
    LAlt = 226, /**< alt, option */
    LGui = 227, /**< windows, command (apple), meta */
    RCtrl = 228,
    RShift = 229,
    RAlt = 230, /**< alt gr, option */
    RGUI = 231, /**< windows, command (apple), meta */

    Mode = 257, /**< I'm not sure if this is really not covered
                 *   by any of the above, but since there's a
                 *   special KMOD_MODE for it I'm adding it here
                 */

    /* @} */ /* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    AudioNext = 258,
    AudioPrev = 259,
    AudioStop = 260,
    AudioPlay = 261,
    AudioMute = 262,
    MediaSelect = 263,
    WWW = 264,
    Mail = 265,
    Calculator = 266,
    Computer = 267,
    ACSearch = 268,
    ACHome = 269,
    ACBack = 270,
    ACForward = 271,
    ACStop = 272,
    ACRefresh = 273,
    ACBookmarks = 274,

    /* @} */ /* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    BrightnessDown = 275,
    BrightnessUp = 276,
    DisplaySwitch = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    KbdillumToggle = 278,
    KbdillumDown = 279,
    KbdillumUP = 280,
    Eject = 281,
    Sleep = 282,

    APP1 = 283,
    APP2 = 284,

    /* @} */ /* Walther keys */

    /**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    AudioRewind = 285,
    AudioFastforward = 286,

    /* @} */ /* Usage page 0x0C (additional media keys) */

    /* Add any other keys here. */

    NumOfScancodes = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
};

enum class Keycode : uint32_t {

    UNKNOWN = 0,

    RETURN = '\r',
    ESCAPE = '\x1B',
    BACKSPACE = '\b',
    TAB = '\t',
    SPACE = ' ',
    EXCLAIM = '!',
    QUOTEDBL = '"',
    HASH = '#',
    PERCENT = '%',
    DOLLAR = '$',
    AMPERSAND = '&',
    QUOTE = '\'',
    LEFTPAREN = '(',
    RIGHTPAREN = ')',
    ASTERISK = '*',
    PLUS = '+',
    COMMA = ',',
    MINUS = '-',
    PERIOD = '.',
    SLASH = '/',
    D0 = '0',
    D1 = '1',
    D2 = '2',
    D3 = '3',
    D4 = '4',
    D5 = '5',
    D6 = '6',
    D7 = '7',
    D8 = '8',
    D9 = '9',
    COLON = ':',
    SEMICOLON = ';',
    LESS = '<',
    EQUALS = '=',
    GREATER = '>',
    QUESTION = '?',
    AT = '@',

    /*
       Skip uppercase letters
     */

    LEFTBRACKET = '[',
    BACKSLASH = '\\',
    RIGHTBRACKET = ']',
    CARET = '^',
    UNDERSCORE = '_',
    BACKQUOTE = '`',
    A = 'a',
    B = 'b',
    C = 'c',
    D = 'd',
    E = 'e',
    F = 'f',
    G = 'g',
    H = 'h',
    I = 'i',
    J = 'j',
    K = 'k',
    L = 'l',
    M = 'm',
    N = 'n',
    O = 'o',
    P = 'p',
    Q = 'q',
    R = 'r',
    S = 's',
    T = 't',
    U = 'u',
    V = 'v',
    W = 'w',
    X = 'x',
    Y = 'y',
    Z = 'z',
    CAPSLOCK = SCANCODE_TO_KEYCODE(Scancode::Capslock),

    F1 = SCANCODE_TO_KEYCODE(Scancode::F1),
    F2 = SCANCODE_TO_KEYCODE(Scancode::F2),
    F3 = SCANCODE_TO_KEYCODE(Scancode::F3),
    F4 = SCANCODE_TO_KEYCODE(Scancode::F4),
    F5 = SCANCODE_TO_KEYCODE(Scancode::F5),
    F6 = SCANCODE_TO_KEYCODE(Scancode::F6),
    F7 = SCANCODE_TO_KEYCODE(Scancode::F7),
    F8 = SCANCODE_TO_KEYCODE(Scancode::F8),
    F9 = SCANCODE_TO_KEYCODE(Scancode::F9),
    F10 = SCANCODE_TO_KEYCODE(Scancode::F10),
    F11 = SCANCODE_TO_KEYCODE(Scancode::F11),
    F12 = SCANCODE_TO_KEYCODE(Scancode::F12),

    PRINTSCREEN = SCANCODE_TO_KEYCODE(Scancode::Printscreen),
    SCROLLLOCK = SCANCODE_TO_KEYCODE(Scancode::Scrolllock),
    PAUSE = SCANCODE_TO_KEYCODE(Scancode::Pause),
    INSERT = SCANCODE_TO_KEYCODE(Scancode::Insert),
    HOME = SCANCODE_TO_KEYCODE(Scancode::Home),
    PAGEUP = SCANCODE_TO_KEYCODE(Scancode::PageUp),
    DELETE = '\x7F',
    END = SCANCODE_TO_KEYCODE(Scancode::End),
    PAGEDOWN = SCANCODE_TO_KEYCODE(Scancode::PageDown),
    RIGHT = SCANCODE_TO_KEYCODE(Scancode::Right),
    LEFT = SCANCODE_TO_KEYCODE(Scancode::Left),
    DOWN = SCANCODE_TO_KEYCODE(Scancode::Down),
    UP = SCANCODE_TO_KEYCODE(Scancode::Up),

    NUMLOCKCLEAR = SCANCODE_TO_KEYCODE(Scancode::NumLockClear),
    KP_DIVIDE = SCANCODE_TO_KEYCODE(Scancode::KPDivide),
    KP_MULTIPLY = SCANCODE_TO_KEYCODE(Scancode::KPMultiply),
    KP_MINUS = SCANCODE_TO_KEYCODE(Scancode::KPMinus),
    KP_PLUS = SCANCODE_TO_KEYCODE(Scancode::KPPlus),
    KP_ENTER = SCANCODE_TO_KEYCODE(Scancode::KPEnter),
    KP_1 = SCANCODE_TO_KEYCODE(Scancode::KP1),
    KP_2 = SCANCODE_TO_KEYCODE(Scancode::KP2),
    KP_3 = SCANCODE_TO_KEYCODE(Scancode::KP3),
    KP_4 = SCANCODE_TO_KEYCODE(Scancode::KP4),
    KP_5 = SCANCODE_TO_KEYCODE(Scancode::KP5),
    KP_6 = SCANCODE_TO_KEYCODE(Scancode::KP6),
    KP_7 = SCANCODE_TO_KEYCODE(Scancode::KP7),
    KP_8 = SCANCODE_TO_KEYCODE(Scancode::KP8),
    KP_9 = SCANCODE_TO_KEYCODE(Scancode::KP9),
    KP_0 = SCANCODE_TO_KEYCODE(Scancode::KP0),
    KP_PERIOD = SCANCODE_TO_KEYCODE(Scancode::KPPeriod),

    APPLICATION = SCANCODE_TO_KEYCODE(Scancode::Application),
    POWER = SCANCODE_TO_KEYCODE(Scancode::Power),
    KP_EQUALS = SCANCODE_TO_KEYCODE(Scancode::KPEquals),
    F13 = SCANCODE_TO_KEYCODE(Scancode::F13),
    F14 = SCANCODE_TO_KEYCODE(Scancode::F14),
    F15 = SCANCODE_TO_KEYCODE(Scancode::F15),
    F16 = SCANCODE_TO_KEYCODE(Scancode::F16),
    F17 = SCANCODE_TO_KEYCODE(Scancode::F17),
    F18 = SCANCODE_TO_KEYCODE(Scancode::F18),
    F19 = SCANCODE_TO_KEYCODE(Scancode::F19),
    F20 = SCANCODE_TO_KEYCODE(Scancode::F20),
    F21 = SCANCODE_TO_KEYCODE(Scancode::F21),
    F22 = SCANCODE_TO_KEYCODE(Scancode::F22),
    F23 = SCANCODE_TO_KEYCODE(Scancode::F23),
    F24 = SCANCODE_TO_KEYCODE(Scancode::F24),
    EXECUTE = SCANCODE_TO_KEYCODE(Scancode::Execute),
    HELP = SCANCODE_TO_KEYCODE(Scancode::Help),
    MENU = SCANCODE_TO_KEYCODE(Scancode::Menu),
    SELECT = SCANCODE_TO_KEYCODE(Scancode::Select),
    STOP = SCANCODE_TO_KEYCODE(Scancode::Stop),
    AGAIN = SCANCODE_TO_KEYCODE(Scancode::Again),
    UNDO = SCANCODE_TO_KEYCODE(Scancode::Undo),
    CUT = SCANCODE_TO_KEYCODE(Scancode::Cut),
    COPY = SCANCODE_TO_KEYCODE(Scancode::Copy),
    PASTE = SCANCODE_TO_KEYCODE(Scancode::Paste),
    FIND = SCANCODE_TO_KEYCODE(Scancode::Find),
    MUTE = SCANCODE_TO_KEYCODE(Scancode::Mute),
    VOLUMEUP = SCANCODE_TO_KEYCODE(Scancode::VolumeUp),
    VOLUMEDOWN = SCANCODE_TO_KEYCODE(Scancode::VolumeDown),
    KP_COMMA = SCANCODE_TO_KEYCODE(Scancode::KPComma),
    KP_EQUALSAS400 = SCANCODE_TO_KEYCODE(Scancode::KPEqualSAS400),

    ALTERASE = SCANCODE_TO_KEYCODE(Scancode::AltErase),
    SYSREQ = SCANCODE_TO_KEYCODE(Scancode::SysReq),
    CANCEL = SCANCODE_TO_KEYCODE(Scancode::Cancel),
    CLEAR = SCANCODE_TO_KEYCODE(Scancode::Clear),
    PRIOR = SCANCODE_TO_KEYCODE(Scancode::Prior),
    RETURN2 = SCANCODE_TO_KEYCODE(Scancode::Return2),
    SEPARATOR = SCANCODE_TO_KEYCODE(Scancode::Separator),
    OUT = SCANCODE_TO_KEYCODE(Scancode::Out),
    OPER = SCANCODE_TO_KEYCODE(Scancode::Oper),
    CLEARAGAIN = SCANCODE_TO_KEYCODE(Scancode::ClearAgain),
    CRSEL = SCANCODE_TO_KEYCODE(Scancode::CrSel),
    EXSEL = SCANCODE_TO_KEYCODE(Scancode::ExSel),

    KP_00 = SCANCODE_TO_KEYCODE(Scancode::KP00),
    KP_000 = SCANCODE_TO_KEYCODE(Scancode::KP000),
    THOUSANDSSEPARATOR = SCANCODE_TO_KEYCODE(Scancode::ThousandsSeparator),
    DECIMALSEPARATOR = SCANCODE_TO_KEYCODE(Scancode::DecimalSeparator),
    CURRENCYUNIT = SCANCODE_TO_KEYCODE(Scancode::CURRENCYUNIT),
    CURRENCYSUBUNIT = SCANCODE_TO_KEYCODE(Scancode::CURRENCYSUBUNIT),
    KP_LEFTPAREN = SCANCODE_TO_KEYCODE(Scancode::KP_LEFTPAREN),
    KP_RIGHTPAREN = SCANCODE_TO_KEYCODE(Scancode::KP_RIGHTPAREN),
    KP_LEFTBRACE = SCANCODE_TO_KEYCODE(Scancode::KP_LEFTBRACE),
    KP_RIGHTBRACE = SCANCODE_TO_KEYCODE(Scancode::KP_RIGHTBRACE),
    KP_TAB = SCANCODE_TO_KEYCODE(Scancode::KP_TAB),
    KP_BACKSPACE = SCANCODE_TO_KEYCODE(Scancode::KP_BACKSPACE),
    KP_A = SCANCODE_TO_KEYCODE(Scancode::KP_A),
    KP_B = SCANCODE_TO_KEYCODE(Scancode::KP_B),
    KP_C = SCANCODE_TO_KEYCODE(Scancode::KP_C),
    KP_D = SCANCODE_TO_KEYCODE(Scancode::KP_D),
    KP_E = SCANCODE_TO_KEYCODE(Scancode::KP_E),
    KP_F = SCANCODE_TO_KEYCODE(Scancode::KP_F),
    KP_XOR = SCANCODE_TO_KEYCODE(Scancode::KP_XOR),
    KP_POWER = SCANCODE_TO_KEYCODE(Scancode::KP_POWER),
    KP_PERCENT = SCANCODE_TO_KEYCODE(Scancode::KP_PERCENT),
    KP_LESS = SCANCODE_TO_KEYCODE(Scancode::KP_LESS),
    KP_GREATER = SCANCODE_TO_KEYCODE(Scancode::KP_GREATER),
    KP_AMPERSAND = SCANCODE_TO_KEYCODE(Scancode::KP_AMPERSAND),
    KP_DBLAMPERSAND = SCANCODE_TO_KEYCODE(Scancode::KP_DBLAMPERSAND),
    KP_VERTICALBAR = SCANCODE_TO_KEYCODE(Scancode::KP_VERTICALBAR),
    KP_DBLVERTICALBAR = SCANCODE_TO_KEYCODE(Scancode::KP_DBLVERTICALBAR),
    KP_COLON = SCANCODE_TO_KEYCODE(Scancode::KP_COLON),
    KP_HASH = SCANCODE_TO_KEYCODE(Scancode::KP_HASH),
    KP_SPACE = SCANCODE_TO_KEYCODE(Scancode::KP_SPACE),
    KP_AT = SCANCODE_TO_KEYCODE(Scancode::KP_AT),
    KP_EXCLAM = SCANCODE_TO_KEYCODE(Scancode::KP_EXCLAM),
    KP_MEMSTORE = SCANCODE_TO_KEYCODE(Scancode::KP_MEMSTORE),
    KP_MEMRECALL = SCANCODE_TO_KEYCODE(Scancode::KP_MEMRECALL),
    KP_MEMCLEAR = SCANCODE_TO_KEYCODE(Scancode::KP_MEMCLEAR),
    KP_MEMADD = SCANCODE_TO_KEYCODE(Scancode::KP_MEMADD),
    KP_MEMSUBTRACT = SCANCODE_TO_KEYCODE(Scancode::KP_MEMSUBTRACT),
    KP_MEMMULTIPLY = SCANCODE_TO_KEYCODE(Scancode::KP_MEMMULTIPLY),
    KP_MEMDIVIDE = SCANCODE_TO_KEYCODE(Scancode::KP_MEMDIVIDE),
    KP_PLUSMINUS = SCANCODE_TO_KEYCODE(Scancode::KP_PLUSMINUS),
    KP_CLEAR = SCANCODE_TO_KEYCODE(Scancode::KP_CLEAR),
    KP_CLEARENTRY = SCANCODE_TO_KEYCODE(Scancode::KP_CLEARENTRY),
    KP_BINARY = SCANCODE_TO_KEYCODE(Scancode::KP_BINARY),
    KP_OCTAL = SCANCODE_TO_KEYCODE(Scancode::KP_OCTAL),
    KP_DECIMAL = SCANCODE_TO_KEYCODE(Scancode::KP_DECIMAL),
    KP_HEXADECIMAL = SCANCODE_TO_KEYCODE(Scancode::KP_HEXADECIMAL),

    LCTRL = SCANCODE_TO_KEYCODE(Scancode::LCTRL),
    LSHIFT = SCANCODE_TO_KEYCODE(Scancode::LSHIFT),
    LALT = SCANCODE_TO_KEYCODE(Scancode::LALT),
    LGUI = SCANCODE_TO_KEYCODE(Scancode::LGUI),
    RCTRL = SCANCODE_TO_KEYCODE(Scancode::RCTRL),
    RSHIFT = SCANCODE_TO_KEYCODE(Scancode::RSHIFT),
    RALT = SCANCODE_TO_KEYCODE(Scancode::RALT),
    RGUI = SCANCODE_TO_KEYCODE(Scancode::RGUI),

    MODE = SCANCODE_TO_KEYCODE(Scancode::MODE),

    AUDIONEXT = SCANCODE_TO_KEYCODE(Scancode::AUDIONEXT),
    AUDIOPREV = SCANCODE_TO_KEYCODE(Scancode::AUDIOPREV),
    AUDIOSTOP = SCANCODE_TO_KEYCODE(Scancode::AUDIOSTOP),
    AUDIOPLAY = SCANCODE_TO_KEYCODE(Scancode::AUDIOPLAY),
    AUDIOMUTE = SCANCODE_TO_KEYCODE(Scancode::AUDIOMUTE),
    MEDIASELECT = SCANCODE_TO_KEYCODE(Scancode::MEDIASELECT),
    WWW = SCANCODE_TO_KEYCODE(Scancode::WWW),
    MAIL = SCANCODE_TO_KEYCODE(Scancode::MAIL),
    CALCULATOR = SCANCODE_TO_KEYCODE(Scancode::CALCULATOR),
    COMPUTER = SCANCODE_TO_KEYCODE(Scancode::COMPUTER),
    AC_SEARCH = SCANCODE_TO_KEYCODE(Scancode::AC_SEARCH),
    AC_HOME = SCANCODE_TO_KEYCODE(Scancode::AC_HOME),
    AC_BACK = SCANCODE_TO_KEYCODE(Scancode::AC_BACK),
    AC_FORWARD = SCANCODE_TO_KEYCODE(Scancode::AC_FORWARD),
    AC_STOP = SCANCODE_TO_KEYCODE(Scancode::AC_STOP),
    AC_REFRESH = SCANCODE_TO_KEYCODE(Scancode::AC_REFRESH),
    AC_BOOKMARKS = SCANCODE_TO_KEYCODE(Scancode::AC_BOOKMARKS),

    BRIGHTNESSDOWN = SCANCODE_TO_KEYCODE(Scancode::BRIGHTNESSDOWN),
    BRIGHTNESSUP = SCANCODE_TO_KEYCODE(Scancode::BRIGHTNESSUP),
    DISPLAYSWITCH = SCANCODE_TO_KEYCODE(Scancode::DISPLAYSWITCH),
    KBDILLUMTOGGLE = SCANCODE_TO_KEYCODE(Scancode::KBDILLUMTOGGLE),
    KBDILLUMDOWN = SCANCODE_TO_KEYCODE(Scancode::KBDILLUMDOWN),
    KBDILLUMUP = SCANCODE_TO_KEYCODE(Scancode::KBDILLUMUP),
    EJECT = SCANCODE_TO_KEYCODE(Scancode::EJECT),
    SLEEP = SCANCODE_TO_KEYCODE(Scancode::SLEEP),
    APP1 = SCANCODE_TO_KEYCODE(Scancode::APP1),
    APP2 = SCANCODE_TO_KEYCODE(Scancode::APP2),

    AUDIOREWIND = SCANCODE_TO_KEYCODE(Scancode::AUDIOREWIND),
    AUDIOFASTFORWARD = SCANCODE_TO_KEYCODE(Scancode::AUDIOFASTFORWARD)
};

Keycode SD_CORE_API GetKeycodeFromScancode(Scancode scancode);
Scancode SD_CORE_API GetScancodeFromKeycode(Keycode keycode);

inline bool IsKeyModActive(uint16_t src, uint16_t dst) {
    return (src & dst) == dst;
}

}  // namespace SD

#endif
