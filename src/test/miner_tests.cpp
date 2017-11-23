#include <boost/test/unit_test.hpp>

#include "init.h"
#include "main.h"
#include "uint256.h"
#include "util.h"
#include "miner.h"
#include "wallet.h"

extern void SHA256Transform(void* pstate, void* pinput, const void* pinit);

BOOST_AUTO_TEST_SUITE(miner_tests)

static
struct {
    unsigned char extranonce;
    unsigned int nonce;
} blockinfo[] = {
    {5, 0x2e611100}, {6, 0x4c491200}, {2, 0xf3791700}, {6, 0x95f41a00},
    {6, 0xea0c1b00}, {2, 0xf8743200}, {2, 0x361a3500}, {3, 0x76cc3900},
    {4, 0xf4213c00}, {4, 0x3cfd5200}, {3, 0xe2497300}, {4, 0xe1967400},
    {2, 0xbf6c9100}, {2, 0x5f219d00}, {4, 0x6928a700}, {2, 0x3099c100},
    {5, 0x0fecd200}, {3, 0xd0ebfa00}, {4, 0x22bd1201}, {1, 0x0c7f2501},
    {4, 0x2bfc2901}, {3, 0xe1c75a01}, {2, 0x52c65c01}, {4, 0x4faf5e01},
    {4, 0x21476901}, {1, 0x47b38c01}, {6, 0xd8f89201}, {1, 0x031bb101},
    {3, 0x79f5b401}, {6, 0x1328c301}, {1, 0x7f3cc801}, {1, 0x75ede001},
    {5, 0x801fe101}, {6, 0x27520702}, {1, 0xa78b2502}, {2, 0x49002b02},
    {2, 0x578c4202}, {6, 0x14df4602}, {4, 0xcafa4602},
};

