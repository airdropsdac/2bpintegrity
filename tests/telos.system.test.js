const { loadConfig, Blockchain } = require("@klevoya/hydra");

const config = loadConfig("hydra.yml");
const bpJson = JSON.stringify(require(`./bp.json`), null, 2);

// #define TELOS in contract to build the contract for Telos
// Telos has a different producer_info structure
describe("telos.system", () => {
  let blockchain = new Blockchain(config);
  let eosio = blockchain.accounts.eosio;
  let tester = blockchain.createAccount(`bpintegrity`);
  let bpAccount = blockchain.createAccount(`maltablockbp`);

  beforeAll(async () => {
    tester.setContract(blockchain.contractTemplates[`bpintegrity`]);
    eosio.setContract(blockchain.contractTemplates[`telos.system`]);
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
  });

  beforeEach(async () => {
    tester.resetTables();
  });

  it("can register a TELOS bp producer format", async () => {
    expect.assertions(1);

    await eosio.contract.regproducer(
      {
        producer: bpAccount.accountName,
        producer_key: "EOS68qB2xe87B4zGU96KD4z9UY3BeYRHpkaytRnxA11494KBdHZh2",
        url: "https://null",
        location: 0,
      },
      [{ actor: bpAccount.accountName, permission: `active` }]
    );

    expect(eosio.getTableRowsScoped(`producers`)[`eosio`]).toEqual([
      {
        is_active: true,
        kick_penalty_hours: 0,
        kick_reason: "",
        kick_reason_id: 0,
        last_claim_time: "2000-01-01T00:00:00.000",
        last_time_kicked: "2000-01-01T00:00:00.000",
        lifetime_missed_blocks: 0,
        lifetime_produced_blocks: 0,
        location: 0,
        missed_blocks_per_rotation: 0,
        owner: "maltablockbp",
        producer_key:
          "PUB_K1_68qB2xe87B4zGU96KD4z9UY3BeYRHpkaytRnxA11494K7BkdRq",
        times_kicked: 0,
        total_votes: 0,
        unpaid_blocks: 0,
        unreg_reason: "",
        url: "https://null",
      },
    ]);
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
