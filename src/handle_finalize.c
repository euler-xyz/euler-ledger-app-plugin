#include "euler_plugin.h"

void handle_finalize(void *parameters) {
    ethPluginFinalize_t *msg = (ethPluginFinalize_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;
    msg->result = ETH_PLUGIN_RESULT_OK;
    msg->tokenLookup1 = context->address_2;

    switch (context->selectorIndex) {
        case ACTIVATE_MARKET:
            msg->numScreens = 1;
            break;
        case BATCH_DISPATCH:
            if (context->batch_dispatch_number_of_items == 1) {
                switch (context->item_types[0]) {
                    case ENTER_MARKET:
                    case EXIT_MARKET:
                    case USE_PERMIT:
                    case USE_PERMIT_ALLOWED:
                    case USE_PERMIT_PACKED:
                        msg->numScreens = 2;
                        break;
                    case DEPOSIT:
                    case WITHDRAW:
                    case BORROW:
                    case REPAY:
                    case MINT:
                    case BURN:
                        msg->numScreens = 3;
                        break;
                    case TRANSFER:
                        msg->numScreens = 4;
                        break;
                    case TRANSFER_FROM:
                        msg->numScreens = 5;
                        break;
                    case SWAP:
                    case SWAP_AND_REPAY:
                        msg->numScreens = 5;
                        msg->tokenLookup2 = context->address_3;
                        break;
                    default:
                        msg->numScreens = 1;
                        break;
                }
            } else {
                if (context->is_max_exceeded) {
                    msg->numScreens = 1 + MAX_ITEMS_IN_BATCH / 2 + 1;
                } else {
                    msg->numScreens = 1 + context->batch_dispatch_number_of_items / 2 + context->batch_dispatch_number_of_items % 2;
                }
            }
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    } 
}
