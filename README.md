# Euler Ledger App Plugin

This repository implements Ledger App Plugin for Euler which allows Ledger devices to display user friendly transaction summary for the most common Euler interactions, instead of blind signing.

[Euler](https://app.euler.finance/) is a non-custiodial permissionless lending protocol on Ethereum network.

# Documentation

For more information refer to the official Ledger [documenation](https://developers.ledger.com/docs/dapp/nano-plugin/overview/). 

# Tests

To run plugin tests, do the following:
1. Go through the [setup process](https://developers.ledger.com/docs/dapp/nano-plugin/environment-setup/)
2. Clone repositories into `plugin_dev` directory created in the previous step. Execute the following commands in the new terminal window pointing to `plugin_dev`:
```
git clone https://github.com/euler-xyz/euler-ledger-app-plugin.git
cd euler-ledger-app-plugin
git clone https://github.com/LedgerHQ/ethereum-plugin-sdk
```
3. Install tests dependencies using terminal from step 2.:
```
cd tests && yarn install
```
4. Build the plugin and the Ethereum app. Go back to your docker setup from step 1. and execute the following in the container:
```
cd ../euler-ledger-app-plugin/tests
./build_local_test_elfs.sh
```
5. Run tests. Use the terminal from step 2. and 3.
```
yarn test
```
