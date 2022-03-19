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
    LGUI = 0x0400,
    RGUI = 0x0800,
    Num = 0x1000,
    Caps = 0x2000,
    Mode = 0x4000,
    Reserved = 0x8000,

    Ctrl = LCtrl | RCtrl,
    Shift = LShift | RShift,
    Alt = LAlt | RAlt,
    GUI = LGUI | RGUI
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
    KPEqualsAs400 = 134,

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
    KPXOR = 194,
    KPPower = 195,
    KPPercent = 196,
    KPLess = 197,
    KPGreater = 198,
    KPAmpersand = 199,
    KPDBLAmpersand = 200,
    KPVerticalBar = 201,
    KPDBLVerticalBar = 202,
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
    LGUI = 227, /**< windows, command (apple), meta */
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
    KBDillumToggle = 278,
    KBDillumDown = 279,
    KBDillumUp = 280,
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

    Unknown = 0,

    Return = '\r',
    Escape = '\x1B',
    Backspace = '\b',
    Tab = '\t',
    Space = ' ',
    Exclaim = '!',
    QuoteDBL = '"',
    Hash = '#',
    Percent = '%',
    Dollar = '$',
    Ampersand = '&',
    Quote = '\'',
    LeftParen = '(',
    RightParen = ')',
    Asterisk = '*',
    Plus = '+',
    Comma = ',',
    Minus = '-',
    Period = '.',
    Slash = '/',
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
    Colon = ':',
    Semicolon = ';',
    Less = '<',
    Equals = '=',
    Greater = '>',
    Question = '?',
    At = '@',

    /*
       Skip uppercase letters
     */

    LeftBracket = '[',
    BackSlash = '\\',
    RightBrackeT = ']',
    Caret = '^',
    UnderScore = '_',
    BackQuote = '`',
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
    CapsLock = SCANCODE_TO_KEYCODE(Scancode::Capslock),

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

    PrintScreen = SCANCODE_TO_KEYCODE(Scancode::Printscreen),
    ScrollLOCK = SCANCODE_TO_KEYCODE(Scancode::Scrolllock),
    Pause = SCANCODE_TO_KEYCODE(Scancode::Pause),
    Insert = SCANCODE_TO_KEYCODE(Scancode::Insert),
    Home = SCANCODE_TO_KEYCODE(Scancode::Home),
    PageUp = SCANCODE_TO_KEYCODE(Scancode::PageUp),
    Delete = '\x7F',
    END = SCANCODE_TO_KEYCODE(Scancode::End),
    PageDown = SCANCODE_TO_KEYCODE(Scancode::PageDown),
    Right = SCANCODE_TO_KEYCODE(Scancode::Right),
    Left = SCANCODE_TO_KEYCODE(Scancode::Left),
    Down = SCANCODE_TO_KEYCODE(Scancode::Down),
    Up = SCANCODE_TO_KEYCODE(Scancode::Up),

    NumLockClear = SCANCODE_TO_KEYCODE(Scancode::NumLockClear),
    KPDivide = SCANCODE_TO_KEYCODE(Scancode::KPDivide),
    KPMultiply = SCANCODE_TO_KEYCODE(Scancode::KPMultiply),
    KPMinus = SCANCODE_TO_KEYCODE(Scancode::KPMinus),
    KPPlus = SCANCODE_TO_KEYCODE(Scancode::KPPlus),
    KPenter = SCANCODE_TO_KEYCODE(Scancode::KPEnter),
    KP1 = SCANCODE_TO_KEYCODE(Scancode::KP1),
    KP2 = SCANCODE_TO_KEYCODE(Scancode::KP2),
    KP3 = SCANCODE_TO_KEYCODE(Scancode::KP3),
    KP4 = SCANCODE_TO_KEYCODE(Scancode::KP4),
    KP5 = SCANCODE_TO_KEYCODE(Scancode::KP5),
    KP6 = SCANCODE_TO_KEYCODE(Scancode::KP6),
    KP7 = SCANCODE_TO_KEYCODE(Scancode::KP7),
    KP8 = SCANCODE_TO_KEYCODE(Scancode::KP8),
    KP9 = SCANCODE_TO_KEYCODE(Scancode::KP9),
    KP0 = SCANCODE_TO_KEYCODE(Scancode::KP0),
    KPPeriod = SCANCODE_TO_KEYCODE(Scancode::KPPeriod),

    Application = SCANCODE_TO_KEYCODE(Scancode::Application),
    Power = SCANCODE_TO_KEYCODE(Scancode::Power),
    KPEquals = SCANCODE_TO_KEYCODE(Scancode::KPEquals),
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
    Execute = SCANCODE_TO_KEYCODE(Scancode::Execute),
    Help = SCANCODE_TO_KEYCODE(Scancode::Help),
    Menu = SCANCODE_TO_KEYCODE(Scancode::Menu),
    Select = SCANCODE_TO_KEYCODE(Scancode::Select),
    Stop = SCANCODE_TO_KEYCODE(Scancode::Stop),
    Again = SCANCODE_TO_KEYCODE(Scancode::Again),
    Undo = SCANCODE_TO_KEYCODE(Scancode::Undo),
    Cut = SCANCODE_TO_KEYCODE(Scancode::Cut),
    Copy = SCANCODE_TO_KEYCODE(Scancode::Copy),
    Paste = SCANCODE_TO_KEYCODE(Scancode::Paste),
    Find = SCANCODE_TO_KEYCODE(Scancode::Find),
    Mute = SCANCODE_TO_KEYCODE(Scancode::Mute),
    VolumeUp = SCANCODE_TO_KEYCODE(Scancode::VolumeUp),
    VolumeDown = SCANCODE_TO_KEYCODE(Scancode::VolumeDown),
    KPComma = SCANCODE_TO_KEYCODE(Scancode::KPComma),
    KPEqualsAs400 = SCANCODE_TO_KEYCODE(Scancode::KPEqualsAs400),

    AltErase = SCANCODE_TO_KEYCODE(Scancode::AltErase),
    SysReq = SCANCODE_TO_KEYCODE(Scancode::SysReq),
    Cancel = SCANCODE_TO_KEYCODE(Scancode::Cancel),
    Clear = SCANCODE_TO_KEYCODE(Scancode::Clear),
    Prior = SCANCODE_TO_KEYCODE(Scancode::Prior),
    Return2 = SCANCODE_TO_KEYCODE(Scancode::Return2),
    Separator = SCANCODE_TO_KEYCODE(Scancode::Separator),
    Out = SCANCODE_TO_KEYCODE(Scancode::Out),
    Oper = SCANCODE_TO_KEYCODE(Scancode::Oper),
    ClearAgain = SCANCODE_TO_KEYCODE(Scancode::ClearAgain),
    CrSel = SCANCODE_TO_KEYCODE(Scancode::CrSel),
    ExSel = SCANCODE_TO_KEYCODE(Scancode::ExSel),

    KP00 = SCANCODE_TO_KEYCODE(Scancode::KP00),
    KP000 = SCANCODE_TO_KEYCODE(Scancode::KP000),
    ThousandsSeparator = SCANCODE_TO_KEYCODE(Scancode::ThousandsSeparator),
    DecimalSeparator = SCANCODE_TO_KEYCODE(Scancode::DecimalSeparator),
    CurrencyUnit = SCANCODE_TO_KEYCODE(Scancode::CurrencyUnit),
    CurrencySubunit = SCANCODE_TO_KEYCODE(Scancode::CurrencySubunit),
    KPLeftParen = SCANCODE_TO_KEYCODE(Scancode::KPLeftParen),
    KPRightParen = SCANCODE_TO_KEYCODE(Scancode::KPRightParen),
    KPLeftBRace = SCANCODE_TO_KEYCODE(Scancode::KPLeftBrace),
    KPRightBRace = SCANCODE_TO_KEYCODE(Scancode::KPRightBrace),
    KPTab = SCANCODE_TO_KEYCODE(Scancode::KPTab),
    KPBackspace = SCANCODE_TO_KEYCODE(Scancode::KPBackspace),
    KPA = SCANCODE_TO_KEYCODE(Scancode::KPA),
    KPB = SCANCODE_TO_KEYCODE(Scancode::KPB),
    KPC = SCANCODE_TO_KEYCODE(Scancode::KPC),
    KPD = SCANCODE_TO_KEYCODE(Scancode::KPD),
    KPE = SCANCODE_TO_KEYCODE(Scancode::KPE),
    KPF = SCANCODE_TO_KEYCODE(Scancode::KPF),
    KPXOR = SCANCODE_TO_KEYCODE(Scancode::KPXOR),
    KPPower = SCANCODE_TO_KEYCODE(Scancode::KPPower),
    KPPercent = SCANCODE_TO_KEYCODE(Scancode::KPPercent),
    KPLess = SCANCODE_TO_KEYCODE(Scancode::KPLess),
    KPGreater = SCANCODE_TO_KEYCODE(Scancode::KPGreater),
    KPAmpersand = SCANCODE_TO_KEYCODE(Scancode::KPAmpersand),
    KPDBLAmpersand = SCANCODE_TO_KEYCODE(Scancode::KPDBLAmpersand),
    KPVerticalBAR = SCANCODE_TO_KEYCODE(Scancode::KPVerticalBar),
    KPDBLVerticalBar = SCANCODE_TO_KEYCODE(Scancode::KPDBLVerticalBar),
    KPColon = SCANCODE_TO_KEYCODE(Scancode::KPColon),
    KPHash = SCANCODE_TO_KEYCODE(Scancode::KPHash),
    KPSpace = SCANCODE_TO_KEYCODE(Scancode::KPSpace),
    KPAt = SCANCODE_TO_KEYCODE(Scancode::KPAt),
    KPExclam = SCANCODE_TO_KEYCODE(Scancode::KPExclam),
    KPMemStore = SCANCODE_TO_KEYCODE(Scancode::KPMemStore),
    KPMemRecall = SCANCODE_TO_KEYCODE(Scancode::KPMemRecall),
    KPMemClear = SCANCODE_TO_KEYCODE(Scancode::KPMemClear),
    KPMemAdd = SCANCODE_TO_KEYCODE(Scancode::KPMemAdd),
    KPMemSubtract = SCANCODE_TO_KEYCODE(Scancode::KPMemSubtract),
    KPMemMultiply = SCANCODE_TO_KEYCODE(Scancode::KPMemMultiply),
    KPMemDivide = SCANCODE_TO_KEYCODE(Scancode::KPMemDivide),
    KPPlusMinus = SCANCODE_TO_KEYCODE(Scancode::KPPlusMinus),
    KPClear = SCANCODE_TO_KEYCODE(Scancode::KPClear),
    KPClearEntry = SCANCODE_TO_KEYCODE(Scancode::KPClearEntry),
    KPBinary = SCANCODE_TO_KEYCODE(Scancode::KPBinary),
    KPOctal = SCANCODE_TO_KEYCODE(Scancode::KPOctal),
    KPDecimal = SCANCODE_TO_KEYCODE(Scancode::KPDecimal),
    KPHexadecimal = SCANCODE_TO_KEYCODE(Scancode::KPHexadecimal),

    LCtrl = SCANCODE_TO_KEYCODE(Scancode::LCtrl),
    LShift = SCANCODE_TO_KEYCODE(Scancode::LShift),
    LAlt = SCANCODE_TO_KEYCODE(Scancode::LAlt),
    LGUI = SCANCODE_TO_KEYCODE(Scancode::LGUI),
    RCtrl = SCANCODE_TO_KEYCODE(Scancode::RCtrl),
    RShift = SCANCODE_TO_KEYCODE(Scancode::RShift),
    RAlt = SCANCODE_TO_KEYCODE(Scancode::RAlt),
    RGUI = SCANCODE_TO_KEYCODE(Scancode::RGUI),

    Mode = SCANCODE_TO_KEYCODE(Scancode::Mode),

    AudioNext = SCANCODE_TO_KEYCODE(Scancode::AudioNext),
    AudioPrev = SCANCODE_TO_KEYCODE(Scancode::AudioPrev),
    AudioStop = SCANCODE_TO_KEYCODE(Scancode::AudioStop),
    AudioPlay = SCANCODE_TO_KEYCODE(Scancode::AudioPlay),
    AudioMute = SCANCODE_TO_KEYCODE(Scancode::AudioMute),
    MediaSelect = SCANCODE_TO_KEYCODE(Scancode::MediaSelect),
    WWW = SCANCODE_TO_KEYCODE(Scancode::WWW),
    Mail = SCANCODE_TO_KEYCODE(Scancode::Mail),
    Calculator = SCANCODE_TO_KEYCODE(Scancode::Calculator),
    Computer = SCANCODE_TO_KEYCODE(Scancode::Computer),
    ACSearch = SCANCODE_TO_KEYCODE(Scancode::ACSearch),
    ACHome = SCANCODE_TO_KEYCODE(Scancode::ACHome),
    ACBack = SCANCODE_TO_KEYCODE(Scancode::ACBack),
    ACForward = SCANCODE_TO_KEYCODE(Scancode::ACForward),
    ACStop = SCANCODE_TO_KEYCODE(Scancode::ACStop),
    ACRefresh = SCANCODE_TO_KEYCODE(Scancode::ACRefresh),
    ACBookmarks = SCANCODE_TO_KEYCODE(Scancode::ACBookmarks),

    BrightnessDown = SCANCODE_TO_KEYCODE(Scancode::BrightnessDown),
    BrightnessUp = SCANCODE_TO_KEYCODE(Scancode::BrightnessUp),
    DisplaySwitch = SCANCODE_TO_KEYCODE(Scancode::DisplaySwitch),
    KBDillumToggle = SCANCODE_TO_KEYCODE(Scancode::KBDillumToggle),
    KBDillumDown = SCANCODE_TO_KEYCODE(Scancode::KBDillumDown),
    KBDillumUp = SCANCODE_TO_KEYCODE(Scancode::KBDillumUp),
    Eject = SCANCODE_TO_KEYCODE(Scancode::Eject),
    Sleep = SCANCODE_TO_KEYCODE(Scancode::Sleep),
    APP1 = SCANCODE_TO_KEYCODE(Scancode::APP1),
    APP2 = SCANCODE_TO_KEYCODE(Scancode::APP2),

    AudioRewind = SCANCODE_TO_KEYCODE(Scancode::AudioRewind),
    AudioFastforward = SCANCODE_TO_KEYCODE(Scancode::AudioFastforward)
};

Keycode SD_CORE_API GetKeycodeFromScancode(Scancode scancode);
Scancode SD_CORE_API GetScancodeFromKeycode(Keycode keycode);

inline bool IsKeyModActive(uint16_t src, uint16_t dst)
{
    return (src & dst) == dst;
}

}  // namespace SD

#endif
