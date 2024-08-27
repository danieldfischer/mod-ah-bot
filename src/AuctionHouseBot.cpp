/*
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "ObjectMgr.h"
#include "AuctionHouseMgr.h"
#include "Config.h"
#include "Player.h"
#include "WorldSession.h"
#include "GameTime.h"
#include "DatabaseEnv.h"

#include "AuctionHouseBot.h"
#include "AuctionHouseBotCommon.h"

using namespace std;

AuctionHouseBot::AuctionHouseBot(uint32 account, uint32 id)
{
    _account        = account;
    _id             = id;

    _lastrun_a_sec  = time(NULL);
    _lastrun_h_sec  = time(NULL);
    _lastrun_n_sec  = time(NULL);

    _allianceConfig = NULL;
    _hordeConfig    = NULL;
    _neutralConfig  = NULL;
}

AuctionHouseBot::~AuctionHouseBot()
{
    // Nothing
}

uint32 AuctionHouseBot::getElement(std::set<uint32> set, int index)
{
<<<<<<< HEAD
    if (!AHBSeller)
    {
        if (debug_Out)
            LOG_INFO("module", "AHSeller: Disabled");
        return;
    }

    uint32 minItems = config->GetMinItemsTotal();
    uint32 maxItems = config->GetMaxItemsTotal();
=======
    std::set<uint32>::iterator it = set.begin();
    std::advance(it, index);
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4

    return *it;
}

uint32 AuctionHouseBot::getStackCount(AHBConfig* config, uint32 max)
{
    if (max == 1)
    {
        return 1;
    }

    // 
    // Organize the stacks in a pseudo random way
    // 

    if (config->DivisibleStacks)
    {
        uint32 ret = 0;

        if (max % 5 == 0) // 5, 10, 15, 20
        {
            ret = urand(1, 4) * 5;
        }

        if (max % 4 == 0) // 4, 8, 12, 16
        {
            ret = urand(1, 4) * 4;
        }

        if (max % 3 == 0) // 3, 6, 9
        {
            ret = urand(1, 3) * 3;
        }

        if (ret > max)
        {
            ret = max;
        }

        return ret;
    }

    // 
    // Totally random
    // 

    return urand(1, max);
}

uint32 AuctionHouseBot::getElapsedTime(uint32 timeClass)
{
    switch (timeClass)
    {
    case 2:
        return urand(1, 5) * 600;   // SHORT = In the range of one hour

    case 1:
        return urand(1, 23) * 3600; // MEDIUM = In the range of one day

<<<<<<< HEAD
            // If BuyPrice or SellPrice missing, use other. BuyPrice typically 4x SellPrice
            // TODO: Make a config option "EstimatePrice"
            if (SellMethod)
            {
                buyoutPrice = prototype->BuyPrice;
                if (buyoutPrice == 0)
                    buyoutPrice = prototype->SellPrice * 4;
            }
            else
            {
                buyoutPrice = prototype->SellPrice;
                if (buyoutPrice == 0)
                    buyoutPrice = prototype->BuyPrice / 4;
            }
=======
    default:
        return urand(1, 3) * 86400; // LONG = More than one day but less than three
    }
}
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4

uint32 AuctionHouseBot::getNofAuctions(AHBConfig* config, AuctionHouseObject* auctionHouse, ObjectGuid guid)
{
    //
    // All the auctions
    //

    if (!config->ConsiderOnlyBotAuctions)
    {
        return auctionHouse->Getcount();
    }

    //
    // Just the one handled by the bot
    //

    uint32 count = 0;

    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin(); itr != auctionHouse->GetAuctionsEnd(); ++itr)
    {
        AuctionEntry* Aentry = itr->second;

        if (guid == Aentry->owner)
        {
            count++;
            break;
        }
    }

    return count;
}

// =============================================================================
// This routine performs the bidding operations for the bot
// =============================================================================

void AuctionHouseBot::Buy(Player* AHBplayer, AHBConfig* config, WorldSession* session)
{
<<<<<<< HEAD
    // TODO: seems like it just shouldnt call this if it is disabled.
    if (!AHBBuyer)
=======
    //
    // Check if disabled
    //

    if (!config->AHBBuyer)
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
    {
        return;
    }

<<<<<<< HEAD
    if (debug_Out)
    {
        LOG_INFO("module", "-------------------------------------------------");
        LOG_INFO("module", "AHBuyer: Starting");
    }

    // TODO: Config option for multi-bids
//    QueryResult result = CharacterDatabase.Query("SELECT id FROM auctionhouse WHERE itemowner<>{} AND buyguid<>{}", AHBplayerGUID, AHBplayerGUID);
    QueryResult result = CharacterDatabase.Query("SELECT id FROM auctionhouse WHERE itemowner<>{}", AHBplayerGUID);
=======
    //
    // Retrieve items not owner by the bot and not bought by the bot
    //

    QueryResult result = CharacterDatabase.Query("SELECT id FROM auctionhouse WHERE itemowner<>{} AND buyguid<>{}", _id, _id);
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4

    if (!result)
    {
        return;
    }

    if (result->GetRowCount() == 0)
    {
        return;
    }

    //
    // Fetches content of selected AH to look for possible bids
    //

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(config->GetAHFID());
    std::set<uint32>    possibleBids;

    do
    {
        uint32 tmpdata = result->Fetch()->Get<uint32>();
        possibleBids.insert(tmpdata);
    } while (result->NextRow());

<<<<<<< HEAD
    // Note: Modified by max item bonus.
    // TODO: Make this a config option
    //  Make an option to scale by player count.
    //  The ID doesn't seem to be the one we expect.
    if (debug_Out)
    {
        LOG_INFO("module", "AHBuyer: Placing {} bids from {} auctionhouse.", config->GetTotalBidsPerInterval(), config->GetAHFID());
    }

    for (uint32 count = 1; count <= config->GetTotalBidsPerInterval(); ++count)
=======
    //
    // If it's not possible to bid stop here
    //

    if (possibleBids.empty())
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
    {
        if (config->DebugOutBuyer)
        {
<<<<<<< HEAD
            //if (debug_Out) sLog->outError( "AHBuyer: I have no items to bid on.");
            count = config->GetTotalBidsPerInterval();
            continue;
            // TODO: Why does he just not exit the loop? Weird way to do so.
=======
            LOG_INFO("module", "AHBot [{}]: no auctions to bid on has been recovered", _id);
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
        }

        return;
    }

    //
    // Perform the operation for a maximum amount of bids attempts configured
    //

    for (uint32 count = 1; count <= config->GetBidsPerInterval(); ++count)
    {
        //
        // Choose a random auction from possible auctions
        //

        uint32 randBid = urand(0, possibleBids.size() - 1);

        std::set<uint32>::iterator it = possibleBids.begin();
        std::advance(it, randBid);

        AuctionEntry* auction = auctionHouse->GetAuction(*it);

        //
        // Prevent to bid again on the same auction
        //

        possibleBids.erase(randBid);

        if (!auction)
        {
            continue;
        }

        //
        // Prevent from buying items from the other bots
        //

        if (gBotsId.find(auction->owner.GetCounter()) != gBotsId.end())
        {
            continue;
        }

        //
        // Get the item information
        //

        Item* pItem = sAuctionMgr->GetAItem(auction->item_guid);

        if (!pItem)
        {
            if (config->DebugOutBuyer)
            {
                LOG_ERROR("module", "AHBot [{}]: item {} doesn't exist, perhaps bought already?", _id, auction->item_guid.ToString());
            }

            continue;
        }

        //
        // Get the item prototype
        //

        ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(auction->item_template);

        //
        // Check which price we have to use, startbid or if it is bidded already
        //

        uint32 currentprice;

        if (auction->bid)
        {
            currentprice = auction->bid;
        }
        else
        {
            currentprice = auction->startbid;
        }

        //
        // Prepare portion from maximum bid
<<<<<<< HEAD
        double bidrate = static_cast<double>(urand(1, 100)) / 100;
        long double bidMax = 0;
        uint32 itemPrice = 0;

        // check that bid has acceptable value and take bid based on vendorprice, stacksize and quality
        // TODO: Still not the most efficient code, but better than before and more readable.
        if (BuyMethod)
        {
            if (prototype->Quality <= AHB_MAX_QUALITY)
            {
                // TODO: Does logic need to be added similar to sell method for missing price data?
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality))
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality);
            }
            else
            {
                // quality is something it shouldn't be, let's get out of here
                if (debug_Out)
                    LOG_ERROR("module", "AHBuyer: Quality {} not Supported", prototype->Quality);
=======
        //

        double      bidrate = static_cast<double>(urand(1, 100)) / 100;
        long double bidMax  = 0;

        //
        // Check that bid has an acceptable value and take bid based on vendorprice, stacksize and quality
        //

        if (config->BuyMethod)
        {
            if (prototype->Quality <= AHB_MAX_QUALITY)
            {
                if (currentprice < prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality))
                {
                    bidMax = prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality);
                }
            }
            else
            {
                if (config->DebugOutBuyer)
                {
                    LOG_ERROR("module", "AHBot [{}]: Quality {} not Supported", _id, prototype->Quality);
                }

>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
                continue;
            }
        }
        else
        {
            if (prototype->Quality <= AHB_MAX_QUALITY)
            {
<<<<<<< HEAD
                if (prototype->SellPrice == 0)
                    itemPrice = prototype->BuyPrice / 5;  // was 25% for many resources and some items, but majority and new standard is 20%
                else
                    itemPrice = prototype->SellPrice;
                itemPrice = itemPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality);

                if (currentprice < itemPrice)
                    bidMax = itemPrice;
                // else fail out later
            }
            else
            {
                // quality is something it shouldn't be, let's get out of here
                if (debug_Out)
                    LOG_ERROR("module", "AHBuyer: Quality {} not Supported", prototype->Quality);
=======
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality))
                {
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality);
                }
            }
            else
            {
                if (config->DebugOutBuyer)
                {
                    LOG_ERROR("module", "AHBot [{}]: Quality {} not Supported", _id, prototype->Quality);
                }

>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
                continue;
            }
        }

<<<<<<< HEAD
        // check some special items, and do recalculating to their prices
        // TODO: Why is this?
=======
        //
        // Recalculate the bid depending on the type of the item
        //

>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
        switch (prototype->Class)
        {
            // ammo
        case 6:
            bidMax = 0;
            break;
        default:
            break;
        }

        //
        // Test the computed bid
        //

        if (bidMax == 0)
        {
<<<<<<< HEAD
            // quality check failed to get bidmax, let's get out of here
            // TODO: Should really log the reasons better, and fail out as needed instead of this random catch all.
=======
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
            continue;
        }

        //
        // Calculate our bid
<<<<<<< HEAD
        // Note: Previously bids can't fail as long as they under the max.
        //  1. bidrate is a random 1-100%.
        //  2. Why is bidrate only applying to the difference?
//        long double bidvalue = currentprice + ((bidMax - currentprice) * bidrate);
        // Note: New logic - fully random bid
        //  Sets a min bid based on itemvalue. ex: if sellprice model, always bid at least sellprice.
        // TODO: check if buyprice should be used.
        long double bidvalue = prototype->SellPrice + (bidMax - prototype->SellPrice) * bidrate;
        // Convert to uint32
        uint32 bidprice = static_cast<uint32>(bidvalue);

        // Note: Moved this farther up to log failed bids
        if (debug_Out)
=======
        //

        long double bidvalue = currentprice + ((bidMax - currentprice) * bidrate);
        uint32      bidprice = static_cast<uint32>(bidvalue);

        //
        // Check our bid is high enough to be valid. If not, correct it to minimum.
        //

        if ((currentprice + auction->GetAuctionOutBid()) > bidprice)
        {
            bidprice = currentprice + auction->GetAuctionOutBid();
        }

        //
        // Print out debug info
        //

        if (config->DebugOutBuyer)
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
        {
            LOG_INFO("module", "-------------------------------------------------");
            LOG_INFO("module", "AHBot [{}]: Info for Auction #{}:", _id, auction->Id);
            LOG_INFO("module", "AHBot [{}]: AuctionHouse: {}"     , _id, auction->GetHouseId());
            LOG_INFO("module", "AHBot [{}]: Owner: {}"            , _id, auction->owner.ToString());
            LOG_INFO("module", "AHBot [{}]: Bidder: {}"           , _id, auction->bidder.ToString());
            LOG_INFO("module", "AHBot [{}]: Starting Bid: {}"     , _id, auction->startbid);
            LOG_INFO("module", "AHBot [{}]: Current Bid: {}"      , _id, currentprice);
            LOG_INFO("module", "AHBot [{}]: Buyout: {}"           , _id, auction->buyout);
            LOG_INFO("module", "AHBot [{}]: Deposit: {}"          , _id, auction->deposit);
            LOG_INFO("module", "AHBot [{}]: Expire Time: {}"      , _id, uint32(auction->expire_time));
            LOG_INFO("module", "AHBot [{}]: Bid Rate: {}"         , _id, bidrate);
            LOG_INFO("module", "AHBot [{}]: Bid Max: {}"          , _id, bidMax);
            LOG_INFO("module", "AHBot [{}]: Bid Value: {}"        , _id, bidvalue);
            LOG_INFO("module", "AHBot [{}]: Bid Price: {}"        , _id, bidprice);
            LOG_INFO("module", "AHBot [{}]: Item GUID: {}"        , _id, auction->item_guid.ToString());
            LOG_INFO("module", "AHBot [{}]: Item Template: {}"    , _id, auction->item_template);
            LOG_INFO("module", "AHBot [{}]: Item Info:");
            LOG_INFO("module", "AHBot [{}]: Item ID: {}"          , _id, prototype->ItemId);
            LOG_INFO("module", "AHBot [{}]: Buy Price: {}"        , _id, prototype->BuyPrice);
            LOG_INFO("module", "AHBot [{}]: Sell Price: {}"       , _id, prototype->SellPrice);
            LOG_INFO("module", "AHBot [{}]: Bonding: {}"          , _id, prototype->Bonding);
            LOG_INFO("module", "AHBot [{}]: Quality: {}"          , _id, prototype->Quality);
            LOG_INFO("module", "AHBot [{}]: Item Level: {}"       , _id, prototype->ItemLevel);
            LOG_INFO("module", "AHBot [{}]: Ammo Type: {}"        , _id, prototype->AmmoType);
            LOG_INFO("module", "-------------------------------------------------");
        }

<<<<<<< HEAD
        // Note: If bidprice isn't high enough, skip this bid.
        //  Previously - Check our bid is high enough to be valid. If not, correct it to minimum.
        //    The next bid has to be more than a threshold, ex: 2 copper?
        //    With the prior bidvalue logic this just meant if an item was priced under the max a bid would always be successfull.
        if ((currentprice + auction->GetAuctionOutBid()) > bidprice)
            //            bidprice = currentprice + auction->GetAuctionOutBid();
        {
            LOG_INFO("module", "AHBuyer: Random bid too low, skipping.");
            continue;
        }

=======
        //
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
        // Check whether we do normal bid, or buyout
        //

        bool bought = false;

        if ((bidprice < auction->buyout) || (auction->buyout == 0))
        {
            //
            // Perform a new bid on the auction
            //

            if (auction->bidder)
            {
                if (auction->bidder != AHBplayer->GetGUID())
                {
                    //
                    // Mail to last bidder and return their money
                    //

                    auto trans = CharacterDatabase.BeginTransaction();

                    sAuctionMgr->SendAuctionOutbiddedMail(auction, bidprice, session->GetPlayer(), trans);
                    CharacterDatabase.CommitTransaction  (trans);
                }
            }

            auction->bidder = AHBplayer->GetGUID();
            auction->bid    = bidprice;

            //
            // Save the auction into database
            //

            CharacterDatabase.Execute("UPDATE auctionhouse SET buyguid = '{}', lastbid = '{}' WHERE id = '{}'", auction->bidder.GetCounter(), auction->bid, auction->Id);
        }
        else
        {
            bought = true;

            //
            // Perform the buyout
            //

            auto trans = CharacterDatabase.BeginTransaction();

            if ((auction->bidder) && (AHBplayer->GetGUID() != auction->bidder))
            {
                //
                // Send the mail to the last bidder
                //

                sAuctionMgr->SendAuctionOutbiddedMail(auction, auction->buyout, session->GetPlayer(), trans);
            }

            auction->bidder = AHBplayer->GetGUID();
            auction->bid    = auction->buyout;

            // 
            // Send mails to buyer & seller
            // 

            sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
            sAuctionMgr->SendAuctionWonMail       (auction, trans);

            // 
            // Removes any trace of the item
            // 

            auction->DeleteFromDB(trans);

            sAuctionMgr->RemoveAItem   (auction->item_guid);
            auctionHouse->RemoveAuction(auction);

            CharacterDatabase.CommitTransaction(trans);
        }

        //
        // Tracing
        //

        if (config->TraceBuyer)
        {
            if (bought)
            {
                LOG_INFO("module", "AHBot [{}]: Bought , id={}, ah={}, item={}, start={}, current={}, buyout={}", _id, auction->Id, auction->GetHouseId(), auction->item_template, auction->startbid, currentprice, auction->buyout);
            }
            else
            {
                LOG_INFO("module", "AHBot [{}]: New bid, id={}, ah={}, item={}, start={}, current={}, buyout={}", _id, auction->Id, auction->GetHouseId(), auction->item_template, auction->startbid, currentprice, auction->buyout);
            }
        }
    }
}

// =============================================================================
// This routine performs the selling operations for the bot
// =============================================================================

void AuctionHouseBot::Sell(Player* AHBplayer, AHBConfig* config)
{
    // 
    // Check if disabled
    // 

    if (!config->AHBSeller)
    {
        return;
    }

    // 
    // Check the given limits
    // 

    uint32 minItems = config->GetMinItems();
    uint32 maxItems = config->GetMaxItems();

    if (maxItems == 0)
    {
        return;
    }

    // 
    // Retrieve the auction house situation
    // 

<<<<<<< HEAD
void AuctionHouseBot::Initialize()
{

    //
    // check if the AHBot account/GUID in the config actually exists
    //
    // TODO: For some reason I couldn't get either error to trigger.

    if ((AHBplayerAccount != 0) || (AHBplayerGUID != 0))
    {
        QueryResult result = CharacterDatabase.Query("SELECT 1 FROM characters WHERE account = {} AND guid = {}", AHBplayerAccount, AHBplayerGUID);
        if (!result)
        {
            LOG_ERROR("module", "AuctionHouseBot: The account/GUID-information set for your AHBot is incorrect (account: {} guid: {})", AHBplayerAccount, AHBplayerGUID);
            return;
        }
    }
    else
    {
        LOG_ERROR("module", "AuctionHouseBot: The account/GUID-information is not set for your AHBot");
        return;
    }

    DisableItemStore.clear();
    QueryResult result = WorldDatabase.Query("SELECT item FROM mod_auctionhousebot_disabled_items");
=======
    AuctionHouseEntry const* ahEntry = sAuctionMgr->GetAuctionHouseEntry(config->GetAHFID());
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4

    if (!ahEntry)
    {
        return;
    }

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(config->GetAHFID());

    if (!auctionHouse)
    {
        return;
    }

<<<<<<< HEAD
    if (AHBSeller)
=======
    auctionHouse->Update();

    // 
    // Check if we are clear to proceed
    // 

    bool   aboveMin = false;
    bool   aboveMax = false;
    uint32 auctions = getNofAuctions(config, auctionHouse, AHBplayer->GetGUID());
    uint32 items    = 0;

    if (auctions >= minItems)
    {
        aboveMin = true;

        if (config->DebugOutSeller)
        {
            LOG_ERROR("module", "AHBot [{}]: Auctions above minimum", _id);
        }

        return;
    }

    if (auctions >= maxItems)
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
    {
        aboveMax = true;

        if (config->DebugOutSeller)
        {
            LOG_ERROR("module", "AHBot [{}]: Auctions at or above maximum", _id);
        }

        return;
    }

    if ((maxItems - auctions) >= config->ItemsPerCycle)
    {
        items = config->ItemsPerCycle;
    }
    else
    {
        items = (maxItems - auctions);
    }

    // 
    // Retrieve the configuration for this run
    // 

    uint32 greyTGcount   = config->GetMaximum(AHB_GREY_TG);
    uint32 whiteTGcount  = config->GetMaximum(AHB_WHITE_TG);
    uint32 greenTGcount  = config->GetMaximum(AHB_GREEN_TG);
    uint32 blueTGcount   = config->GetMaximum(AHB_BLUE_TG);
    uint32 purpleTGcount = config->GetMaximum(AHB_PURPLE_TG);
    uint32 orangeTGcount = config->GetMaximum(AHB_ORANGE_TG);
    uint32 yellowTGcount = config->GetMaximum(AHB_YELLOW_TG);

    uint32 greyIcount    = config->GetMaximum(AHB_GREY_I);
    uint32 whiteIcount   = config->GetMaximum(AHB_WHITE_I);
    uint32 greenIcount   = config->GetMaximum(AHB_GREEN_I);
    uint32 blueIcount    = config->GetMaximum(AHB_BLUE_I);
    uint32 purpleIcount  = config->GetMaximum(AHB_PURPLE_I);
    uint32 orangeIcount  = config->GetMaximum(AHB_ORANGE_I);
    uint32 yellowIcount  = config->GetMaximum(AHB_YELLOW_I);

    uint32 greyTGoods    = config->GetItemCounts(AHB_GREY_TG);
    uint32 whiteTGoods   = config->GetItemCounts(AHB_WHITE_TG);
    uint32 greenTGoods   = config->GetItemCounts(AHB_GREEN_TG);
    uint32 blueTGoods    = config->GetItemCounts(AHB_BLUE_TG);
    uint32 purpleTGoods  = config->GetItemCounts(AHB_PURPLE_TG);
    uint32 orangeTGoods  = config->GetItemCounts(AHB_ORANGE_TG);
    uint32 yellowTGoods  = config->GetItemCounts(AHB_YELLOW_TG);

    uint32 greyItems     = config->GetItemCounts(AHB_GREY_I);
    uint32 whiteItems    = config->GetItemCounts(AHB_WHITE_I);
    uint32 greenItems    = config->GetItemCounts(AHB_GREEN_I);
    uint32 blueItems     = config->GetItemCounts(AHB_BLUE_I);
    uint32 purpleItems   = config->GetItemCounts(AHB_PURPLE_I);
    uint32 orangeItems   = config->GetItemCounts(AHB_ORANGE_I);
    uint32 yellowItems   = config->GetItemCounts(AHB_YELLOW_I);

    //
    // Only insert a few at a time, so they dont appear all at once
    //

    uint32 noSold   = 0; // Tracing counter
    uint32 binEmpty = 0; // Tracing counter
    uint32 noNeed   = 0; // Tracing counter
    uint32 tooMany  = 0; // Tracing counter
    uint32 loopBrk  = 0; // Tracing counter
    uint32 err      = 0; // Tracing counter

    for (uint32 cnt = 1; cnt <= items; cnt++)
    {
        uint32 itemID      = 0;
        uint32 loopbreaker = 0;

        // 
        // Attempts for some times to insert a single item stack as an auction.
        // The attempt can be stopped by several internal checks.
        //

        while (itemID == 0 && loopbreaker <= 32)
        {
            ++loopbreaker;

            // 
            // Get a random item from the bins
            // 

            uint32 choice = urand(0, 13);

            switch (choice)
            {
            case AHB_GREY_I:
                if ((config->GreyItemsBin.size() > 0) && (greyItems < greyIcount))
                {
                    itemID = getElement(config->GreyItemsBin, urand(0, config->GreyItemsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_WHITE_I:
                if ((config->WhiteItemsBin.size() > 0) && (whiteItems < whiteIcount))
                {
                    itemID = getElement(config->WhiteItemsBin, urand(0, config->WhiteItemsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_GREEN_I:
                if ((config->GreenItemsBin.size() > 0) && (greenItems < greenIcount))
                {
                    itemID = getElement(config->GreenItemsBin, urand(0, config->GreenItemsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_BLUE_I:
                if ((config->BlueItemsBin.size() > 0) && (blueItems < blueIcount))
                {
                    itemID = getElement(config->BlueItemsBin, urand(0, config->BlueItemsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_PURPLE_I:
                if ((config->PurpleItemsBin.size() > 0) && (purpleItems < purpleIcount))
                {
                    itemID = getElement(config->PurpleItemsBin, urand(0, config->PurpleItemsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_ORANGE_I:
                if ((config->OrangeItemsBin.size() > 0) && (orangeItems < orangeIcount))
                {
                    itemID = getElement(config->OrangeItemsBin, urand(0, config->OrangeItemsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_YELLOW_I:
                if ((config->YellowItemsBin.size() > 0) && (yellowItems < yellowIcount))
                {
                    itemID = getElement(config->YellowItemsBin, urand(0, config->YellowItemsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

<<<<<<< HEAD
            // TODO: This seems backwards, also would rather it check if both BuyPrice & SellPrice are empty.
            //       * This should be a config option.
            /*
            if (SellMethod)
            {
                if (itr->second.BuyPrice == 0)
=======
            case AHB_GREY_TG:
                if ((config->GreyTradeGoodsBin.size() > 0) && (greyTGoods < greyTGcount))
                {
                    itemID = getElement(config->GreyTradeGoodsBin, urand(0, config->GreyTradeGoodsBin.size() - 1));
                }
                else
                {
                    noNeed++;
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
                    continue;
                }

                break;

            case AHB_WHITE_TG:
                if ((config->WhiteTradeGoodsBin.size() > 0) && (whiteTGoods < whiteTGcount))
                {
                    itemID = getElement(config->WhiteTradeGoodsBin, urand(0, config->WhiteTradeGoodsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
<<<<<<< HEAD
            }
            */
            if (itr->second.BuyPrice == 0 && itr->second.SellPrice == 0)
                continue;
