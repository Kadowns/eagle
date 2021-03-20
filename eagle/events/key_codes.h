//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_KEYCODES_H
#define EAGLE_KEYCODES_H

//--Key codes from glfw3.h--//

//key actions
#define EG_RELEASE                0
#define EG_PRESS                  1
#define EG_REPEAT                 2

/* The unknown key */
#define EG_KEY_UNKNOWN            -1

/* Printable keys */
#define EG_KEY_SPACE              32
#define EG_KEY_APOSTROPHE         39  /* ' */
#define EG_KEY_COMMA              44  /* , */
#define EG_KEY_MINUS              45  /* - */
#define EG_KEY_PERIOD             46  /* . */
#define EG_KEY_SLASH              47  /* / */
#define EG_KEY_0                  48
#define EG_KEY_1                  49
#define EG_KEY_2                  50
#define EG_KEY_3                  51
#define EG_KEY_4                  52
#define EG_KEY_5                  53
#define EG_KEY_6                  54
#define EG_KEY_7                  55
#define EG_KEY_8                  56
#define EG_KEY_9                  57
#define EG_KEY_SEMICOLON          59  /* ; */
#define EG_KEY_EQUAL              61  /* = */
#define EG_KEY_A                  65
#define EG_KEY_B                  66
#define EG_KEY_C                  67
#define EG_KEY_D                  68
#define EG_KEY_E                  69
#define EG_KEY_F                  70
#define EG_KEY_G                  71
#define EG_KEY_H                  72
#define EG_KEY_I                  73
#define EG_KEY_J                  74
#define EG_KEY_K                  75
#define EG_KEY_L                  76
#define EG_KEY_M                  77
#define EG_KEY_N                  78
#define EG_KEY_O                  79
#define EG_KEY_P                  80
#define EG_KEY_Q                  81
#define EG_KEY_R                  82
#define EG_KEY_S                  83
#define EG_KEY_T                  84
#define EG_KEY_U                  85
#define EG_KEY_V                  86
#define EG_KEY_W                  87
#define EG_KEY_X                  88
#define EG_KEY_Y                  89
#define EG_KEY_Z                  90
#define EG_KEY_LEFT_BRACKET       91  /* [ */
#define EG_KEY_BACKSLASH          92  /* \ */
#define EG_KEY_RIGHT_BRACKET      93  /* ] */
#define EG_KEY_GRAVE_ACCENT       96  /* ` */
#define EG_KEY_WORLD_1            161 /* non-US #1 */
#define EG_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define EG_KEY_ESCAPE             256
#define EG_KEY_ENTER              257
#define EG_KEY_TAB                258
#define EG_KEY_BACKSPACE          259
#define EG_KEY_INSERT             260
#define EG_KEY_DELETE             261
#define EG_KEY_RIGHT              262
#define EG_KEY_LEFT               263
#define EG_KEY_DOWN               264
#define EG_KEY_UP                 265
#define EG_KEY_PAGE_UP            266
#define EG_KEY_PAGE_DOWN          267
#define EG_KEY_HOME               268
#define EG_KEY_END                269
#define EG_KEY_CAPS_LOCK          280
#define EG_KEY_SCROLL_LOCK        281
#define EG_KEY_NUM_LOCK           282
#define EG_KEY_PRINT_SCREEN       283
#define EG_KEY_PAUSE              284
#define EG_KEY_F1                 290
#define EG_KEY_F2                 291
#define EG_KEY_F3                 292
#define EG_KEY_F4                 293
#define EG_KEY_F5                 294
#define EG_KEY_F6                 295
#define EG_KEY_F7                 296
#define EG_KEY_F8                 297
#define EG_KEY_F9                 298
#define EG_KEY_F10                299
#define EG_KEY_F11                300
#define EG_KEY_F12                301
#define EG_KEY_F13                302
#define EG_KEY_F14                303
#define EG_KEY_F15                304
#define EG_KEY_F16                305
#define EG_KEY_F17                306
#define EG_KEY_F18                307
#define EG_KEY_F19                308
#define EG_KEY_F20                309
#define EG_KEY_F21                310
#define EG_KEY_F22                311
#define EG_KEY_F23                312
#define EG_KEY_F24                313
#define EG_KEY_F25                314
#define EG_KEY_KP_0               320
#define EG_KEY_KP_1               321
#define EG_KEY_KP_2               322
#define EG_KEY_KP_3               323
#define EG_KEY_KP_4               324
#define EG_KEY_KP_5               325
#define EG_KEY_KP_6               326
#define EG_KEY_KP_7               327
#define EG_KEY_KP_8               328
#define EG_KEY_KP_9               329
#define EG_KEY_KP_DECIMAL         330
#define EG_KEY_KP_DIVIDE          331
#define EG_KEY_KP_MULTIPLY        332
#define EG_KEY_KP_SUBTRACT        333
#define EG_KEY_KP_ADD             334
#define EG_KEY_KP_ENTER           335
#define EG_KEY_KP_EQUAL           336
#define EG_KEY_LEFT_SHIFT         340
#define EG_KEY_LEFT_CONTROL       341
#define EG_KEY_LEFT_ALT           342
#define EG_KEY_LEFT_SUPER         343
#define EG_KEY_RIGHT_SHIFT        344
#define EG_KEY_RIGHT_CONTROL      345
#define EG_KEY_RIGHT_ALT          346
#define EG_KEY_RIGHT_SUPER        347
#define EG_KEY_MENU               348

