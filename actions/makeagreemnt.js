const initEnvironment = require(`eosiac`);

const { sendTransaction, env } = initEnvironment(
  process.env.EOSIAC_ENV || `wax`,
  { verbose: true }
);

const accounts = Object.keys(env.accounts);

const CONTRACT_ACCOUNT = accounts[0];

async function action() {
  try {
    await sendTransaction({
      account: CONTRACT_ACCOUNT,
      name: `makeagreemnt`,
      authorization: [
        {
          actor: CONTRACT_ACCOUNT,
          permission: `active`
        }
      ],
      data: {
        identifier: "infra",
        agreement: "We confirm that we are not sharing our BP infrastructure or our System Administrator with any other Block Producers."
      }
    });
    process.exit(0);
  } catch (error) {
    // ignore
    process.exit(1);
  }
}

action();