// NOTE: These tests rely on CreateNewBlock doing its own self-validation!
BOOST_AUTO_TEST_CASE(CreateNewBlock_validity)
{
    CReserveKey reservekey(pwalletMain);
    CBlockTemplate *pblocktemplate;
    CTransaction tx;
    CScript script;
    uint256 hash;

    // Simple block creation, nothing special yet:
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));

    // We can't make transactions until we have inputs
    // Therefore, load 39 blocks :)
    // Only load 39, because difficulty adjustment kicks in on the 40th.
    std::vector<CTransaction*>txFirst;
    for (unsigned int i = 0; i < sizeof(blockinfo)/sizeof(*blockinfo); ++i)
    {
        CBlock *pblock = &pblocktemplate->block; // pointer for convenience
        pblock->nVersion = 1;
        pblock->nTime = pindexBest->GetMedianTimePast()+1;
        pblock->vtx[0].vin[0].scriptSig = CScript();
        pblock->vtx[0].vin[0].scriptSig.push_back(blockinfo[i].extranonce);
        pblock->vtx[0].vin[0].scriptSig.push_back(pindexBest->nHeight);
        pblock->vtx[0].vout[0].scriptPubKey = CScript();
        if (txFirst.size() < 2)
            txFirst.push_back(new CTransaction(pblock->vtx[0]));
        pblock->hashMerkleRoot = pblock->BuildMerkleTree();
        pblock->nNonce = blockinfo[i].nonce;
        CValidationState state;
        BOOST_CHECK(ProcessBlock(state, NULL, pblock));
        BOOST_CHECK(state.IsValid());
        pblock->hashPrevBlock = pblock->GetHash();
    }
    delete pblocktemplate;

    // Just to make sure we can still make simple blocks
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));

    // block sigops > limit: 1000 CHECKMULTISIG + 1
    tx.vin.resize(1);
    // NOTE: OP_NOP is used to force 20 SigOps for the CHECKMULTISIG
    tx.vin[0].scriptSig = CScript() << OP_0 << OP_0 << OP_0 << OP_NOP << OP_CHECKMULTISIG << OP_1;
    tx.vin[0].prevout.hash = txFirst[0]->GetHash();
    tx.vin[0].prevout.n = 0;
    tx.vout.resize(1);
    tx.vout[0].nValue = 5000000000LL;
    for (unsigned int i = 0; i < 1001; ++i)
    {
        tx.vout[0].nValue -= 1000000;
        hash = tx.GetHash();
        mempool.addUnchecked(hash, tx);
        tx.vin[0].prevout.hash = hash;
    }
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // block size > limit
    tx.vin[0].scriptSig = CScript();
    // 18 * (520char + DROP) + OP_1 = 9433 bytes
    std::vector<unsigned char> vchData(520);
    for (unsigned int i = 0; i < 18; ++i)
        tx.vin[0].scriptSig << vchData << OP_DROP;
    tx.vin[0].scriptSig << OP_1;
    tx.vin[0].prevout.hash = txFirst[0]->GetHash();
    tx.vout[0].nValue = 5000000000LL;
    for (unsigned int i = 0; i < 128; ++i)
    {
        tx.vout[0].nValue -= 10000000;
        hash = tx.GetHash();
        mempool.addUnchecked(hash, tx);
        tx.vin[0].prevout.hash = hash;
    }
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // orphan in mempool
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // child with higher priority than parent
    tx.vin[0].scriptSig = CScript() << OP_1;
    tx.vin[0].prevout.hash = txFirst[1]->GetHash();
    tx.vout[0].nValue = 4900000000LL;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    tx.vin[0].prevout.hash = hash;
    tx.vin.resize(2);
    tx.vin[1].scriptSig = CScript() << OP_1;
    tx.vin[1].prevout.hash = txFirst[0]->GetHash();
    tx.vin[1].prevout.n = 0;
    tx.vout[0].nValue = 5900000000LL;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // coinbase in mempool
    tx.vin.resize(1);
    tx.vin[0].prevout.SetNull();
    tx.vin[0].scriptSig = CScript() << OP_0 << OP_1;
    tx.vout[0].nValue = 0;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // invalid (pre-p2sh) txn in mempool
    tx.vin[0].prevout.hash = txFirst[0]->GetHash();
    tx.vin[0].prevout.n = 0;
    tx.vin[0].scriptSig = CScript() << OP_1;
    tx.vout[0].nValue = 4900000000LL;
    script = CScript() << OP_0;
    tx.vout[0].scriptPubKey.SetDestination(script.GetID());
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    tx.vin[0].prevout.hash = hash;
    tx.vin[0].scriptSig = CScript() << (std::vector<unsigned char>)script;
    tx.vout[0].nValue -= 1000000;
    hash = tx.GetHash();
    mempool.addUnchecked(hash,tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // double spend txn pair in mempool
    tx.vin[0].prevout.hash = txFirst[0]->GetHash();
    tx.vin[0].scriptSig = CScript() << OP_1;
    tx.vout[0].nValue = 4900000000LL;
    tx.vout[0].scriptPubKey = CScript() << OP_1;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    tx.vout[0].scriptPubKey = CScript() << OP_2;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // subsidy changing
    int nHeight = pindexBest->nHeight;
    pindexBest->nHeight = 209999;
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    pindexBest->nHeight = 210000;
    BOOST_CHECK(pblocktemplate = CreateNewBlock(reservekey));
    delete pblocktemplate;
    pindexBest->nHeight = nHeight;
}

BOOST_AUTO_TEST_CASE(sha256transform_equality)
{
    unsigned int pSHA256InitState[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};


    // unsigned char pstate[32];
    unsigned char pinput[64];

    int i;

    for (i = 0; i < 32; i++) {
        pinput[i] = i;
        pinput[i+32] = 0;
    }

    uint256 hash;

    SHA256Transform(&hash, pinput, pSHA256InitState);

    BOOST_TEST_MESSAGE(hash.GetHex());

    uint256 hash_reference("0x2df5e1c65ef9f8cde240d23cae2ec036d31a15ec64bc68f64be242b1da6631f3");

    BOOST_CHECK(hash == hash_reference);
}

BOOST_AUTO_TEST_SUITE_END()
