#ifdef HAVE_BAGL
#include <ux.h>
#include <ux_flow_engine.h>

#include "glyphs.h"
#include "ui.h"


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

// We have a screen with the icon and "RENEC is ready"
UX_STEP_NOCB(ux_menu_ready_step, pnn, {&C_icon_eye, "RENEC", "is ready"});
UX_STEP_NOCB(ux_menu_version_step, bn, {"Version", APPVERSION});
// UX_STEP_CB(ux_menu_settings_step, pb, display_settings(NULL), {&C_icon_coggle, "Settings"});
UX_STEP_VALID(ux_menu_exit_step, pb, os_sched_exit(-1), {&C_icon_dashboard_x, "Quit"});

// FLOW for the main menu:
// #1 screen: ready
// #2 screen: version of the app
// #3 screen: quit
UX_FLOW(ux_menu_main_flow,
        &ux_menu_ready_step,
        // &ux_menu_settings_step,
        &ux_menu_version_step,
        &ux_menu_exit_step,
        FLOW_LOOP);

void ui_menu_main(void) {
    if (G_ux.stack_count == 0) {
        ux_stack_push();
    }

    ux_flow_init(0, ux_menu_main_flow, NULL);
}

#endif // HAVE_BAGL