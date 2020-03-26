# 2bpintegrity

A smart contract for decentralizing block producers' bp.json and signing BP agreements. 

Project is live on the [2bpintegrity account](https://wax.bloks.io/account/2bpintegrity).

> ⚠️ To sign the existing "infra" agreement, please add your system administrators' full names and Telegram handles as a "`system_admins`" key as part of the "`org`" field of your bp.json.

## Usage

### BP.json

Submitting `bp.json`:

```bash

cleos push action 2bpintegrity setbpjson '{"owner":"your_account", "json": "your_json"}' -p your_account@active
```

### Agreements

#### Creating an agreement:

```bash
cleos push action 2bpintegrity makeagreemnt '{"identifier":"agreement_identifier", "agreement": "your_agreement"}' -p your_account@active
```

#### Viewing & Signing an agreement

Agreements can be viewed in the [agreements table](https://wax.bloks.io/account/2bpintegrity?loadContract=true&tab=Tables&account=2bpintegrity&scope=2bpintegrity&limit=100&action=makeagreemnt).

```bash
cleos push action 2bpintegrity signagreemnt '{"owner":"your_account", "agreement_identifier": "agreement_identifier"}' -p your_account@active
```


# License

2bpintegrity is [MIT licensed](./LICENSE).
