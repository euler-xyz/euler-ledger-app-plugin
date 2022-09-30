#include "euler_plugin.h"

/**
 * Ansi C "itoa" based on Kernighan & Ritchie's "Ansi C":
 */

void strreverse(char *begin, char *end) {
    char aux;
    while (end > begin) {
        aux = *end;
        *end-- = *begin;
        *begin++ = aux;
    }
}

void itoa(int value, char *str, int base) {
    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char *wstr = str;
    int sign;

    // Validate base
    if (base < 2 || base > 35) {
        *wstr = '\0';
        return;
    }

    // Take care of sign
    if ((sign = value) < 0) {
        value = -value;
    }

    // Conversion. Number is reversed.
    do {
        *wstr++ = num[value % base];
    } while (value /= base);

    if (sign < 0) {
        *wstr++ = '-';
    }

    *wstr = '\0';

    // Reverse string
    strreverse(str, wstr - 1);
}

static const char *item_type_to_string(selector_t type) {
    switch (type) {
        case ACTIVATE_MARKET:
            return "Activate Market";
        case ENTER_MARKET:
            return "Enter Market";
        case EXIT_MARKET:
            return "Exit Market";
        case USE_PERMIT:
        case USE_PERMIT_ALLOWED:
        case USE_PERMIT_PACKED:
            return "Permit";
        case DEPOSIT:
            return "Deposit";
        case WITHDRAW:
            return "Withdraw";
        case BORROW:
            return "Borrow";
        case REPAY:
            return "Repay";
        case MINT:
            return "Mint";
        case BURN:
            return "Burn";
        case TRANSFER:
            return "Transfer";
        case TRANSFER_FROM:
            return "Transfer From";
        case SWAP:
            return "Swap";
        case SWAP_AND_REPAY:
            return "Swap and Repay";
        default:
            return "Unknown Item";
    }
}

// Set UI for the summary screen.
static void set_summary_ui(ethQueryContractUI_t *msg, const context_t *context) {
    char str[10];
    memset(str, 0, 10);
    itoa(context->batch_dispatch_number_of_items, str, 10);
    strcat(str, " items");
    strlcpy(msg->title, str, msg->titleLength);
    strlcpy(msg->msg, "in a batch", msg->msgLength);
}

