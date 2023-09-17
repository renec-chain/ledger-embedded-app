#ifdef HAVE_BAGL

#include <ux.h>
#include <ux_flow_engine.h>

#include "glyphs.h"
#include "ui.h"
#include "globals.h"

static const char* settings_submenu_getter(unsigned int idx);
static void settings_submenu_selector(unsigned int idx);
static const char* allow_blind_sign_data_getter(unsigned int idx);
static void allow_blind_sign_data_selector(unsigned int idx);
static const char* pubkey_display_data_getter(unsigned int idx);
static void pubkey_display_data_selector(unsigned int idx);
static const char* display_mode_data_getter(unsigned int idx);
static void display_mode_data_selector(unsigned int idx);

// void switch_settings_hash_signing();

// // FLOW for the settings menu:
// // #1 screen: enable hash signing
// // #2 screen: quit
// #if defined(TARGET_NANOS)
// UX_STEP_CB(ux_settings_hash_signing_step,
//            bnnn_paging,
//            switch_settings_hash_signing(),
//            {
//                .title = "Hash signing",
//                .text = G.ui.detail_value,
//            });

// #endif

// // We have a screen with the icon and "RENEC is ready"
// UX_STEP_NOCB(ux_menu_ready_step, pnn, {&C_icon_eye, "RENEC", "is ready"});
// UX_STEP_NOCB(ux_menu_version_step, bn, {"Version", APPVERSION});
// // UX_STEP_CB(ux_menu_settings_step, pb, display_settings(NULL), {&C_icon_coggle, "Settings"});
// UX_STEP_VALID(ux_menu_exit_step, pb, os_sched_exit(-1), {&C_icon_dashboard_x, "Quit"});

// // FLOW for the main menu:
// // #1 screen: ready
// // #2 screen: version of the app
// // #3 screen: quit
// UX_FLOW(ux_menu_main_flow,
//         &ux_menu_ready_step,
//         // &ux_menu_settings_step,
//         &ux_menu_version_step,
//         &ux_menu_exit_step,
//         FLOW_LOOP);

// void ui_menu_main(void) {
//     if (G_ux.stack_count == 0) {
//         ux_stack_push();
//     }

//     ux_flow_init(0, ux_menu_main_flow, NULL);
// }

enum SettingsMenuOption {
    SettingsMenuOptionAllowBlindSign,
    SettingsMenuOptionPubkeyLength,
    SettingsMenuOptionDisplayMode,
    SettingsMenuOptionBack,
};

static unsigned int settings_submenu_option_index(enum SettingsMenuOption settings_menu_option) {
    switch (settings_menu_option) {
        case SettingsMenuOptionAllowBlindSign:
        case SettingsMenuOptionPubkeyLength:
        case SettingsMenuOptionDisplayMode:
            return (unsigned int) settings_menu_option;
        default:
            break;
    }
    return 0;
}

const char* const settings_submenu_getter_values[] = {
    "Allow blind sign",
    "Pubkey length",
    "Display mode",
    "Back",
};

static const char* settings_submenu_getter(unsigned int idx) {
    if (idx < ARRAYLEN(settings_submenu_getter_values)) {
        return settings_submenu_getter_values[idx];
    }
    return NULL;
}

static void settings_submenu_selector(unsigned int idx) {
    switch (idx) {
        case 0:
            ux_menulist_init_select(0,
                                    allow_blind_sign_data_getter,
                                    allow_blind_sign_data_selector,
                                    N_storage.settings.allow_blind_sign);
            break;
        // case 1:
        //     ux_menulist_init_select(0,
        //                             pubkey_display_data_getter,
        //                             pubkey_display_data_selector,
        //                             N_storage.settings.pubkey_display);
        //     break;
        // case 2:
        //     ux_menulist_init_select(0,
        //                             display_mode_data_getter,
        //                             display_mode_data_selector,
        //                             N_storage.settings.display_mode);
        //     break;
    default:
        ui_idle();
    }
}

static void allow_blind_sign_data_change(enum BlindSign blind_sign) {
    uint8_t value;
    switch (blind_sign) {
        case BLIND_SIGN_DISABLED:
        case BLIND_SIGN_ENABLED:
            value = (uint8_t) blind_sign;
            nvm_write((void *) &N_storage.settings.allow_blind_sign, &value, sizeof(value));
            break;
    
    default:
        break;
    }
}

const char* const no_yes_data_getter_values[] = {"No", "Yes", "Back"};

static const char* allow_blind_sign_data_getter(unsigned int idx) {
    if (idx < ARRAYLEN(no_yes_data_getter_values)) {
        return no_yes_data_getter_values[idx];
    }
    return NULL;
}

static void allow_blind_sign_data_selector(unsigned int idx) {
    switch (idx) {
        case 0:
            allow_blind_sign_data_change(BLIND_SIGN_DISABLED);
            break;
        case 1:
            allow_blind_sign_data_change(BLIND_SIGN_ENABLED);
            break;
        default:
            break;
    }
    unsigned int select_item = settings_submenu_option_index(SettingsMenuOptionAllowBlindSign);
    ux_menulist_init_select(0, settings_submenu_getter, settings_submenu_selector, select_item);
}

UX_STEP_NOCB(ux_idle_flow_1_step,
             pnn,
             {
                &C_icon_eye,
                "RENEC",
                "is ready",
             });
UX_STEP_CB(ux_idle_flow_2_step,
           pb,
           ux_menulist_init(0, settings_submenu_getter, settings_submenu_selector),
           {
                &C_icon_coggle,
                "Settings",
           });
UX_STEP_NOCB(ux_idle_flow_3_step,
             bn,
             {
                "Version",
                APPVERSION,
             });
UX_STEP_CB(ux_idle_flow_4_step,
           pb,
           os_sched_exit(-1),
           {
                &C_icon_dashboard_x,
                "Quit",
           });
UX_FLOW(ux_idle_flow,
        &ux_idle_flow_1_step,
        &ux_idle_flow_2_step,
        &ux_idle_flow_3_step,
        &ux_idle_flow_4_step,
        FLOW_LOOP);

void ui_idle(void) {
    // reserve a display stack slot if none yet
    if (G_ux.stack_count == 0) {
        ux_stack_push();
    }
    ux_flow_init(0, ux_idle_flow, NULL);
}

#endif // HAVE_BAGL