const { loadConfig, Blockchain } = require("@klevoya/hydra");

const config = loadConfig("hydra.yml");
const bpJson = JSON.stringify(require(`./bp.json`), null, 2);

describe("bpintegrity", () => {
  let blockchain = new Blockchain(config);
  let eosio = blockchain.accounts.eosio;
  let tester = blockchain.createAccount(`bpintegrity`);
  let bpAccount = blockchain.createAccount(`maltablockbp`);

  beforeAll(async () => {
    tester.setContract(blockchain.contractTemplates[`bpintegrity`]);
    eosio.setContract(blockchain.contractTemplates[`eosio.system`]);
    tester.updateAuth(`active`, `owner`, {
      accounts: [
        {
          permission: {
            actor: tester.accountName,
            permission: `eosio.code`,
          },
          weight: 1,
        },
      ],
    });

    await eosio.loadFixtures();
  });

  beforeEach(async () => {
    tester.resetTables();
  });

  it("can create, sign agreements", async () => {
    expect.assertions(3);

    const agreementText = `We confirm that we are not sharing our BP infrastructure or our System Administrator with any other Block Producers.`;
    await tester.contract.makeagreemnt({
      identifier: "infra",
      agreement: agreementText,
    });

    expect(tester.getTableRowsScoped(`agreements`)[`bpintegrity`]).toEqual([
      {
        identifier: `infra`,
        text: agreementText,
      },
    ]);

    await tester.contract.signagreemnt(
      {
        owner: bpAccount.accountName,
        agreement_identifier: "infra",
      },
      [{ actor: bpAccount.accountName, permission: `active` }]
    );

    expect(tester.getTableRowsScoped(`signatures`)).toEqual({
      [tester.accountName]: [
        {
          id: "0",
          signer: `maltablockbp`,
          agreement_id: "infra",
          updated_at: "2000-01-01T00:00:00.000",
          next_update: "2000-06-29T00:00:00.000",
        },
      ],
    });

    // try signing it again
    await tester.contract.signagreemnt(
      {
        owner: bpAccount.accountName,
        agreement_identifier: "infra",
      },
      [{ actor: bpAccount.accountName, permission: `active` }]
    );

    expect(
      tester.getTableRowsScoped(`signatures`)[tester.accountName].length
    ).toBe(1);
  });

  it("can submit bp.json", async () => {
    expect.assertions(1);

    await tester.contract.setbpjson(
      {
        owner: bpAccount.accountName,
        json: bpJson,
      },
      [{ actor: bpAccount.accountName, permission: `active` }]
    );

    expect(
      tester.getTableRowsScoped(`sysadminjson`)[tester.accountName]
    ).toEqual([
      {
        owner: `maltablockbp`,
        json: bpJson,
      },
    ]);
  });
});
