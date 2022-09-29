import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models} from './test.fixture';
import { ethers } from "ethers";
import { Euler } from "@eulerxyz/euler-sdk"

const DUMMY_SWAP_HANDLER_ADDRESS = "0x0101010101010101010101010101010101010101"
const DUMMY_UNDERLYING_IN_ADDRESS = "0x0202020202020202020202020202020202020202"
const DUMMY_UNDERLYING_OUT_ADDRESS = "0x0303030303030303030303030303030303030303"
const DUMMY_DEFERRED_ACCOUNT = "0x0404040404040404040404040404040404040404"
const DUMMY_PAYLOAD = "0x0000000000000000000000000000000000000000000000000000000000000000"

const sdk = new Euler()

nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Exact In Swap', zemu(model, async (sim, eth) => {
    const { data } = await sdk.contracts.exec.populateTransaction.batchDispatch(
      sdk.buildBatch([
        {
          contract: "swapHub",
          method: "swap",
          args: [
            1,
            2,
            DUMMY_SWAP_HANDLER_ADDRESS,
            {
              underlyingIn: DUMMY_UNDERLYING_IN_ADDRESS,
              underlyingOut: DUMMY_UNDERLYING_OUT_ADDRESS,
              amountIn: ethers.utils.parseEther('30'),
              amountOut: ethers.utils.parseEther('40'),
              mode: 0,
              exactOutTolerance: 0,
              payload: DUMMY_PAYLOAD,
            },
          ]
        }
      ]),
      [ DUMMY_DEFERRED_ACCOUNT ]
    )
  
    // Get the generic transaction template
    const unsignedTx = genericTx;
    unsignedTx.to = sdk.contracts.exec.address;
    unsignedTx.data = data;

    // Create serializedTx and remove the "0x" prefix
    const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

    const right_clicks = model.letter === 'S' ? 12 : 8;

    // Wait for the application to actually load and parse the transaction
    await waitForAppScreen(sim);

    // Navigate the display by pressing the right button, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_exact_in_swap', [right_clicks, 0]);

    await tx;
  }));
});

nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Exact Out Swap', zemu(model, async (sim, eth) => {
    const { data } = await sdk.contracts.exec.populateTransaction.batchDispatch(
      sdk.buildBatch([
        {
          contract: "swapHub",
          method: "swap",
          args: [
            2,
            2,
            DUMMY_SWAP_HANDLER_ADDRESS,
            {
              underlyingIn: DUMMY_UNDERLYING_IN_ADDRESS,
              underlyingOut: DUMMY_UNDERLYING_OUT_ADDRESS,
              amountIn: ethers.utils.parseEther('25'),
              amountOut: ethers.utils.parseEther('35'),
              mode: 1,
              exactOutTolerance: 0,
              payload: DUMMY_PAYLOAD,
            },
          ]
        }
      ]),
      [ DUMMY_DEFERRED_ACCOUNT ]
    )
  
    // Get the generic transaction template
    const unsignedTx = genericTx;
    unsignedTx.to = sdk.contracts.exec.address;
    unsignedTx.data = data;

    // Create serializedTx and remove the "0x" prefix
    const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

    const right_clicks = model.letter === 'S' ? 12 : 8;

    // Wait for the application to actually load and parse the transaction
    await waitForAppScreen(sim);

    // Navigate the display by pressing the right button, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_exact_out_swap', [right_clicks, 0]);

    await tx;
  }));
});

nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Swap And Repay', zemu(model, async (sim, eth) => {
    const { data } = await sdk.contracts.exec.populateTransaction.batchDispatch(
      sdk.buildBatch([
        {
          contract: "swapHub",
          method: "swapAndRepay",
          args: [
            1,
            0,
            DUMMY_SWAP_HANDLER_ADDRESS,
            {
              underlyingIn: DUMMY_UNDERLYING_IN_ADDRESS,
              underlyingOut: DUMMY_UNDERLYING_OUT_ADDRESS,
              amountIn: ethers.utils.parseEther('5'),
              amountOut: ethers.utils.parseEther('1'),
              mode: 1,
              exactOutTolerance: 0,
              payload: DUMMY_PAYLOAD,
            },
            0,
          ]
        }
      ]),
      [ DUMMY_DEFERRED_ACCOUNT ]
    )
  
    // Get the generic transaction template
    const unsignedTx = genericTx;
    unsignedTx.to = sdk.contracts.exec.address;
    unsignedTx.data = data;

    // Create serializedTx and remove the "0x" prefix
    const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

    const right_clicks = model.letter === 'S' ? 12 : 8;

    // Wait for the application to actually load and parse the transaction
    await waitForAppScreen(sim);

    // Navigate the display by pressing the right button, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_swap_and_repay', [right_clicks, 0]);

    await tx;
  }));
});