=======
                }
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4

                break;

            case AHB_GREEN_TG:
                if ((config->GreenTradeGoodsBin.size() > 0) && (greenTGoods < greenTGcount))
                {
                    itemID = getElement(config->GreenTradeGoodsBin, urand(0, config->GreenTradeGoodsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_BLUE_TG:
                if ((config->BlueTradeGoodsBin.size() > 0) && (blueTGoods < blueTGcount))
                {
                    itemID = getElement(config->BlueTradeGoodsBin, urand(0, config->BlueTradeGoodsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_PURPLE_TG:
                if ((config->PurpleTradeGoodsBin.size() > 0) && (purpleTGoods < purpleTGcount))
                {
                    itemID = getElement(config->PurpleTradeGoodsBin, urand(0, config->PurpleTradeGoodsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_ORANGE_TG:
                if ((config->OrangeTradeGoodsBin.size() > 0) && (orangeTGoods < orangeTGcount))
                {
                    itemID = getElement(config->OrangeTradeGoodsBin, urand(0, config->OrangeTradeGoodsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            case AHB_YELLOW_TG:
                if ((config->YellowTradeGoodsBin.size() > 0) && (yellowTGoods < yellowTGcount))
                {
                    itemID = getElement(config->YellowTradeGoodsBin, urand(0, config->YellowTradeGoodsBin.size() - 1));
                }
                else
                {
                    noNeed++;
                    continue;
                }

                break;

            default:
                err++;

                if (config->DebugOutSeller)
                {
                    LOG_ERROR("module", "AHBot [{}]: itemID Switch - Default Reached", _id);
                }

                break;
            }

            // 
            // Check if it's a valid selection
            // 

            if (itemID == 0)
            {
                binEmpty++;

                if (config->DebugOutSeller)
                {
                    LOG_ERROR("module", "AHBot [{}]: No item could be selected in the bin {}", _id, choice);
                }

                continue;
            }

            // 
            // Check how many stacks of this items are sold by the bot.
            // This avoid unfortunate rolls to overwhelm the market with the same products on low population, especially with whitelists.
            // 

            if (config->DuplicatesCount > 0)
            {
                uint32 noStacks = 0;

                for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin(); itr != auctionHouse->GetAuctionsEnd(); ++itr)
                {
                    AuctionEntry* Aentry = itr->second;

                    if (AHBplayer->GetGUID() == Aentry->owner)
                    {
                        if (itemID == Aentry->item_template)
                        {
                            noStacks++;
                        }
                    }
                }

                if (noStacks >= config->DuplicatesCount)
                {
                    tooMany++;
                    continue;
                }
            }

            // 
            // Retrieve information about the selected item
            // 

            ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(itemID);

            if (prototype == NULL)
            {
                err++;

                if (config->DebugOutSeller)
                {
                    LOG_ERROR("module", "AHBot [{}]: could not get prototype of item {}", _id, itemID);
                }

<<<<<<< HEAD
            // Disable items by Deprecated Flag
            // TODO: Check if Flags includes 16 (bit 4) or Flags2 includes ?
            if ((itr->second.Flags & 16) or (itr->second.Flags2 & 8192))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Deprecated/PTR/Beta/Unused Item)", itr->second.ItemId);
                continue;
            }

            // Disable items by Id
            if (DisableItemStore.find(itr->second.ItemId) != DisableItemStore.end())
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (PTR/Beta/Unused Item)", itr->second.ItemId);
=======
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
                continue;
            }

            Item* item = Item::CreateItem(itemID, 1, AHBplayer);

            if (item == NULL)
            {
                err++;

                if (config->DebugOutSeller)
                {
                    LOG_ERROR("module", "AHBot [{}]: could not create item from prototype {}", _id, itemID);
                }

                break;
            }

            // 
            // Start interacting with the item by adding a random property
            // 

            item->AddToUpdateQueueOf(AHBplayer);

            uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(itemID);

            if (randomPropertyId != 0)
            {
                item->SetItemRandomProperties(randomPropertyId);
            }

            // 
            // Determine the price and stack size
            // 

            uint64 buyoutPrice = 0;
            uint64 bidPrice    = 0;
            uint32 stackCount  = 1;

            if (config->SellMethod)
            {
                buyoutPrice = prototype->BuyPrice;
            }
            else
            {
                buyoutPrice = prototype->SellPrice;
            }

            if (prototype->Quality <= AHB_MAX_QUALITY)
            {
                if (config->GetMaxStack(prototype->Quality) > 1 && item->GetMaxStackCount() > 1)
                {
                    stackCount = minValue(getStackCount(config, item->GetMaxStackCount()), config->GetMaxStack(prototype->Quality));
                }
                else if (config->GetMaxStack(prototype->Quality) == 0 && item->GetMaxStackCount() > 1)
                {
                    stackCount = getStackCount(config, item->GetMaxStackCount());
                }
                else
                {
                    stackCount = 1;
                }

                buyoutPrice *= urand(config->GetMinPrice(prototype->Quality), config->GetMaxPrice(prototype->Quality));
                buyoutPrice /= 100;
                bidPrice     = buyoutPrice * urand(config->GetMinBidPrice(prototype->Quality), config->GetMaxBidPrice(prototype->Quality));
                bidPrice    /= 100;
            }
            else
            {
                err++;

                if (config->DebugOutSeller)
                {
                    LOG_ERROR("module", "AHBot [{}]: Quality {} TOO HIGH for item {}", _id, prototype->Quality, itemID);
                }

                item->RemoveFromUpdateQueueOf(AHBplayer);
                continue;
            }

            item->SetCount(stackCount);

            // 
            // Determine the auction time
            // 

            uint32 etime = getElapsedTime(config->ElapsingTimeClass);

            // 
            // Determine the deposit
            // 

            uint32 dep   = sAuctionMgr->GetAuctionDeposit(ahEntry, etime, item, stackCount);

            // 
            // Perform the auction
            // 

            auto trans = CharacterDatabase.BeginTransaction();

            AuctionEntry* auctionEntry      = new AuctionEntry();
            auctionEntry->Id                = sObjectMgr->GenerateAuctionID();
            auctionEntry->houseId           = config->GetAHID();
            auctionEntry->item_guid         = item->GetGUID();
            auctionEntry->item_template     = item->GetEntry();
            auctionEntry->itemCount         = item->GetCount();
            auctionEntry->owner             = AHBplayer->GetGUID();
            auctionEntry->startbid          = bidPrice * stackCount;
            auctionEntry->buyout            = buyoutPrice * stackCount;
            auctionEntry->bid               = 0;
            auctionEntry->deposit           = dep;
            auctionEntry->expire_time       = (time_t)etime + time(NULL);
            auctionEntry->auctionHouseEntry = ahEntry;

            item->SaveToDB(trans);
            item->RemoveFromUpdateQueueOf(AHBplayer);
            sAuctionMgr->AddAItem(item);
            auctionHouse->AddAuction(auctionEntry);
            auctionEntry->SaveToDB(trans);

            CharacterDatabase.CommitTransaction(trans);

            // 
            // Increments the number of items presents in the auction
            // 

            switch (choice)
            {
            case 0:
                ++greyItems;
                break;

            case 1:
                ++whiteItems;
                break;

            case 2:
                ++greenItems;
                break;

            case 3:
                ++blueItems;
                break;

            case 4:
                ++purpleItems;
                break;

            case 5:
                ++orangeItems;
                break;

            case 6:
                ++yellowItems;
                break;

            case 7:
                ++greyTGoods;
                break;

<<<<<<< HEAD
            // Is Trade Good?
            bool isTradeGood = false;
            if (itr->second.Class == ITEM_CLASS_TRADE_GOODS)
                isTradeGood = true;
            // Or if Gem switch activated, catergorize simple & uncut gems as Trade Goods
            // TODO: Create conf option for this
            else if (itr->second.Class == ITEM_CLASS_GEM && itr->second.GemProperties == 0)
                isTradeGood = true;

            if (isTradeGood)
                switch (itr->second.Quality)
                {
                case AHB_GREY:
                    greyTradeGoodsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_WHITE:
                    whiteTradeGoodsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_GREEN:
                    greenTradeGoodsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_BLUE:
                    blueTradeGoodsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_PURPLE:
                    purpleTradeGoodsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_ORANGE:
                    orangeTradeGoodsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_YELLOW:
                    yellowTradeGoodsBin.push_back(itr->second.ItemId);
                    break;
                }
            else
                switch (itr->second.Quality)
                {
                case AHB_GREY:
                    greyItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_WHITE:
                    whiteItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_GREEN:
                    greenItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_BLUE:
                    blueItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_PURPLE:
                    purpleItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_ORANGE:
                    orangeItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_YELLOW:
                    yellowItemsBin.push_back(itr->second.ItemId);
                    break;
                }
=======
            case 8:
                ++whiteTGoods;
                break;

            case 9:
                ++greenTGoods;
                break;

            case 10:
                ++blueTGoods;
                break;

            case 11:
                ++purpleTGoods;
                break;

            case 12:
                ++orangeTGoods;
                break;

            case 13:
                ++yellowTGoods;
                break;

            default:
                break;
            }

            noSold++;

            if (config->TraceSeller)
            {
                LOG_INFO("module", "AHBot [{}]: New stack ah={}, id={}, stack={}, bid={}, buyout={}", _id, config->GetAHID(), itemID, stackCount, auctionEntry->startbid, auctionEntry->buyout);
            }
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
        }

        if (itemID == 0 || loopbreaker > 50)
        {
            loopBrk++;
        }
<<<<<<< HEAD

        LOG_INFO("module", "AuctionHouseBot:");
        LOG_INFO("module", "{} disabled items in mod_auctionhousebot_disabled_items", uint32(DisableItemStore.size()));
        LOG_INFO("module", "loaded {} grey trade goods", uint32(greyTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} white trade goods", uint32(whiteTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} green trade goods", uint32(greenTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} blue trade goods", uint32(blueTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} purple trade goods", uint32(purpleTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} orange trade goods", uint32(orangeTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} yellow trade goods", uint32(yellowTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} grey items", uint32(greyItemsBin.size()));
        LOG_INFO("module", "loaded {} white items", uint32(whiteItemsBin.size()));
        LOG_INFO("module", "loaded {} green items", uint32(greenItemsBin.size()));
        LOG_INFO("module", "loaded {} blue items", uint32(blueItemsBin.size()));
        LOG_INFO("module", "loaded {} purple items", uint32(purpleItemsBin.size()));
        LOG_INFO("module", "loaded {} orange items", uint32(orangeItemsBin.size()));
        LOG_INFO("module", "loaded {} yellow items", uint32(yellowItemsBin.size()));
=======
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
    }

    if (config->TraceSeller)
    {
        LOG_INFO("module", "AHBot [{}]: auctionhouse {}, req={}, sold={}, aboveMin={}, aboveMax={}, loopBrk={}, noNeed={}, tooMany={}, binEmpty={}, err={}", _id, config->GetAHID(), items, noSold, aboveMin, aboveMax, loopBrk, noNeed, tooMany, binEmpty, err);
    }
}

// =============================================================================
// Perform an update cycle
// =============================================================================

void AuctionHouseBot::Update()
{
    time_t _newrun = time(NULL);

    //
    // If no configuration is associated, then stop here
    //

    if (!_allianceConfig && !_hordeConfig && !_neutralConfig)
    {
        return;
    }

    //
    // Preprare for operation
    //

    std::string accountName = "AuctionHouseBot" + std::to_string(_account);

    WorldSession _session(_account, std::move(accountName), nullptr, SEC_PLAYER, sWorld->getIntConfig(CONFIG_EXPANSION), 0, LOCALE_enUS, 0, false, false, 0);

    Player _AHBplayer(&_session);
    _AHBplayer.Initialize(_id);

    ObjectAccessor::AddObject(&_AHBplayer);

    //
    // Perform update for the factions markets
    //

    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        //
        // Alliance
        //

        if (_allianceConfig)
        {
            Sell(&_AHBplayer, _allianceConfig);

            if (((_newrun - _lastrun_a_sec) >= (_allianceConfig->GetBiddingInterval() * MINUTE)) && (_allianceConfig->GetBidsPerInterval() > 0))
            {
                Buy(&_AHBplayer, _allianceConfig, &_session);
                _lastrun_a_sec = _newrun;
            }
        }

        //
        // Horde
        //

        if (_hordeConfig)
        {
            Sell(&_AHBplayer, _hordeConfig);

            if (((_newrun - _lastrun_h_sec) >= (_hordeConfig->GetBiddingInterval() * MINUTE)) && (_hordeConfig->GetBidsPerInterval() > 0))
            {
                Buy(&_AHBplayer, _hordeConfig, &_session);
                _lastrun_h_sec = _newrun;
            }
        }

    }

    //
    // Neutral
    //

    if (_neutralConfig)
    {
        Sell(&_AHBplayer, _neutralConfig);

        if (((_newrun - _lastrun_n_sec) >= (_neutralConfig->GetBiddingInterval() * MINUTE)) && (_neutralConfig->GetBidsPerInterval() > 0))
        {
            Buy(&_AHBplayer, _neutralConfig, &_session);
            _lastrun_n_sec = _newrun;
        }
    }

    ObjectAccessor::RemoveObject(&_AHBplayer);
}

// =============================================================================
// Execute commands coming from the console
// =============================================================================

void AuctionHouseBot::Commands(AHBotCommand command, uint32 ahMapID, uint32 col, char* args)
{
    //
    // Retrieve the auction house configuration
    //

    AHBConfig *config = NULL;

    switch (ahMapID)
    {
    case 2:
        config = _allianceConfig;
        break;
    case 6:
        config = _hordeConfig;
        break;
    case 7:
        config = _neutralConfig;
        break;
    }

    //
    // Retrive the item quality
    //

    std::string color;

    switch (col)
    {
    case AHB_GREY:
        color = "grey";
        break;
    case AHB_WHITE:
        color = "white";
        break;
    case AHB_GREEN:
        color = "green";
        break;
    case AHB_BLUE:
        color = "blue";
        break;
    case AHB_PURPLE:
        color = "purple";
        break;
    case AHB_ORANGE:
        color = "orange";
        break;
    case AHB_YELLOW:
        color = "yellow";
        break;
    default:
        break;
    }

    //
    // Perform the command
    //

    switch (command)
    {
    case AHBotCommand::buyer:
    {
        char* param1 = strtok(args, " ");
        uint32 state = (uint32)strtoul(param1, NULL, 0);

        if (state == 0)
        {
            config->AHBBuyer = false;
            LOG_ERROR("module", "AHBot: Buyer disabled from console");
        }
<<<<<<< HEAD
        break;
    case 2:     //max items TODO: This doesn't use the class var
=======
        else
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4
        {
            config->AHBBuyer = true;
            LOG_ERROR("module", "AHBot: Buyer enabled from console");
        }

        break;
    }
    case AHBotCommand::seller:
    {
        char* param1 = strtok(args, " ");
        uint32 state = (uint32)strtoul(param1, NULL, 0);

        if (state == 0)
        {
            config->AHBSeller = false;
            LOG_ERROR("module", "AHBot: Seller disabled from console");
        }
        else
        {
            config->AHBSeller = true;
            LOG_ERROR("module", "AHBot: Seller enabled from console");
        }

        break;
    }
    case AHBotCommand::ahexpire:
    {
        AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(config->GetAHFID());

        AuctionHouseObject::AuctionEntryMap::iterator itr;
        itr = auctionHouse->GetAuctionsBegin();

        while (itr != auctionHouse->GetAuctionsEnd())
        {
            if (itr->second->owner.GetCounter() == _id)
            {
                // Expired NOW.
                itr->second->expire_time = GameTime::GetGameTime().count();

                uint32 id                = itr->second->Id;
                uint32 expire_time       = itr->second->expire_time;

                CharacterDatabase.Execute("UPDATE auctionhouse SET time = '{}' WHERE id = '{}'", expire_time, id);
            }

            ++itr;
        }

        break;
    }
    case AHBotCommand::minitems:
    {
        char * param1   = strtok(args, " ");
        uint32 minItems = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET minitems = '{}' WHERE auctionhouse = '{}'", minItems, ahMapID);

        config->SetMinItems(minItems);

        break;
    }
    case AHBotCommand::maxitems:
    {
        char * param1   = strtok(args, " ");
        uint32 maxItems = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxitems = '{}' WHERE auctionhouse = '{}'", maxItems, ahMapID);

        config->SetMaxItems(maxItems);
        config->CalculatePercents();
        break;
    }
    case AHBotCommand::percentages:
    {
        char * param1   = strtok(args, " ");
        char * param2   = strtok(NULL, " ");
        char * param3   = strtok(NULL, " ");
        char * param4   = strtok(NULL, " ");
        char * param5   = strtok(NULL, " ");
        char * param6   = strtok(NULL, " ");
        char * param7   = strtok(NULL, " ");
        char * param8   = strtok(NULL, " ");
        char * param9   = strtok(NULL, " ");
        char * param10  = strtok(NULL, " ");
        char * param11  = strtok(NULL, " ");
        char * param12  = strtok(NULL, " ");
        char * param13  = strtok(NULL, " ");
        char * param14  = strtok(NULL, " ");

        uint32 greytg   = (uint32) strtoul(param1, NULL, 0);
        uint32 whitetg  = (uint32) strtoul(param2, NULL, 0);
        uint32 greentg  = (uint32) strtoul(param3, NULL, 0);
        uint32 bluetg   = (uint32) strtoul(param4, NULL, 0);
        uint32 purpletg = (uint32) strtoul(param5, NULL, 0);
        uint32 orangetg = (uint32) strtoul(param6, NULL, 0);
        uint32 yellowtg = (uint32) strtoul(param7, NULL, 0);
        uint32 greyi    = (uint32) strtoul(param8, NULL, 0);
        uint32 whitei   = (uint32) strtoul(param9, NULL, 0);
        uint32 greeni   = (uint32) strtoul(param10, NULL, 0);
        uint32 bluei    = (uint32) strtoul(param11, NULL, 0);
        uint32 purplei  = (uint32) strtoul(param12, NULL, 0);
        uint32 orangei  = (uint32) strtoul(param13, NULL, 0);
        uint32 yellowi  = (uint32) strtoul(param14, NULL, 0);

        auto trans = WorldDatabase.BeginTransaction();

        trans->Append("UPDATE mod_auctionhousebot SET percentgreytradegoods = '{}'   WHERE auctionhouse = '{}'", greytg, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentwhitetradegoods = '{}'  WHERE auctionhouse = '{}'", whitetg, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentgreentradegoods = '{}'  WHERE auctionhouse = '{}'", greentg, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentbluetradegoods = '{}'   WHERE auctionhouse = '{}'", bluetg, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentpurpletradegoods = '{}' WHERE auctionhouse = '{}'", purpletg, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentorangetradegoods = '{}' WHERE auctionhouse = '{}'", orangetg, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentyellowtradegoods = '{}' WHERE auctionhouse = '{}'", yellowtg, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentgreyitems = '{}'        WHERE auctionhouse = '{}'", greyi, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentwhiteitems = '{}'       WHERE auctionhouse = '{}'", whitei, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentgreenitems = '{}'       WHERE auctionhouse = '{}'", greeni, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentblueitems = '{}'        WHERE auctionhouse = '{}'", bluei, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentpurpleitems = '{}'      WHERE auctionhouse = '{}'", purplei, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentorangeitems = '{}'      WHERE auctionhouse = '{}'", orangei, ahMapID);
        trans->Append("UPDATE mod_auctionhousebot SET percentyellowitems = '{}'      WHERE auctionhouse = '{}'", yellowi, ahMapID);

        WorldDatabase.CommitTransaction(trans);

        config->SetPercentages(greytg, whitetg, greentg, bluetg, purpletg, orangetg, yellowtg, greyi, whitei, greeni, bluei, purplei, orangei, yellowi);

        break;
    }
    case AHBotCommand::minprice:
    {
        char * param1   = strtok(args, " ");
        uint32 minPrice = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET minprice{} = '{}' WHERE auctionhouse = '{}'", color, minPrice, ahMapID);

        config->SetMinPrice(col, minPrice);

        break;
    }
    case AHBotCommand::maxprice:
    {
        char * param1   = strtok(args, " ");
        uint32 maxPrice = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxprice{} = '{}' WHERE auctionhouse = '{}'", color, maxPrice, ahMapID);

        config->SetMaxPrice(col, maxPrice);

        break;
    }
    case AHBotCommand::minbidprice:
    {
        char * param1      = strtok(args, " ");
        uint32 minBidPrice = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET minbidprice{} = '{}' WHERE auctionhouse = '{}'", color, minBidPrice, ahMapID);

        config->SetMinBidPrice(col, minBidPrice);

        break;
    }
    case AHBotCommand::maxbidprice:
    {
        char * param1      = strtok(args, " ");
        uint32 maxBidPrice = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxbidprice{} = '{}' WHERE auctionhouse = '{}'", color, maxBidPrice, ahMapID);

        config->SetMaxBidPrice(col, maxBidPrice);

        break;
    }
    case AHBotCommand::maxstack:
    {
        char * param1   = strtok(args, " ");
        uint32 maxStack = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxstack{} = '{}' WHERE auctionhouse = '{}'", color, maxStack, ahMapID);

        config->SetMaxStack(col, maxStack);

        break;
    }
    case AHBotCommand::buyerprice:
    {
<<<<<<< HEAD
        //load min and max items TODO: item bonus
		config->SetMinItems(WorldDatabase.Query("SELECT minitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxItems(WorldDatabase.Query("SELECT maxitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        config->SetItemBonus(WorldDatabase.Query("SELECT itemcountbonus FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        //load percentages
        uint32 greytg = WorldDatabase.Query("SELECT percentgreytradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 whitetg = WorldDatabase.Query("SELECT percentwhitetradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 greentg = WorldDatabase.Query("SELECT percentgreentradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 bluetg = WorldDatabase.Query("SELECT percentbluetradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 purpletg = WorldDatabase.Query("SELECT percentpurpletradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 orangetg = WorldDatabase.Query("SELECT percentorangetradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 yellowtg = WorldDatabase.Query("SELECT percentyellowtradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 greyi = WorldDatabase.Query("SELECT percentgreyitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 whitei = WorldDatabase.Query("SELECT percentwhiteitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 greeni = WorldDatabase.Query("SELECT percentgreenitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 bluei = WorldDatabase.Query("SELECT percentblueitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 purplei = WorldDatabase.Query("SELECT percentpurpleitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 orangei = WorldDatabase.Query("SELECT percentorangeitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
		uint32 yellowi = WorldDatabase.Query("SELECT percentyellowitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>();
        config->SetPercentages(greytg, whitetg, greentg, bluetg, purpletg, orangetg, yellowtg, greyi, whitei, greeni, bluei, purplei, orangei, yellowi);
        //load min and max prices
		config->SetMinPrice(AHB_GREY, WorldDatabase.Query("SELECT minpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxPrice(AHB_GREY, WorldDatabase.Query("SELECT maxpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinPrice(AHB_WHITE, WorldDatabase.Query("SELECT minpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxPrice(AHB_WHITE, WorldDatabase.Query("SELECT maxpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinPrice(AHB_GREEN, WorldDatabase.Query("SELECT minpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxPrice(AHB_GREEN, WorldDatabase.Query("SELECT maxpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinPrice(AHB_BLUE, WorldDatabase.Query("SELECT minpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxPrice(AHB_BLUE, WorldDatabase.Query("SELECT maxpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinPrice(AHB_PURPLE, WorldDatabase.Query("SELECT minpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxPrice(AHB_PURPLE, WorldDatabase.Query("SELECT maxpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinPrice(AHB_ORANGE, WorldDatabase.Query("SELECT minpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxPrice(AHB_ORANGE, WorldDatabase.Query("SELECT maxpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinPrice(AHB_YELLOW, WorldDatabase.Query("SELECT minpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxPrice(AHB_YELLOW, WorldDatabase.Query("SELECT maxpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        //load min and max bid prices
		config->SetMinBidPrice(AHB_GREY, WorldDatabase.Query("SELECT minbidpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxBidPrice(AHB_GREY, WorldDatabase.Query("SELECT maxbidpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinBidPrice(AHB_WHITE, WorldDatabase.Query("SELECT minbidpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxBidPrice(AHB_WHITE, WorldDatabase.Query("SELECT maxbidpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinBidPrice(AHB_GREEN, WorldDatabase.Query("SELECT minbidpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxBidPrice(AHB_GREEN, WorldDatabase.Query("SELECT maxbidpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinBidPrice(AHB_BLUE, WorldDatabase.Query("SELECT minbidpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxBidPrice(AHB_BLUE, WorldDatabase.Query("SELECT maxbidpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinBidPrice(AHB_PURPLE, WorldDatabase.Query("SELECT minbidpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxBidPrice(AHB_PURPLE, WorldDatabase.Query("SELECT maxbidpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinBidPrice(AHB_ORANGE, WorldDatabase.Query("SELECT minbidpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxBidPrice(AHB_ORANGE, WorldDatabase.Query("SELECT maxbidpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMinBidPrice(AHB_YELLOW, WorldDatabase.Query("SELECT minbidpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxBidPrice(AHB_YELLOW, WorldDatabase.Query("SELECT maxbidpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        //load max stacks
		config->SetMaxStack(AHB_GREY, WorldDatabase.Query("SELECT maxstackgrey FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxStack(AHB_WHITE, WorldDatabase.Query("SELECT maxstackwhite FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxStack(AHB_GREEN, WorldDatabase.Query("SELECT maxstackgreen FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxStack(AHB_BLUE, WorldDatabase.Query("SELECT maxstackblue FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxStack(AHB_PURPLE, WorldDatabase.Query("SELECT maxstackpurple FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxStack(AHB_ORANGE, WorldDatabase.Query("SELECT maxstackorange FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxStack(AHB_YELLOW, WorldDatabase.Query("SELECT maxstackyellow FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        if (debug_Out)
        {
            LOG_ERROR("module", "minItems                = {}", config->GetMinItems());
            LOG_ERROR("module", "maxItems                = {}", config->GetMaxItems());
            LOG_ERROR("module", "minItemsTotal           = {}", config->GetMinItemsTotal());
            LOG_ERROR("module", "maxItemsTotal           = {}", config->GetMaxItemsTotal());
            LOG_ERROR("module", "percentGreyTradeGoods   = {}", config->GetPercentages(AHB_GREY_TG));
            LOG_ERROR("module", "percentWhiteTradeGoods  = {}", config->GetPercentages(AHB_WHITE_TG));
            LOG_ERROR("module", "percentGreenTradeGoods  = {}", config->GetPercentages(AHB_GREEN_TG));
            LOG_ERROR("module", "percentBlueTradeGoods   = {}", config->GetPercentages(AHB_BLUE_TG));
            LOG_ERROR("module", "percentPurpleTradeGoods = {}", config->GetPercentages(AHB_PURPLE_TG));
            LOG_ERROR("module", "percentOrangeTradeGoods = {}", config->GetPercentages(AHB_ORANGE_TG));
            LOG_ERROR("module", "percentYellowTradeGoods = {}", config->GetPercentages(AHB_YELLOW_TG));
            LOG_ERROR("module", "percentGreyItems        = {}", config->GetPercentages(AHB_GREY_I));
            LOG_ERROR("module", "percentWhiteItems       = {}", config->GetPercentages(AHB_WHITE_I));
            LOG_ERROR("module", "percentGreenItems       = {}", config->GetPercentages(AHB_GREEN_I));
            LOG_ERROR("module", "percentBlueItems        = {}", config->GetPercentages(AHB_BLUE_I));
            LOG_ERROR("module", "percentPurpleItems      = {}", config->GetPercentages(AHB_PURPLE_I));
            LOG_ERROR("module", "percentOrangeItems      = {}", config->GetPercentages(AHB_ORANGE_I));
            LOG_ERROR("module", "percentYellowItems      = {}", config->GetPercentages(AHB_YELLOW_I));
            LOG_ERROR("module", "minPriceGrey            = {}", config->GetMinPrice(AHB_GREY));
            LOG_ERROR("module", "maxPriceGrey            = {}", config->GetMaxPrice(AHB_GREY));
            LOG_ERROR("module", "minPriceWhite           = {}", config->GetMinPrice(AHB_WHITE));
            LOG_ERROR("module", "maxPriceWhite           = {}", config->GetMaxPrice(AHB_WHITE));
            LOG_ERROR("module", "minPriceGreen           = {}", config->GetMinPrice(AHB_GREEN));
            LOG_ERROR("module", "maxPriceGreen           = {}", config->GetMaxPrice(AHB_GREEN));
            LOG_ERROR("module", "minPriceBlue            = {}", config->GetMinPrice(AHB_BLUE));
            LOG_ERROR("module", "maxPriceBlue            = {}", config->GetMaxPrice(AHB_BLUE));
            LOG_ERROR("module", "minPricePurple          = {}", config->GetMinPrice(AHB_PURPLE));
            LOG_ERROR("module", "maxPricePurple          = {}", config->GetMaxPrice(AHB_PURPLE));
            LOG_ERROR("module", "minPriceOrange          = {}", config->GetMinPrice(AHB_ORANGE));
            LOG_ERROR("module", "maxPriceOrange          = {}", config->GetMaxPrice(AHB_ORANGE));
            LOG_ERROR("module", "minPriceYellow          = {}", config->GetMinPrice(AHB_YELLOW));
            LOG_ERROR("module", "maxPriceYellow          = {}", config->GetMaxPrice(AHB_YELLOW));
            LOG_ERROR("module", "minBidPriceGrey         = {}", config->GetMinBidPrice(AHB_GREY));
            LOG_ERROR("module", "maxBidPriceGrey         = {}", config->GetMaxBidPrice(AHB_GREY));
            LOG_ERROR("module", "minBidPriceWhite        = {}", config->GetMinBidPrice(AHB_WHITE));
            LOG_ERROR("module", "maxBidPriceWhite        = {}", config->GetMaxBidPrice(AHB_WHITE));
            LOG_ERROR("module", "minBidPriceGreen        = {}", config->GetMinBidPrice(AHB_GREEN));
            LOG_ERROR("module", "maxBidPriceGreen        = {}", config->GetMaxBidPrice(AHB_GREEN));
            LOG_ERROR("module", "minBidPriceBlue         = {}", config->GetMinBidPrice(AHB_BLUE));
            LOG_ERROR("module", "maxBidPriceBlue         = {}", config->GetMinBidPrice(AHB_BLUE));
            LOG_ERROR("module", "minBidPricePurple       = {}", config->GetMinBidPrice(AHB_PURPLE));
            LOG_ERROR("module", "maxBidPricePurple       = {}", config->GetMaxBidPrice(AHB_PURPLE));
            LOG_ERROR("module", "minBidPriceOrange       = {}", config->GetMinBidPrice(AHB_ORANGE));
            LOG_ERROR("module", "maxBidPriceOrange       = {}", config->GetMaxBidPrice(AHB_ORANGE));
            LOG_ERROR("module", "minBidPriceYellow       = {}", config->GetMinBidPrice(AHB_YELLOW));
            LOG_ERROR("module", "maxBidPriceYellow       = {}", config->GetMaxBidPrice(AHB_YELLOW));
            LOG_ERROR("module", "maxStackGrey            = {}", config->GetMaxStack(AHB_GREY));
            LOG_ERROR("module", "maxStackWhite           = {}", config->GetMaxStack(AHB_WHITE));
            LOG_ERROR("module", "maxStackGreen           = {}", config->GetMaxStack(AHB_GREEN));
            LOG_ERROR("module", "maxStackBlue            = {}", config->GetMaxStack(AHB_BLUE));
            LOG_ERROR("module", "maxStackPurple          = {}", config->GetMaxStack(AHB_PURPLE));
            LOG_ERROR("module", "maxStackOrange          = {}", config->GetMaxStack(AHB_ORANGE));
            LOG_ERROR("module", "maxStackYellow          = {}", config->GetMaxStack(AHB_YELLOW));
        }
=======
        char * param1     = strtok(args, " ");
        uint32 buyerPrice = (uint32) strtoul(param1, NULL, 0);
>>>>>>> 450239ca6bc4d208881c053b1189fd74534e35a4

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET buyerprice{} = '{}' WHERE auctionhouse = '{}'", color, buyerPrice, ahMapID);

        config->SetBuyerPrice(col, buyerPrice);

        break;
    }
    case AHBotCommand::bidinterval:
    {
        char * param1      = strtok(args, " ");
        uint32 bidInterval = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET buyerbiddinginterval = '{}' WHERE auctionhouse = '{}'", bidInterval, ahMapID);

        config->SetBiddingInterval(bidInterval);

        break;
    }
    case AHBotCommand::bidsperinterval:
    {
        char * param1          = strtok(args, " ");
        uint32 bidsPerInterval = (uint32) strtoul(param1, NULL, 0);

        WorldDatabase.Execute("UPDATE mod_auctionhousebot SET buyerbidsperinterval = '{}' WHERE auctionhouse = '{}'", bidsPerInterval, ahMapID);

        config->SetBidsPerInterval(bidsPerInterval);

        break;
    }
    default:
        break;
    }
}

// =============================================================================
// Initialization of the bot
// =============================================================================

void AuctionHouseBot::Initialize(AHBConfig* allianceConfig, AHBConfig* hordeConfig, AHBConfig* neutralConfig)
{
    // 
    // Save the pointer for the configurations
    // 

    _allianceConfig = allianceConfig;
    _hordeConfig    = hordeConfig;
    _neutralConfig  = neutralConfig;

    //
    // Done
    //

    LOG_INFO("module", "AHBot [{}]: initialization complete", uint32(_id));
}
