// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    _COLEMAK = 0, // 0
    _MEDIA,       // 1
    _NAVIGATION,  // 2
    _MOUSYS,      // 3
    _FUNCTION,    // 4
    _SYMBOLS,     // 5
    _NUMPAD,      // 6
};

enum custom_keycodes {
    VVV = KC_TRNS,
    XXX = KC_NO,
    LSFT_A = LSFT_T(KC_A),
    LCTL_R = LCTL_T(KC_R),
    LALT_S = LALT_T(KC_S),
    LGUI_T = LGUI_T(KC_T),
    RGUI_N = RGUI_T(KC_N),
    RALT_E = RALT_T(KC_E),
    RCTL_I = RCTL_T(KC_I),
    RSFT_O = RSFT_T(KC_O),

    LALT_PR = LALT_T(KC_PERC),
    LSFT_SL = LSFT_T(KC_SLASH),
    RGUI_EQ = RGUI_T(KC_EQL),
    RCTL_DA = RCTL_T(KC_MINS),

    // The following are altered in the process_record_user function
    LCTL_UN = LCTL_T(KC_UNDS),
    LALT_LP = LALT_T(KC_LPRN),
    LGUI_RP = LGUI_T(KC_RPRN),
    RALT_PL = RALT_T(KC_PLUS),
    RSFT_PR = RSFT_T(KC_PERC),

    LT1_ESC = LT(_MEDIA, KC_ESC),
    LT2_BSP = LT(_NAVIGATION, KC_BSPC),
    LT3_DEL = LT(_MOUSYS, KC_DEL),
    LT4_ENT = LT(_FUNCTION, KC_ENT),
    LT5_SPC = LT(_SYMBOLS, KC_SPC),
    LT6_TAB = LT(_NUMPAD, KC_TAB),

    REDO = LSG(KC_Z),
    UNDO = LGUI(KC_Z),
    COPY = LGUI(KC_C),
    PAST = LGUI(KC_V),
    CUT = LGUI(KC_X),
    PSPACE = LCTL(KC_LEFT),
    NSPACE = LCTL(KC_RIGHT),
    ALFRED = LGUI(KC_SPACE),

    CS_COMA = SAFE_RANGE,
    CS_PERI,
    CS_SLSH,
};

const key_override_t comma_semicolon_override = ko_make_basic(MOD_MASK_SHIFT, CS_COMA, KC_SCLN);
const key_override_t dot_colon_override = ko_make_basic(MOD_MASK_SHIFT, CS_PERI, KC_COLN);
const key_override_t slash_exclaim_override = ko_make_basic(MOD_MASK_SHIFT, CS_SLSH, KC_EXLM);

