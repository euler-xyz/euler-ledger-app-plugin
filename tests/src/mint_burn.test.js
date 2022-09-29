import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models} from './test.fixture';
import { ethers } from "ethers";
import { Euler } from "@eulerxyz/euler-sdk"

const DUMMY_UNDERLYING_ADDRESS = "0x0202020202020202020202020202020202020202"
const DUMMY_DEFERRED_ACCOUNT = "0x0404040404040404040404040404040404040404"

const sdk = new Euler()

nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Mint', zemu(model, async (sim, eth) => {
    const { data } = await sdk.contracts.exec.populateTransaction.batchDispatch(
      sdk.buildBatch([
        {
          contract: sdk.eToken(DUMMY_UNDERLYING_ADDRESS),
          method: "mint",
          args: [1, ethers.constants.MaxUint256]
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

    const right_clicks = model.letter === 'S' ? 8 : 6;

    // Wait for the application to actually load and parse the transaction
    await waitForAppScreen(sim);

    // Navigate the display by pressing the right button, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_mint', [right_clicks, 0]);

    await tx;
  }));
});

nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Burn', zemu(model, async (sim, eth) => {
    const { data } = await sdk.contracts.exec.populateTransaction.batchDispatch(
      sdk.buildBatch([
        {
          contract: sdk.eToken(DUMMY_UNDERLYING_ADDRESS),
          method: "burn",
          args: [5, ethers.constants.MaxUint256]
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

    const right_clicks = model.letter === 'S' ? 8 : 6;

    // Wait for the application to actually load and parse the transaction
    await waitForAppScreen(sim);

    // Navigate the display by pressing the right button, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_burn', [right_clicks, 0]);

    await tx;
  }));
});
