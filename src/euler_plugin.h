#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

// Name of the plugin.
#define PLUGIN_NAME "Euler"

// Number of selectors defined in this plugin. Should match the enum `selector_t`.
#define NUM_SELECTORS 17

// Max number of items in batch dispatch call
#define MAX_ITEMS_IN_BATCH 5

// Overrides the default length of the ticker string due to memory constraints
#define MAX_TICKER_LEN_OVERRIDE 7

// Casts the handler appropriately
#define HANDLER_CAST(handler) ((bool (*)(uint8_t *, uint8_t *))(handler))

// Standard offsets for EulerBatchItem to get proxy address and bytes length
#define BATCH_ITEM_PROXY_OFFSET  PARAMETER_LENGTH
#define BATCH_ITEM_LENGTH_OFFSET (PARAMETER_LENGTH * 2)

// Auxiliary lengths for swap and swap and repay parsing
#define SWAP_ITEM_LENGTH_WITHOUT_PAYLOAD_ALIGNED           (13 * 32)
#define SWAP_AND_REPAY_ITEM_LENGTH_WITHOUT_PAYLOAD_ALIGNED (14 * 32)

// List of selectors supported by this plugin.
#define ACTIVATE_MARKET_SELECTOR    ((uint32_t) 0x4e807177)
#define BATCH_DISPATCH_SELECTOR     ((uint32_t) 0xeb937aeb)
#define ENTER_MARKET_SELECTOR       ((uint32_t) 0x73f0b437)
#define EXIT_MARKET_SELECTOR        ((uint32_t) 0xc8a5fba3)
#define USE_PERMIT_SELECTOR         ((uint32_t) 0xa3e02273)
#define USE_PERMIT_ALLOWED_SELECTOR ((uint32_t) 0x0f5621e7)
#define USE_PERMIT_PACKED_SELECTOR  ((uint32_t) 0xbf143b34)
#define DEPOSIT_SELECTOR            ((uint32_t) 0xe2bbb158)
#define WITHDRAW_SELECTOR           ((uint32_t) 0x441a3e70)
#define BORROW_SELECTOR             ((uint32_t) 0x0ecbcdab)
#define REPAY_SELECTOR              ((uint32_t) 0xd8aed145)
#define MINT_SELECTOR               ((uint32_t) 0x1b2ef1ca)
#define BURN_SELECTOR               ((uint32_t) 0xb390c0ab)
#define TRANSFER_SELECTOR           ((uint32_t) 0xa9059cbb)
#define TRANSFER_FROM_SELECTOR      ((uint32_t) 0x23b872dd)
#define SWAP_SELECTOR               ((uint32_t) 0x25cad427)
#define SWAP_AND_REPAY_SELECTOR     ((uint32_t) 0xb0257235)

// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
typedef enum {
    ACTIVATE_MARKET = 0,
    BATCH_DISPATCH,
    ENTER_MARKET,
    EXIT_MARKET,
    USE_PERMIT,
    USE_PERMIT_ALLOWED,
    USE_PERMIT_PACKED,
    DEPOSIT,
    WITHDRAW,
    BORROW,
    REPAY,
    MINT,
    BURN,
    TRANSFER,
    TRANSFER_FROM,
    SWAP,
    SWAP_AND_REPAY,
    UNEXPECTED_ITEM
} selector_t;

// Enumeration used to parse the smart contract data.
typedef enum {
    ACTIVATE_MARKET_UNDERLYING = 0,
    BATCH_DISPATCH_ITEMS_OFFSET,
    BATCH_DISPATCH_NUMBER_OF_ITEMS,
    BATCH_DISPATCH_ITEMS_OFFSETS,
    BATCH_DISPATCH_ITEM_PROXY,
    BATCH_DISPATCH_ITEM_LENGTH,
    BATCH_DISPATCH_ITEM,
    UNEXPECTED_PARAMETER,
} parameter;

extern const uint32_t EULER_SELECTORS[NUM_SELECTORS];

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct context_t {
    // For display.
    bool is_max_exceeded;
    uint8_t batch_dispatch_number_of_items;
    selector_t item_types[MAX_ITEMS_IN_BATCH];
    uint8_t address_1[ADDRESS_LENGTH];
    uint8_t address_2[ADDRESS_LENGTH];
    uint8_t address_3[ADDRESS_LENGTH];
    uint8_t amount[INT256_LENGTH];
    uint8_t sub_account_1;
    uint8_t sub_account_2;
    uint8_t swap_mode;

    char ticker_1[MAX_TICKER_LEN_OVERRIDE];
    uint8_t decimals_1;
    uint8_t token_found_1;

    char ticker_2[MAX_TICKER_LEN_OVERRIDE];
    uint8_t decimals_2;
    uint8_t token_found_2;

    // For parsing data.
    bool go_to_offset;
    uint8_t next_param;
    uint8_t cnt;
    uint16_t offset;
    uint16_t base_items_offset;
    uint16_t items_offsets[MAX_ITEMS_IN_BATCH];
    uint16_t item_length;
    uint32_t item_selector;
    bool (*item_handler)(uint8_t *, uint8_t *);

    // For both parsing and display.
    selector_t selectorIndex;
} context_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);