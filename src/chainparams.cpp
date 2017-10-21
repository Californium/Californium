// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"

#include "chainparams.h"
#include "core.h"
#include "protocol.h"
#include "util.h"

//
// Main network
//

unsigned int pnSeed[] =
{
    0x12345678
};

class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        pchMessageStart[0] = 0x0f;
        pchMessageStart[1] = 0xdb;
        pchMessageStart[2] = 0xbb;
        pchMessageStart[3] = 0x07;
        vAlertPubKey = ParseHex("04fd68acb6a895f3462d91b43eef0da845f0d531958a858554feab3ac330562bf76910700b3f7c29ee273ddc4da2bb5b953858f6958a50e8831eb43ee30c32f21d");
        nDefaultPort = 44252;
        nRPCPort = 44254;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 20);
        //nSubsidyHalvingInterval = 100000;

        const char* pszTimestamp = "Californium is a radioactive metallic chemical element with symbol Cf and atomic number 98";
        CTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CBigNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 1 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1415510670;
        genesis.nBits    = 0x1e0fffff;
        genesis.nNonce   = 655496;

        hashGenesisBlock = genesis.GetHash();

        printf("%s\n", hashGenesisBlock.ToString().c_str());
        printf("%s\n", genesis.hashMerkleRoot.ToString().c_str());
        printf("%x\n", bnProofOfWorkLimit.GetCompact());
        genesis.print();

        hashGenesisBlock = genesis.GetHash();


        assert(hashGenesisBlock == uint256("0x00000a99a373e0fd8209e0d19696855a3523cbc6bdd242745b0cf0640ed15eaf"));
        assert(genesis.hashMerkleRoot == uint256("0x7f4b22187b737a1f46ef9481eb55f97c3b5be9744484d41a9730c5b481e7f1a8"));

        vSeeds.push_back(CDNSSeedData("californiumcoin.cf", "54.149.30.199"));
        vSeeds.push_back(CDNSSeedData("", ""));


        base58Prefixes[PUBKEY_ADDRESS] = 88;
        base58Prefixes[SCRIPT_ADDRESS] = 30;
        base58Prefixes[SECRET_KEY] = 224;

        // Convert the pnSeeds array into usable address objects.
        for (unsigned int i = 0; i < ARRAYLEN(pnSeed); i++)
        {
            // It'll only connect to one or two seed nodes because once it connects,
            // it'll get a pile of addresses with newer timestamps.
            // Seed nodes are given a random 'last seen time'
            const int64 nTwoDays = 2 * 24 * 60 * 60;
            struct in_addr ip;
            memcpy(&ip, &pnSeed[i], sizeof(ip));
            CAddress addr(CService(ip, GetDefaultPort()));
            addr.nTime = GetTime() - GetRand(nTwoDays) - nTwoDays;
            vFixedSeeds.push_back(addr);
        }
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }

    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;


//
// Testnet
//
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        pchMessageStart[0] = 0x0f;
        pchMessageStart[1] = 0xdb;
        pchMessageStart[2] = 0xbb;
        pchMessageStart[3] = 0x07;
        vAlertPubKey = ParseHex("cf50a15957f7e05910a9ca54fd84a0b555a3563561f3ecbdb6f844f752917f8a8d0041d579022044bb9398dbb7babec4601444fc60870826f3f15afae074213bf4");
        nDefaultPort = 45530;
        nRPCPort = 45531;
        strDataDir = "testnet";

        genesis.nTime    = 1415510670;
        genesis.nNonce   = 655496;

        //hashGenesisBlock = genesis.GetHash();

        printf("%s\n", hashGenesisBlock.ToString().c_str());
        printf("%s\n", genesis.hashMerkleRoot.ToString().c_str());
        genesis.print();
         hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000a99a373e0fd8209e0d19696855a3523cbc6bdd242745b0cf0640ed15eaf"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = 68;
        base58Prefixes[SCRIPT_ADDRESS] = 30;
        base58Prefixes[SECRET_KEY] = 239;

    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        pchMessageStart[0] = 0x0f;
        pchMessageStart[1] = 0xdb;
        pchMessageStart[2] = 0xbb;
        pchMessageStart[3] = 0x07;
        nSubsidyHalvingInterval = 150;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        genesis.nTime = 1415510670;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 655496;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 45539;
        strDataDir = "regtest";

        hashGenesisBlock = genesis.GetHash();

        printf("%s\n", hashGenesisBlock.ToString().c_str());
        printf("%s\n", genesis.hashMerkleRoot.ToString().c_str());
        genesis.print();

        hashGenesisBlock = genesis.GetHash();

        assert(hashGenesisBlock == uint256("0x3fd531ed3c15820b8282227d4f0975331a0a0fd249f64f9e83d81dccc98508ab"));

        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.

        base58Prefixes[PUBKEY_ADDRESS] = 0;
        base58Prefixes[SCRIPT_ADDRESS] = 5;
        base58Prefixes[SECRET_KEY] = 128;
    }

    virtual bool RequireRPCPassword() const { return false; }
    virtual Network NetworkID() const { return CChainParams::REGTEST; }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        case CChainParams::REGTEST:
            pCurrentParams = &regTestParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet && fRegTest) {
        return false;
    }

    if (fRegTest) {
        SelectParams(CChainParams::REGTEST);
    } else if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}
