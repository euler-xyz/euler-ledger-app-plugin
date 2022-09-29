import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, nano_models, txFromEtherscan} from './test.fixture';

// Test from replayed transaction: https://etherscan.io/tx/0x128574272b63cca8a7ef086568d90daf76b50444be113f207bae3c151aaff2c7
nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Activate Market', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x128574272b63cca8a7ef086568d90daf76b50444be113f207bae3c151aaff2c7
  const serializedTx = txFromEtherscan("0x02f8910117850165a0bc00852c3ce1ec0083089cb7943520d5a913427e6f0d6a83e07ccd4a4da316e4d380a44e807177000000000000000000000000a0b86991c6218b36c1d19d4a2e9eb0ce3606eb48c080a03559c89e27f0cde7274597365943bfe92c50ac073989454ffdf81a69bfc78597a0078bbfb0bc142c23161dc44257a535bb49cacf89ec8054ebe4395a3a27a9d8ac");

  const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

  const right_clicks = 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);

  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_activate_market', [right_clicks, 0]);

  await tx;
  }));
});
