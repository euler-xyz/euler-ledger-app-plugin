#include "euler_plugin.h"

void handle_provide_token(void *parameters) {
    ethPluginProvideInfo_t *msg = (ethPluginProvideInfo_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    if (msg->item1) {
        context->token_found_1 = true;
        context->decimals_1 = msg->item1->token.decimals;
        strlcpy(context->ticker_1, (char *) msg->item1->token.ticker, sizeof(context->ticker_1));
    } else {
        context->token_found_1 = false;
    }

    if (msg->item2) {
        context->token_found_2 = true;
        context->decimals_2 = msg->item2->token.decimals;
        strlcpy(context->ticker_2, (char *) msg->item2->token.ticker, sizeof(context->ticker_2));
    } else {
        context->token_found_2 = false;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}