#define EG_KEY_LAST               EG_KEY_MENU

/*! @} */

/*! @defgroup mods Modifier key flags
 *  @brief Modifier key flags.
 *
 *  See [key input](@ref input_key) for how these are used.
 *
 *  @ingroup input
 *  @{ */

/*! @brief If this bit is set one or more Shift keys were held down.
 *
 *  If this bit is set one or more Shift keys were held down.
 */
#define EG_MOD_SHIFT           0x0001
/*! @brief If this bit is set one or more Control keys were held down.
 *
 *  If this bit is set one or more Control keys were held down.
 */
#define EG_MOD_CONTROL         0x0002
/*! @brief If this bit is set one or more Alt keys were held down.
 *
 *  If this bit is set one or more Alt keys were held down.
 */
#define EG_MOD_ALT             0x0004
/*! @brief If this bit is set one or more Super keys were held down.
 *
 *  If this bit is set one or more Super keys were held down.
 */
#define EG_MOD_SUPER           0x0008
/*! @brief If this bit is set the Caps Lock key is enabled.
 *
 *  If this bit is set the Caps Lock key is enabled and the @ref
 *  EG_LOCK_KEY_MODS input mode is set.
 */
#define EG_MOD_CAPS_LOCK       0x0010
/*! @brief If this bit is set the Num Lock key is enabled.
 *
 *  If this bit is set the Num Lock key is enabled and the @ref
 *  EG_LOCK_KEY_MODS input mode is set.
 */
#define EG_MOD_NUM_LOCK        0x0020

/*! @} */

/*! @defgroup buttons Mouse buttons
 *  @brief Mouse button IDs.
 *
 *  See [mouse button input](@ref input_mouse_button) for how these are used.
 *
 *  @ingroup input
 *  @{ */
#define EG_MOUSE_BUTTON_1         0
#define EG_MOUSE_BUTTON_2         1
#define EG_MOUSE_BUTTON_3         2
#define EG_MOUSE_BUTTON_4         3
#define EG_MOUSE_BUTTON_5         4
#define EG_MOUSE_BUTTON_6         5
#define EG_MOUSE_BUTTON_7         6
#define EG_MOUSE_BUTTON_8         7
#define EG_MOUSE_BUTTON_LAST      EG_MOUSE_BUTTON_8
#define EG_MOUSE_BUTTON_LEFT      EG_MOUSE_BUTTON_1
#define EG_MOUSE_BUTTON_RIGHT     EG_MOUSE_BUTTON_2
#define EG_MOUSE_BUTTON_MIDDLE    EG_MOUSE_BUTTON_3


#endif //EAGLE_KEYCODES_H