const key_override_t **key_overrides = (const key_override_t *[]){
    &comma_semicolon_override,
    &dot_colon_override,
    &slash_exclaim_override,
    NULL // Null terminate the array
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_UNDS):
        case LALT_T(KC_LPRN):
        case LGUI_T(KC_RPRN):
        case RALT_T(KC_PLUS):
        case RSFT_T(KC_PERC):
            if (record->event.pressed) {
                if (record->tap.count && !record->tap.interrupted) {
                    uint8_t mods = get_mods();
                    uint8_t oneshot_mods = get_oneshot_mods();
                    clear_mods();
                    clear_oneshot_mods();
                    
                    char to_send = 0;
                    switch (keycode) {
                        case LSFT_T(KC_UNDS): to_send = '_'; break;
                        case LALT_T(KC_LPRN): to_send = '('; break;
                        case LGUI_T(KC_RPRN): to_send = ')'; break;
                        case RALT_T(KC_PLUS): to_send = '+'; break;
                        case RSFT_T(KC_PERC): to_send = '%'; break;
                    }
                    
                    if (mods & MOD_MASK_CTRL || oneshot_mods & MOD_MASK_CTRL) {
                        SEND_STRING(SS_LCTL(SS_TAP(X_ESCAPE))); // Send Ctrl+Esc
                        send_char(to_send);
                    } else if (mods & MOD_MASK_ALT || oneshot_mods & MOD_MASK_ALT) {
                        SEND_STRING(SS_LALT(SS_TAP(X_ESCAPE))); // Send Alt+Esc
                        send_char(to_send);
                    } else if (mods & MOD_MASK_GUI || oneshot_mods & MOD_MASK_GUI) {
                        SEND_STRING(SS_LGUI(SS_TAP(X_ESCAPE))); // Send Gui+Esc
                        send_char(to_send);
                    } else {
                        send_char(to_send);
                    }
                    
                    set_mods(mods);
                    set_oneshot_mods(oneshot_mods);
                    return false;
                }
                // If it's not a tap or it's interrupted, let QMK handle it as a normal mod-tap
            }
            break;
        case KC_COMM:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    if (get_mods() & MOD_MASK_CSAG) {
                        unregister_code(KC_LSFT);
                        unregister_code(KC_RSFT);
                        SEND_STRING(";");
                        set_mods(get_mods());
                    } else {
                        SEND_STRING(":");
                    }
                } else {
                    SEND_STRING(",");
                }
            }
            return false;
        case KC_DOT:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    if (get_mods() & MOD_MASK_CSAG) {
                        unregister_code(KC_LSFT);
                        unregister_code(KC_RSFT);
                        SEND_STRING(":");
                        set_mods(get_mods());
                    } else {
                        SEND_STRING(">");
                    }
                } else {
                    SEND_STRING(".");
                }
            }
            return false;
        case KC_SLSH:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    SEND_STRING("!");
                } else {
                    SEND_STRING("?");
                }
            }
            return false;
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │ Q │ W │ F │ P │ B │       │ J │ L │ U │ Y │ ' │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ A │ R │ S │ T │ G │       │ M │ N │ E │ I │ O │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ Z │ X │ C │ D │ V │       │ K │ H │ , │ . │ ? │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │Esc├───┐           ┌───┤Tab│
      *           └───┤Bsp├───┐   ┌───┤Spc├───┘
      *               └───┤Del│   │Ent├───┘
      *                   └───┘   └───┘
      */
    [_COLEMAK] = LAYOUT_split_3x5_3(
        //|--------------------------------------------|                    |--------------------------------------------|
           KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                         KC_J,    KC_L,    KC_U,    KC_Y,    KC_QUOTE,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           LSFT_A,  LCTL_R,  LALT_S,  LGUI_T,  KC_G,                         KC_M,    RGUI_N,  RALT_E,  RCTL_I,  RSFT_O,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,                         KC_K,    KC_H,    KC_COMM, KC_DOT,  KC_SLSH,
        //|--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------|
                                      LT1_ESC, LT2_BSP, LT3_DEL,    LT4_ENT, LT5_SPC, LT6_TAB
        //                           |--------+--------+--------|  |--------+--------+--------|
    ),

     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │   │   │   │   │   │       │   │   │   │   │   │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │SFT│CTL│ALT│GUI│   │       │   │ < │ - │ + │ > │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │   │   │   │   │   │       │   │   │ - │ + │   │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │Esc├───┐           ┌───┤Tab│
      *           └───┤Bsp├───┐   ┌───┤Spc├───┘
      *               └───┤Del│   │Ent├───┘
      *                   └───┘   └───┘
      */
    [_MEDIA] = LAYOUT_split_3x5_3(
        //|--------------------------------------------|                    |--------------------------------------------|
           XXX,     XXX,     XXX,     XXX,     XXX,                          XXX,     XXX,     XXX,     XXX,     XXX,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, XXX,                          XXX,     KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           XXX,     XXX,     XXX,     XXX,     XXX,                          XXX,     XXX,     KC_BRID, KC_BRIU, XXX,
        //|--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------|
                                      VVV,     VVV,     VVV,        VVV,     VVV,     VVV
        //                           |--------+--------+--------|  |--------+--------+--------|
    ),
    
     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │   │   │   │   │   │       │Red│Cop│Pst│Cut│Und│
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │SFT│CTL│ALT│GUI│   │       │Cps│ < │ v │ ^ │ > │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │   │   │   │   │   │       │Ins│Hom│PDn│PUp│End│
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │Esc├───┐           ┌───┤Tab│
      *           └───┤Bsp├───┐   ┌───┤Spc├───┘
      *               └───┤Del│   │Ent├───┘
      *                   └───┘   └───┘
      */
    [_NAVIGATION] = LAYOUT_split_3x5_3(
        //|--------------------------------------------|                    |--------------------------------------------|
           XXX,     XXX,     XXX,     XXX,     XXX,                          REDO,    COPY,    PAST,    CUT,     UNDO,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, XXX,                          KC_CAPS, KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           XXX,     XXX,     XXX,     XXX,     XXX,                          KC_INS,  KC_HOME, KC_PGDN, KC_PGUP, KC_END,
        //|--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------|
                                      VVV,     VVV,     VVV,        VVV,     VVV,     VVV
        //                           |--------+--------+--------|  |--------+--------+--------|
    ),

     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │   │   │   │   │   │       │Pwr│SpL│Alf│Win│SpR│
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │SFT│CTL│ALT│GUI│   │       │Slp│ < │ v │ ^ │ > │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │   │   │   │   │   │       │   │   │   │   │   │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │Esc├───┐           ┌───┤Ms2│
      *           └───┤Bsp├───┐   ┌───┤Ms1├───┘
      *               └───┤Del│   │Ms3├───┘
      *                   └───┘   └───┘
      */
    [_MOUSYS] = LAYOUT_split_3x5_3(
        //|--------------------------------------------|                    |--------------------------------------------|
           XXX,     XXX,     XXX,     XXX,     XXX,                          KC_PWR,  PSPACE,  ALFRED,  KC_MCTL, NSPACE,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, XXX,                          XXX,     KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           XXX,     XXX,     XXX,     XXX,     XXX,                          XXX,     KC_WH_L, KC_WH_D, KC_WH_U, KC_WH_R,
        //|--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------|
                                      VVV,     VVV,     VVV,        KC_BTN3, KC_BTN1, KC_BTN2
        //                           |--------+--------+--------|  |--------+--------+--------|
    ),

     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │F12│F7 │F8 │F9 │PtS│       │   │   │   │   │   │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │F11│F4 │F5 │F6 │ScL│       │   │   │   │   │   │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │F10│F1 │F2 │F3 │PsB│       │   │   │   │   │   │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │Esc├───┐           ┌───┤Tab│
      *           └───┤Bsp├───┐   ┌───┤Spc├───┘
      *               └───┤Del│   │Ent├───┘
      *                   └───┘   └───┘
      */
    [_FUNCTION] = LAYOUT_split_3x5_3(
        //|--------------------------------------------|                    |--------------------------------------------|
           KC_F12,  KC_F7,   KC_F8,   KC_F9,   KC_PSCR,                      XXX,     XXX,     XXX,     XXX,     XXX,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_F11,  KC_F4,   KC_F5,   KC_F6,   KC_SCRL,                      XXX,     XXX,     XXX,     XXX,     XXX,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_F10,  KC_F1,   KC_F2,   KC_F3,   KC_PAUSE,                     XXX,     XXX,     XXX,     XXX,     XXX,
        //|--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------|
                                      VVV,     VVV,     VVV,        VVV,     VVV,     VVV
        //                           |--------+--------+--------|  |--------+--------+--------|
    ),

     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │ ` │ & │ [ │ ] │ * │       │ ^ │ < │ > │ $ │ " │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ / │ _ │ ( │ ) │ \ │       │ ~ │ = │ + │ - │ % │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ @ │ | │ { │ } │   │       │   │ # │ ; │ : │ ! │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │Esc├───┐           ┌───┤Tab│
      *           └───┤Bsp├───┐   ┌───┤Spc├───┘
      *               └───┤Del│   │Ent├───┘
      *                   └───┘   └───┘
      */
    [_SYMBOLS] = LAYOUT_split_3x5_3(
        //|--------------------------------------------|                    |--------------------------------------------|
           KC_GRV,  KC_AMPR, KC_LBRC, KC_RBRC, KC_ASTR,                      KC_CIRC, KC_LT,   KC_GT,   KC_DLR,  KC_DQT,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           LSFT_SL, LCTL_UN, LALT_LP, LGUI_RP, KC_BSLS,                      KC_TILD, RGUI_EQ, RALT_PL, RCTL_DA, RSFT_PR,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_AT,   KC_PIPE, KC_LCBR, KC_RCBR, XXX,                          XXX,     KC_HASH, KC_SCLN, KC_COLN, KC_EXLM,
        //|--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------|
                                      VVV,     VVV,     VVV,        VVV,     VVV,     VVV
        //                           |--------+--------+--------|  |--------+--------+--------|
    ),

     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │ / │ 7 │ 8 │ 9 │ * │       │   │   │   │   │   │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ 0 │ 4 │ 5 │ 6 │ - │       │   │CMD│OPT│CTL│SFT│
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ = │ 1 │ 2 │ 3 │ + │       │   │   │   │   │   │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │ . ├───┐           ┌───┤Tab│
      *           └───┤ , ├───┐   ┌───┤Spc├───┘
      *               └───┤Del│   │Ent├───┘
      *                   └───┘   └───┘
      */
    [_NUMPAD] = LAYOUT_split_3x5_3(
        //|--------------------------------------------|                    |--------------------------------------------|
           KC_PSLS, KC_7,    KC_8,    KC_9,   KC_PAST,                       XXX,     XXX,     XXX,     XXX,     XXX,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_F11,  KC_4,    KC_5,    KC_6,   KC_PMNS,                       XXX,     KC_RGUI, KC_RALT, KC_RCTL, KC_RSFT,
        //|--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------|
           KC_PEQL, KC_1,    KC_2,    KC_3,   KC_PPLS,                       XXX,     XXX,     XXX,     XXX,     XXX,
        //|--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------|
                                      KC_PDOT, KC_COMM, VVV,        VVV,     VVV,     VVV
        //                           |--------+--------+--------|  |--------+--------+--------|
    )
};
