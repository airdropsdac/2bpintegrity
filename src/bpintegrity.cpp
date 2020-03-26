#include <bpintegrity.hpp>

ACTION bpintegrity::setbpjson(name owner, const eosio::string &json) {
  require_auth(owner);
  check_is_bp(owner);

  check(json[0] == '{' && json[json.size() - 1] == '}', "payload must be json");

  auto itr = sysadminjson_table.find(owner.value);
  if (itr == sysadminjson_table.end()) {
    sysadminjson_table.emplace(owner, [&](auto &x) {
      x.owner = owner;
      x.json = json;
    });
  } else {
    sysadminjson_table.modify(itr, eosio::same_payer,
                              [&](auto &x) { x.json = json; });
  }
}

ACTION bpintegrity::signagreemnt(name owner, name agreement_identifier) {
  require_auth(owner);
  check_is_bp(owner);

  auto itr = agreements_table.find(agreement_identifier.value);
  check(itr != agreements_table.end(),
        "agreement with this identifier does not exist.");

  microseconds duration = seconds(AGREEMENT_DURATION);
  time_point_sec now = current_time_point();

  signatures_t signatures_table(get_self(), get_self().value);
  auto signatures_by_agreementsigner =
      signatures_table.get_index<"byagrsigner"_n>();
  auto signature_itr = signatures_by_agreementsigner.find(
      signatures_s::make_agreement_signer(agreement_identifier, owner));
  if (signature_itr == signatures_by_agreementsigner.end()) {
    signatures_table.emplace(owner, [&](auto &x) {
      x.id = signatures_table.available_primary_key();
      x.signer = owner;
      x.agreement_id = agreement_identifier;
      x.updated_at = now;
      x.next_update = now + duration;
    });
  } else {
    signatures_by_agreementsigner.modify(signature_itr, eosio::same_payer,
                                         [&](auto &x) {
                                           x.updated_at = now;
                                           x.next_update = now + duration;
                                         });
  }
}

ACTION bpintegrity::makeagreemnt(name identifier,
                                 const eosio::string &agreement) {
  require_auth(get_self());

  auto itr = agreements_table.find(identifier.value);
  check(itr == agreements_table.end(), "agreement already exists and cannot be "
                                       "modified. Create a new agreement.");
  agreements_table.emplace(get_self(), [&](auto &x) {
    x.identifier = identifier;
    x.text = agreement;
  });
}

void bpintegrity::check_is_bp(name owner) {
  producers_t producers_table(name("eosio"), name("eosio").value);
  auto prod = producers_table.get(owner.value, "only BPs can run this action");
}

// void bpintegrity::testrm() {
//   require_auth(get_self());

//   signatures_t signatures_table(get_self(), name("infra").value);
//   auto it = signatures_table.lower_bound(0);
//   while (it != signatures_table.end()) {
//     it = signatures_table.erase(it);
//   }
// }