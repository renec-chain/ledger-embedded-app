#ifdef HAVE_BAGL

#include "ux.h"

#include "handler/handler.h"

#include "apdu/types.h"

UX_STEP_NOCB(ux_display_public_flow_5_step,
             bnnn_paging,
             {
                .title = "Pubkey",
                .text = G_publicKeyStr,
             });
UX_STEP_CB(ux_display_public_flow_6_step,
           pb,
           sendResponse(set_result_get_pubkey(), SUCCESS, true),
           {
                &C_icon_validate_14,
                "Approve",
           });
UX_STEP_CB(ux_display_public_flow_7_step,
           pb,
           sendResponse(0, USER_REFUSAL, true),
           {
                &C_icon_crossmark,
                "Reject",
           });

UX_FLOW(ux_display_public_flow,
        &ux_display_public_flow_5_step,
        &ux_display_public_flow_6_step,
        &ux_display_public_flow_7_step);

void ui_get_public_key(void) {
    ux_flow_init(0, ux_display_public_flow, NULL);
}

#endif