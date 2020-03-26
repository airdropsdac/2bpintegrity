#include <eosio/binary_extension.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/privileged.hpp>
#include <eosio/producer_schedule.hpp>
#include <eosio/string.hpp>
using namespace eosio;

constexpr uint64_t AGREEMENT_DURATION = 60 * 60 * 24 * 180;

#define TELOS

CONTRACT bpintegrity : public contract {
public:
  using contract::contract;
  bpintegrity(eosio::name receiver, eosio::name code,
              eosio::datastream<const char *> ds)
      : contract(receiver, code, ds),
        sysadminjson_table(receiver, receiver.value),
        agreements_table(receiver, receiver.value) {}

  ACTION setbpjson(name owner, const eosio::string &json);
  ACTION signagreemnt(name owner, name agreement_identifier);
  ACTION makeagreemnt(name identifier, const eosio::string &agreement);
  // ACTION testrm();
  void check_is_bp(name owner);

private:
#ifdef TELOS
// https://github.com/telosnetwork/telos.contracts/blob/master/contracts/eosio.system/include/eosio.system/eosio.system.hpp#L226
struct [[eosio::table, eosio::contract("eosio.system")]] producer_info {
  name                  owner;
  double                total_votes = 0;
  eosio::public_key     producer_key; /// a packed public key object
  bool                  is_active = true;
  std::string           unreg_reason;
  std::string           url;
  uint32_t              unpaid_blocks = 0;
  uint32_t              lifetime_produced_blocks = 0;
  uint32_t              missed_blocks_per_rotation = 0;
  uint32_t              lifetime_missed_blocks = 0;
  time_point            last_claim_time;
  uint16_t              location = 0;

  uint32_t              kick_reason_id = 0;
  std::string           kick_reason;
  uint32_t              times_kicked = 0;
  uint32_t              kick_penalty_hours = 0;
  block_timestamp       last_time_kicked;

  uint64_t primary_key()const { return owner.value;                             }
  double   by_votes()const    { return is_active ? -total_votes : total_votes;  }
  bool     active()const      { return is_active;                               }
  void     deactivate()       { producer_key = public_key(); is_active = false; }

  // explicit serialization macro is not necessary, used here only to improve compilation time
  EOSLIB_SERIALIZE( producer_info, (owner)(total_votes)(producer_key)(is_active)(unreg_reason)(url)
                    (unpaid_blocks)(lifetime_produced_blocks)(missed_blocks_per_rotation)(lifetime_missed_blocks)(last_claim_time)
                    (location)(kick_reason_id)(kick_reason)(times_kicked)(kick_penalty_hours)(last_time_kicked) )
};
#else
  struct [[eosio::table, eosio::contract("eosio.system")]] producer_info {
    name owner;
    double total_votes = 0;
    eosio::public_key producer_key; /// a packed public key object
    bool is_active = true;
    std::string url;
    uint32_t unpaid_blocks = 0;
    time_point last_claim_time;
    uint16_t location = 0;
    eosio::binary_extension<eosio::block_signing_authority>
        producer_authority; // added in version 1.9.0

    uint64_t primary_key() const { return owner.value; }
    double by_votes() const { return is_active ? -total_votes : total_votes; }

    template <typename DataStream>
    friend DataStream &operator<<(DataStream &ds, const producer_info &t) {
      ds << t.owner << t.total_votes << t.producer_key << t.is_active << t.url
         << t.unpaid_blocks << t.last_claim_time << t.location;

      if (!t.producer_authority.has_value())
        return ds;

      return ds << t.producer_authority;
    }

    template <typename DataStream>
    friend DataStream &operator>>(DataStream &ds, producer_info &t) {
      return ds >> t.owner >> t.total_votes >> t.producer_key >> t.is_active >>
             t.url >> t.unpaid_blocks >> t.last_claim_time >> t.location >>
             t.producer_authority;
    }
  };
#endif

  struct [[eosio::table("sysadminjson")]] sysadminjson_s {
    name owner;
    eosio::string json;

    auto primary_key() const { return owner.value; }
  };
  typedef eosio::multi_index<name("sysadminjson"), sysadminjson_s>
      sysadminjson_t;
  sysadminjson_t sysadminjson_table;

  struct [[eosio::table("agreements")]] agreements_s {
    name identifier;
    eosio::string text;

    auto primary_key() const { return identifier.value; }
  };
  typedef eosio::multi_index<name("agreements"), agreements_s> agreements_t;
  agreements_t agreements_table;

  // scope is signer
  struct [[eosio::table("signatures")]] signatures_s {
    uint64_t id;
    name signer;
    name agreement_id;
    time_point_sec updated_at;
    time_point_sec next_update;

    auto primary_key() const { return id; }
    uint64_t by_signer() const { return signer.value; }
    static uint128_t make_agreement_signer(const name& agreement_id, const name& signer) { return (uint128_t)agreement_id.value << 64 | signer.value; }
    uint128_t by_agreement_signer() const { return make_agreement_signer(agreement_id, signer); }
  };
  typedef eosio::multi_index<name("signatures"), signatures_s,
  indexed_by<"bysigner"_n, const_mem_fun<signatures_s, uint64_t,
                                                 &signatures_s::by_signer>>,
  indexed_by<"byagrsigner"_n, const_mem_fun<signatures_s, uint128_t,
                                                 &signatures_s::by_agreement_signer>>
  > signatures_t;

  typedef eosio::multi_index<
      "producers"_n, producer_info,
      indexed_by<"prototalvote"_n, const_mem_fun<producer_info, double,
                                                 &producer_info::by_votes>>>
      producers_t;
};