static void set_item_types_ui(ethQueryContractUI_t *msg, const context_t *context) {
    uint16_t index = (msg->screenIndex - 1) * 2;
    char str[20];
    memset(str, 0, 20);

    if (index < MAX_ITEMS_IN_BATCH) {
        itoa(index + 1, str, 10);
        strcat(str, ". ");
        strcat(str, item_type_to_string(context->item_types[index]));
        strlcpy(msg->title, str, msg->titleLength);
    } else if (context->is_max_exceeded) {
        char tmp[5];
        memset(tmp, 0, 5);
        itoa(context->batch_dispatch_number_of_items - MAX_ITEMS_IN_BATCH, tmp, 10);
        strcpy(str, "and ");
        strcat(str, tmp);
        strcat(str, " more");
        strlcpy(msg->title, str, msg->titleLength);
        return;
    } else {
        PRINTF("Received an invalid screenIndex 1\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    index++;
    memset(str, 0, 20);

    if (index < MAX_ITEMS_IN_BATCH) {
        itoa(index + 1, str, 10);
        strcat(str, ". ");
        strcat(str, item_type_to_string(context->item_types[index]));
        strlcpy(msg->msg, str, msg->msgLength);
    } else if (context->is_max_exceeded) {
        char tmp[5];
        memset(tmp, 0, 5);
        itoa(context->batch_dispatch_number_of_items - MAX_ITEMS_IN_BATCH, tmp, 10);
        strcpy(str, "and ");
        strcat(str, tmp);
        strcat(str, " more");
        strlcpy(msg->msg, str, msg->msgLength);
    } else {
        PRINTF("Received an invalid screenIndex 2\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }
}

static void set_batch_ui(ethQueryContractUI_t *msg, const context_t *context) {
    if (msg->screenIndex == 0) {
        set_summary_ui(msg, context);
    } else {
        set_item_types_ui(msg, context);
    }
}

static void set_token_1_ui(ethQueryContractUI_t *msg, const context_t *context, char *title) {
    strlcpy(msg->title, title, msg->titleLength);

    if (context->token_found_1) {
        strlcat(msg->msg, context->ticker_1, msg->msgLength);
    } else {
        strlcpy(msg->msg, "0x", msg->msgLength);
        getEthAddressStringFromBinary((uint8_t *) context->address_2,
                                      msg->msg + 2,
                                      msg->pluginSharedRW->sha3,
                                      0);
    }
}

static void set_token_2_ui(ethQueryContractUI_t *msg, const context_t *context, char *title) {
    strlcpy(msg->title, title, msg->titleLength);

    if (context->token_found_2) {
        strlcat(msg->msg, context->ticker_2, msg->msgLength);
    } else {
        strlcpy(msg->msg, "0x", msg->msgLength);
        getEthAddressStringFromBinary((uint8_t *) context->address_3,
                                      msg->msg + 2,
                                      msg->pluginSharedRW->sha3,
                                      0);
    }
}

static void set_address_3_ui(ethQueryContractUI_t *msg, const context_t *context, char *title) {
    strlcpy(msg->title, title, msg->titleLength);
    strlcpy(msg->msg, "0x", msg->msgLength);
    getEthAddressStringFromBinary((uint8_t *) context->address_3,
                                  msg->msg + 2,
                                  msg->pluginSharedRW->sha3,
                                  0);
}

static void set_address_1_ui(ethQueryContractUI_t *msg, const context_t *context, char *title) {
    strlcpy(msg->title, title, msg->titleLength);
    strlcpy(msg->msg, "0x", msg->msgLength);
    getEthAddressStringFromBinary((uint8_t *) context->address_1,
                                  msg->msg + 2,
                                  msg->pluginSharedRW->sha3,
                                  0);
}

static void set_sub_account_1_ui(ethQueryContractUI_t *msg,
                                 const context_t *context,
                                 char *prefix) {
    char str[10];
    memset(str, 0, 10);
    itoa(context->sub_account_1, str, 10);

    strlcpy(msg->msg, prefix, msg->msgLength);
    strlcat(msg->msg, str, msg->msgLength);
}

static void set_sub_account_2_ui(ethQueryContractUI_t *msg,
                                 const context_t *context,
                                 char *prefix) {
    char str[10];
    memset(str, 0, 10);
    itoa(context->sub_account_2, str, 10);

    strlcpy(msg->title, prefix, msg->titleLength);
    strlcat(msg->title, str, msg->titleLength);
}

static void set_token_1_amount_ui(ethQueryContractUI_t *msg,
                                  const context_t *context,
                                  char *title) {
    uint8_t max_amount[INT256_LENGTH];
    memset(max_amount, 0xff, INT256_LENGTH);

    strlcpy(msg->title, title, msg->titleLength);
    if (memcmp(context->amount, max_amount, sizeof(context->amount)) == 0) {
        strlcpy(msg->msg, "max", msg->msgLength);
    } else {
        amountToString(
            context->amount,
            sizeof(context->amount),
            context->token_found_1 ? context->decimals_1 : 18,
            context->token_found_1 ? "" : "",  //"(?) ", // (?) might be even more confusing
            msg->msg,
            msg->msgLength);
    }
}

static void set_token_2_amount_ui(ethQueryContractUI_t *msg,
                                  const context_t *context,
                                  char *title) {
    uint8_t max_amount[INT256_LENGTH];
    memset(max_amount, 0xff, INT256_LENGTH);

    strlcpy(msg->title, title, msg->titleLength);
    if (memcmp(context->amount, max_amount, sizeof(context->amount)) == 0) {
        strlcpy(msg->msg, "max", msg->msgLength);
    } else {
        amountToString(
            context->amount,
            sizeof(context->amount),
            context->token_found_2 ? context->decimals_2 : 18,
            context->token_found_2 ? "" : "",  //"(?) ", // (?) might be even more confusing
            msg->msg,
            msg->msgLength);
    }
}

static void set_swap_mode_ui(ethQueryContractUI_t *msg, const context_t *context) {
    if (context->swap_mode == 0) {
        strlcat(msg->msg, "Exact In", msg->msgLength);
    } else {
        strlcat(msg->msg, "Exact Out", msg->msgLength);
    }
}

static void set_item_ui(ethQueryContractUI_t *msg, const context_t *context) {
    selector_t item_type = context->item_types[0];
    switch (item_type) {
        case ENTER_MARKET:
        case EXIT_MARKET:
        case USE_PERMIT:
        case USE_PERMIT_ALLOWED:
        case USE_PERMIT_PACKED:
            if (msg->screenIndex == 0) {
                strlcpy(msg->title, item_type_to_string(item_type), msg->titleLength);
            } else {
                set_token_1_ui(msg, context, "Token: ");
            }
            break;
        case DEPOSIT:
        case WITHDRAW:
        case BORROW:
        case REPAY:
        case MINT:
        case BURN:
            if (msg->screenIndex == 0) {
                strlcpy(msg->title, item_type_to_string(item_type), msg->titleLength);
                set_sub_account_1_ui(msg, context, "Sub-account: ");
            } else if (msg->screenIndex == 1) {
                set_token_1_ui(msg, context, "Token: ");
            } else {
                set_token_1_amount_ui(msg, context, "Amount: ");
            }
            break;
        case TRANSFER:
            if (msg->screenIndex == 0) {
                strlcpy(msg->title, item_type_to_string(item_type), msg->titleLength);
            } else if (msg->screenIndex == 1) {
                set_token_1_ui(msg, context, "Token: ");
            } else if (msg->screenIndex == 2) {
                set_token_1_amount_ui(msg, context, "Amount: ");
            } else {
                set_address_3_ui(msg, context, "To: ");
            }
            break;
        case TRANSFER_FROM:
            if (msg->screenIndex == 0) {
                strlcpy(msg->title, item_type_to_string(item_type), msg->titleLength);
            } else if (msg->screenIndex == 1) {
                set_token_1_ui(msg, context, "Token: ");
            } else if (msg->screenIndex == 2) {
                set_token_1_amount_ui(msg, context, "Amount: ");
            } else if (msg->screenIndex == 3) {
                set_address_3_ui(msg, context, "To: ");
            } else {
                set_address_1_ui(msg, context, "From: ");
            }
            break;
        case SWAP:
            if (msg->screenIndex == 0) {
                strlcpy(msg->title, item_type_to_string(item_type), msg->titleLength);
                set_swap_mode_ui(msg, context);
            } else if (msg->screenIndex == 1) {
                set_sub_account_2_ui(msg, context, "To sub-account: ");
                set_sub_account_1_ui(msg, context, "From sub-account: ");
            } else if (msg->screenIndex == 2) {
                set_token_2_ui(msg, context, "Token out: ");
            } else if (msg->screenIndex == 3) {
                set_token_1_ui(msg, context, "Token in: ");
            } else {
                if (context->swap_mode == 0) {
                    set_token_2_amount_ui(msg, context, "Min. received: ");
                } else {
                    set_token_1_amount_ui(msg, context, "Max swapped: ");
                }
            }
            break;
        case SWAP_AND_REPAY:
            if (msg->screenIndex == 0) {
                strlcpy(msg->title, item_type_to_string(item_type), msg->titleLength);
            } else if (msg->screenIndex == 1) {
                set_sub_account_2_ui(msg, context, "Repay sub-account: ");
                set_sub_account_1_ui(msg, context, "From sub-account: ");
            } else if (msg->screenIndex == 2) {
                set_token_2_ui(msg, context, "Repay asset: ");
            } else if (msg->screenIndex == 3) {
                set_token_1_ui(msg, context, "Token in: ");
            } else {
                set_token_1_amount_ui(msg, context, "Max swapped: ");
            }
            break;
        default:
            strlcpy(msg->title, item_type_to_string(item_type), msg->titleLength);
            break;
    }
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case ACTIVATE_MARKET:
            strlcpy(msg->title, item_type_to_string(ACTIVATE_MARKET), msg->titleLength);
            set_token_1_ui(msg, context, "Token: ");
            break;
        case BATCH_DISPATCH:
            if (context->batch_dispatch_number_of_items == 1) {
                set_item_ui(msg, context);
            } else {
                set_batch_ui(msg, context);
            }
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
