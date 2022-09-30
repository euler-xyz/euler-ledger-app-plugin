#include "euler_plugin.h"

static bool handle_item(ethPluginProvideParameter_t *, context_t *);

static bool pre_handle(context_t *context) {
    // check if the item length is aligned
    if (context->item_length >= PARAMETER_LENGTH && context->item_length % PARAMETER_LENGTH == 0)
        return true;
        
    return false;
}

static bool post_handle(context_t *context) {
    context->item_length -= PARAMETER_LENGTH;

    // check if processing should end
    if (context->item_length < PARAMETER_LENGTH) return true;
    return false;
}

static bool handle_activate_market(ethPluginProvideParameter_t *msg, context_t *context) {
    copy_address(context->address_2, msg->parameter, sizeof(context->address_2));
}

static bool handle_enter_market(ethPluginProvideParameter_t *msg, context_t *context) {
    if (!pre_handle(context)) return true;

    uint8_t word = context->item_length / PARAMETER_LENGTH;
    if (word == 2) {
        context->sub_account_1 = (uint8_t) U2BE(msg->parameter, SELECTOR_SIZE / 2);
        copy_address(context->address_2,
                     msg->parameter,
                     sizeof(context->address_2) - SELECTOR_SIZE);
    } else if (word == 1) {
        copy_parameter(context->address_2 + sizeof(context->address_2) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
    }

    return post_handle(context);
}

static bool handle_exit_market(ethPluginProvideParameter_t *msg, context_t *context) {
    return handle_enter_market(msg, context);
}

static bool handle_use_permit(ethPluginProvideParameter_t *msg, context_t *context) {
    if (!pre_handle(context)) return true;

    uint8_t tmp[SELECTOR_SIZE];
    memset(tmp, 0, SELECTOR_SIZE);
    if (memcmp(context->address_2, tmp, SELECTOR_SIZE) == 0) {
        copy_address(context->address_2,
                     msg->parameter,
                     sizeof(context->address_2) - SELECTOR_SIZE);
    } else if (memcmp(context->address_2 + sizeof(context->address_2) - SELECTOR_SIZE,
                      tmp,
                      SELECTOR_SIZE) == 0) {
        copy_parameter(context->address_2 + sizeof(context->address_2) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
    } else {
        return true;
    }

    return post_handle(context);
}

static bool handle_use_permit_allowed(ethPluginProvideParameter_t *msg, context_t *context) {
    return handle_use_permit(msg, context);
}

static bool handle_use_permit_packed(ethPluginProvideParameter_t *msg, context_t *context) {
    return handle_use_permit(msg, context);
}

static bool handle_deposit(ethPluginProvideParameter_t *msg, context_t *context) {
    if (!pre_handle(context)) return true;

    uint8_t word = context->item_length / PARAMETER_LENGTH;
    if (word == 2) {
        memmove(context->address_2, context->address_1, sizeof(context->address_2));
        context->sub_account_1 = (uint8_t) U2BE(msg->parameter, SELECTOR_SIZE / 2);
        copy_parameter(context->amount,
                       msg->parameter + SELECTOR_SIZE,
                       sizeof(context->amount) - SELECTOR_SIZE);
    } else if (word == 1) {
        copy_parameter(context->amount + sizeof(context->amount) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
    }

    return post_handle(context);
}

static bool handle_withdraw(ethPluginProvideParameter_t *msg, context_t *context) {
    return handle_deposit(msg, context);
}

static bool handle_borrow(ethPluginProvideParameter_t *msg, context_t *context) {
    return handle_deposit(msg, context);
}

static bool handle_repay(ethPluginProvideParameter_t *msg, context_t *context) {
    return handle_deposit(msg, context);
}

static bool handle_mint(ethPluginProvideParameter_t *msg, context_t *context) {
    return handle_deposit(msg, context);
}

static bool handle_burn(ethPluginProvideParameter_t *msg, context_t *context) {
    return handle_deposit(msg, context);
}

static bool handle_transfer(ethPluginProvideParameter_t *msg, context_t *context) {
    if (!pre_handle(context)) return true;

    uint8_t word = context->item_length / PARAMETER_LENGTH;
    if (word == 3) {
        memmove(context->address_2, context->address_1, sizeof(context->address_2));
        copy_address(context->address_3,
                     msg->parameter,
                     sizeof(context->address_3) - SELECTOR_SIZE);
    } else if (word == 2) {
        copy_parameter(context->address_3 + sizeof(context->address_3) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
        copy_parameter(context->amount,
                       msg->parameter + SELECTOR_SIZE,
                       sizeof(context->amount) - SELECTOR_SIZE);
    } else if (word == 1) {
        copy_parameter(context->amount + sizeof(context->amount) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
    }
    return post_handle(context);
}

static bool handle_transfer_from(ethPluginProvideParameter_t *msg, context_t *context) {
    if (!pre_handle(context)) return true;

    uint8_t word = context->item_length / PARAMETER_LENGTH;
    if (word == 4) {
        memmove(context->address_2, context->address_1, sizeof(context->address_2));
        copy_address(context->address_1,
                     msg->parameter,
                     sizeof(context->address_1) - SELECTOR_SIZE);
    } else if (word == 3) {
        copy_parameter(context->address_1 + sizeof(context->address_1) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
        copy_address(context->address_3,
                     msg->parameter,
                     sizeof(context->address_3) - SELECTOR_SIZE);
    } else if (word == 2) {
        copy_parameter(context->address_3 + sizeof(context->address_3) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
        copy_parameter(context->amount,
                       msg->parameter + SELECTOR_SIZE,
                       sizeof(context->amount) - SELECTOR_SIZE);
    } else if (word == 1) {
        copy_parameter(context->amount + sizeof(context->amount) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
    }
    return post_handle(context);
}

static bool handle_swap(ethPluginProvideParameter_t *msg, context_t *context) {
    if (!pre_handle(context)) return true;

    uint8_t word = context->item_length / PARAMETER_LENGTH;
    if (word == 12) {
        context->sub_account_1 = (uint8_t) U2BE(msg->parameter, SELECTOR_SIZE / 2);
    } else if (word == 11) {
        context->sub_account_2 = (uint8_t) U2BE(msg->parameter, SELECTOR_SIZE / 2);
    } else if (word == 9) {
        copy_address(context->address_2,
                     msg->parameter,
                     sizeof(context->address_2) - SELECTOR_SIZE);
    } else if (word == 8) {
        copy_parameter(context->address_2 + sizeof(context->address_2) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
        copy_address(context->address_3,
                     msg->parameter,
                     sizeof(context->address_3) - SELECTOR_SIZE);
    } else if (word == 7) {
        copy_parameter(context->address_3 + sizeof(context->address_3) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
    } else if (word == 6) {
        context->swap_mode = (uint8_t) U2BE(msg->parameter, SELECTOR_SIZE / 2);

        if (context->swap_mode != 0) {
            copy_parameter(context->amount,
                           msg->parameter + SELECTOR_SIZE,
                           sizeof(context->amount) - SELECTOR_SIZE);
        }
    } else if (word == 5) {
        if (context->swap_mode != 0) {
            copy_parameter(context->amount + sizeof(context->amount) - SELECTOR_SIZE,
                           msg->parameter,
                           SELECTOR_SIZE);
        } else {
            copy_parameter(context->amount,
                           msg->parameter + SELECTOR_SIZE,
                           sizeof(context->amount) - SELECTOR_SIZE);
        }
    } else if (word == 4) {
        if (context->swap_mode == 0) {
            copy_parameter(context->amount + sizeof(context->amount) - SELECTOR_SIZE,
                           msg->parameter,
                           SELECTOR_SIZE);
        }
    }

    return post_handle(context);
}

static bool handle_swap_and_repay(ethPluginProvideParameter_t *msg, context_t *context) {
    if (!pre_handle(context)) return true;

    uint8_t word = context->item_length / PARAMETER_LENGTH;
    if (word == 13) {
        context->sub_account_1 = (uint8_t) U2BE(msg->parameter, SELECTOR_SIZE / 2);
    } else if (word == 12) {
        context->sub_account_2 = (uint8_t) U2BE(msg->parameter, SELECTOR_SIZE / 2);
    } else if (word == 9) {
        copy_address(context->address_2,
                     msg->parameter,
                     sizeof(context->address_2) - SELECTOR_SIZE);
    } else if (word == 8) {
        copy_parameter(context->address_2 + sizeof(context->address_2) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
        copy_address(context->address_3,
                     msg->parameter,
                     sizeof(context->address_3) - SELECTOR_SIZE);
    } else if (word == 7) {
        copy_parameter(context->address_3 + sizeof(context->address_3) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
    } else if (word == 6) {
        copy_parameter(context->amount,
                       msg->parameter + SELECTOR_SIZE,
                       sizeof(context->amount) - SELECTOR_SIZE);
    } else if (word == 5) {
        copy_parameter(context->amount + sizeof(context->amount) - SELECTOR_SIZE,
                       msg->parameter,
                       SELECTOR_SIZE);
    }

    return post_handle(context);
}

static void handle_batch_dispatch(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        uint32_t current = msg->parameterOffset;
        uint32_t goal = context->offset + SELECTOR_SIZE;

        if (current < goal) {
            return;
        } else if (current != goal) {
            PRINTF("Go to offset error: %d | %d\n", current, goal);
            context->next_param = UNEXPECTED_PARAMETER;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case BATCH_DISPATCH_ITEMS_OFFSET:
            context->offset = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->go_to_offset = true;
            context->next_param = BATCH_DISPATCH_NUMBER_OF_ITEMS;
            break;
        case BATCH_DISPATCH_NUMBER_OF_ITEMS:
            context->batch_dispatch_number_of_items =
                (uint8_t) U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->cnt = context->batch_dispatch_number_of_items;
            context->is_max_exceeded = false;
            context->next_param = BATCH_DISPATCH_ITEMS_OFFSETS;
            break;
        case BATCH_DISPATCH_ITEMS_OFFSETS:
            if (context->batch_dispatch_number_of_items == 1) {
                context->offset = msg->parameterOffset - SELECTOR_SIZE +
                                  U2BE(msg->parameter, PARAMETER_LENGTH - 2) +
                                  BATCH_ITEM_PROXY_OFFSET;
                context->go_to_offset = true;
                context->next_param = BATCH_DISPATCH_ITEM_PROXY;
            } else {
                if (context->batch_dispatch_number_of_items == context->cnt) {
                    context->base_items_offset = msg->parameterOffset;
                }

                uint8_t offset_index = context->batch_dispatch_number_of_items - context->cnt;
                if (offset_index < MAX_ITEMS_IN_BATCH) {
                    context->items_offsets[offset_index] =
                        U2BE(msg->parameter, PARAMETER_LENGTH - 2);
                } else {
                    context->is_max_exceeded = true;
                }

                context->cnt--;
                if (context->cnt == 0) {
                    context->cnt = context->batch_dispatch_number_of_items;
                    context->offset = msg->parameterOffset - SELECTOR_SIZE + PARAMETER_LENGTH +
                                      BATCH_ITEM_PROXY_OFFSET;
                    context->go_to_offset = true;
                    context->next_param = BATCH_DISPATCH_ITEM_PROXY;
                }
            }
            break;
        case BATCH_DISPATCH_ITEM_PROXY:
            copy_address(context->address_1, msg->parameter, sizeof(context->address_1));
            context->offset = msg->parameterOffset - SELECTOR_SIZE + BATCH_ITEM_LENGTH_OFFSET;
            context->go_to_offset = true;
            context->next_param = BATCH_DISPATCH_ITEM_LENGTH;
            break;
        case BATCH_DISPATCH_ITEM_LENGTH:
            context->item_length = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->next_param = BATCH_DISPATCH_ITEM;
            break;
        case BATCH_DISPATCH_ITEM:
            if (handle_item(msg, context)) {
                context->cnt--;
                uint8_t offset_index = context->batch_dispatch_number_of_items - context->cnt;
                if (context->cnt == 0 || offset_index >= MAX_ITEMS_IN_BATCH) {
                    context->next_param = UNEXPECTED_PARAMETER;
                } else {
                    context->offset = context->base_items_offset - SELECTOR_SIZE +
                                      context->items_offsets[offset_index] +
                                      BATCH_ITEM_PROXY_OFFSET;
                    context->go_to_offset = true;
                    context->next_param = BATCH_DISPATCH_ITEM_PROXY;
                }
            } else {
                context->next_param = BATCH_DISPATCH_ITEM;
            }
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            break;
    }
}

static bool handle_item(ethPluginProvideParameter_t *msg, context_t *context) {
    // only when beginning to handle new item
    if (context->item_selector == 0 && context->item_handler == NULL) {
        context->item_selector = (U2BE(msg->parameter, 0) << 16) | U2BE(msg->parameter, 2);
    }

    uint8_t index = context->batch_dispatch_number_of_items - context->cnt;
    switch (context->item_selector) {
        case ENTER_MARKET_SELECTOR:
            context->item_types[index] = ENTER_MARKET;
            context->item_handler = HANDLER_CAST(handle_enter_market);
            break;
        case EXIT_MARKET_SELECTOR:
            context->item_types[index] = EXIT_MARKET;
            context->item_handler = HANDLER_CAST(handle_exit_market);
            break;
        case USE_PERMIT_SELECTOR:
            context->item_types[index] = USE_PERMIT;
            context->item_handler = HANDLER_CAST(handle_use_permit);
            break;
        case USE_PERMIT_ALLOWED_SELECTOR:
            context->item_types[index] = USE_PERMIT_ALLOWED;
            context->item_handler = HANDLER_CAST(handle_use_permit_allowed);
            break;
        case USE_PERMIT_PACKED_SELECTOR:
            context->item_types[index] = USE_PERMIT_PACKED;
            context->item_handler = HANDLER_CAST(handle_use_permit_packed);
            break;
        case DEPOSIT_SELECTOR:
            context->item_types[index] = DEPOSIT;
            context->item_handler = HANDLER_CAST(handle_deposit);
            break;
        case WITHDRAW_SELECTOR:
            context->item_types[index] = WITHDRAW;
            context->item_handler = HANDLER_CAST(handle_withdraw);
            break;
        case BORROW_SELECTOR:
            context->item_types[index] = BORROW;
            context->item_handler = HANDLER_CAST(handle_borrow);
            break;
        case REPAY_SELECTOR:
            context->item_types[index] = REPAY;
            context->item_handler = HANDLER_CAST(handle_repay);
            break;
        case MINT_SELECTOR:
            context->item_types[index] = MINT;
            context->item_handler = HANDLER_CAST(handle_mint);
            break;
        case BURN_SELECTOR:
            context->item_types[index] = BURN;
            context->item_handler = HANDLER_CAST(handle_burn);
            break;
        case TRANSFER_SELECTOR:
            context->item_types[index] = TRANSFER;
            context->item_handler = HANDLER_CAST(handle_transfer);
            break;
        case TRANSFER_FROM_SELECTOR:
            context->item_types[index] = TRANSFER_FROM;
            context->item_handler = HANDLER_CAST(handle_transfer_from);
            break;
        case SWAP_SELECTOR:
            context->item_types[index] = SWAP;
            context->item_handler = HANDLER_CAST(handle_swap);
            break;
        case SWAP_AND_REPAY_SELECTOR:
            context->item_types[index] = SWAP_AND_REPAY;
            context->item_handler = HANDLER_CAST(handle_swap_and_repay);
            break;
        default:
            context->item_types[index] = UNEXPECTED_ITEM;
            break;
    }

    bool result = true;
    if (context->batch_dispatch_number_of_items == 1 &&
        context->item_types[index] != UNEXPECTED_ITEM && context->item_length > SELECTOR_SIZE &&
        context->item_handler != NULL) {
        // processing first word of the item
        if (context->item_length >= PARAMETER_LENGTH &&
            context->item_length % PARAMETER_LENGTH == SELECTOR_SIZE) {
            context->item_length -= SELECTOR_SIZE;
            context->item_length += PARAMETER_LENGTH;

            // assign hardcoded length as the swap payload will not be parsed (while having unknown
            // length)
            if (context->item_selector == SWAP_SELECTOR) {
                context->item_length = SWAP_ITEM_LENGTH_WITHOUT_PAYLOAD_ALIGNED;
            } else if (context->item_selector == SWAP_AND_REPAY_SELECTOR) {
                context->item_length = SWAP_AND_REPAY_ITEM_LENGTH_WITHOUT_PAYLOAD_ALIGNED;
            }
        }

        // execute the handler. if all data processed, expect the handler to return true
        result = context->item_handler((uint8_t *) msg, (uint8_t *) context);
    }

    if (result) {
        context->item_selector = 0;
        context->item_handler = NULL;
    }

    return result;
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case ACTIVATE_MARKET:
            handle_activate_market(msg, context);
            break;
        case BATCH_DISPATCH:
            handle_batch_dispatch(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}