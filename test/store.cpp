//
// Created by lukeh on 01/11/2019.
//

#include "../internal_includes/test_utils.h"
#include <FacebookAdStore.h>
#include <gtest/gtest.h>
#include <util_time.h>

using namespace nstimestamp;

class TestFacebookStore: public ::testing::Test {
public:
    TestFacebookStore() {
        FacebookAd& ad = ads.emplace_back();
        ad.id = 0;
        ad.creationTime = nstimestamp::Time("2019-10-29T17:16:59+0000");
        ad.fundingEntity = "Entity#0";
        ad.pageName = "Page#0";
        ad.linkTitles = {"Title#0"};
        ad.linkCaptions = {"Caption#0"};
        ad.linkDescriptions = {"Description#0"};
        ad.bodies = {"Body#0"};

        FacebookAd& ad1 = ads.emplace_back();
        ad1.id = 1;
        ad1.creationTime = nstimestamp::Time("2019-10-29T18:16:59+0000");
        ad1.fundingEntity = "Entity#1";
        ad1.pageName = "Page#1";
        ad1.linkTitles = {"Title#1"};
        ad1.linkCaptions = {"Caption#1"};
        ad1.linkDescriptions = {"Description#1"};
        ad1.bodies = {"Body#1"};
    }
protected:
    std::vector<FacebookAd> ads;
    FacebookAdStore theStore;
};

TEST_F(TestFacebookStore, StoredUppers) {
    FacebookAd ad;
    ad.id = 0;
    ad.fundingEntity = "Funding Entity";
    ad.bodies = {"Body", "Another Body"};
    ad.linkDescriptions = {"Link Description", "Another"};
    ad.linkTitles = {"Link Title", "Another"};
    ad.linkCaptions = {"Link Caption", "Another"};
    ad.pageName = "Page Name";
    StoredFacebookAd storedAd(std::make_unique<FacebookAd>(ad));

    const std::vector<std::string> exp_bodies = {"BODY", "ANOTHER BODY"};
    const std::vector<std::string> exp_captions = {"LINK CAPTION", "ANOTHER"};
    const std::vector<std::string> exp_titles = {"LINK TITLE", "ANOTHER"};
    const std::vector<std::string> exp_descriptions = {"LINK DESCRIPTION", "ANOTHER"};

    ASSERT_EQ(storedAd.CachedUppers().fundingEntity, "FUNDING ENTITY");
    ASSERT_EQ(storedAd.CachedUppers().bodies, exp_bodies);
    ASSERT_EQ(storedAd.CachedUppers().linkTitles, exp_titles);
    ASSERT_EQ(storedAd.CachedUppers().linkCaptions, exp_captions);
    ASSERT_EQ(storedAd.CachedUppers().linkDescriptions, exp_descriptions);
    ASSERT_EQ(storedAd.CachedUppers().pageName, "PAGE NAME");
}

TEST_F(TestFacebookStore, StoredItem) {
    StoredFacebookAd storedAd(std::make_unique<FacebookAd>(ads[0]));
    AssertEq(ads[0], storedAd.ItemRef());
}

TEST_F(TestFacebookStore, InvalidDeSerialization) {
    StoredFacebookAd invalid (StoredFacebookAd::Serialization{"{"});
    ASSERT_TRUE(invalid.IsNull());
}

TEST_F(TestFacebookStore, Serialization_StoredItem) {
    StoredFacebookAd storedAd(std::make_unique<FacebookAd>(ads[0]));
    const auto key = storedAd.Key();
    auto serialization = storedAd.Serialize();
    StoredFacebookAd copy(serialization);
    ASSERT_FALSE(copy.IsNull());
    AssertEq(ads[0], copy.ItemRef());
    ASSERT_EQ(copy.Key(), key);
}

TEST_F(TestFacebookStore, Serialization_StoredUppers) {
    FacebookAd ad;
    ad.id = 0;
    ad.fundingEntity = "Funding Entity";
    ad.bodies = {"Body", "Another Body"};
    ad.linkDescriptions = {"Link Description", "Another"};
    ad.linkTitles = {"Link Title", "Another"};
    ad.linkCaptions = {"Link Caption", "Another"};
    ad.pageName = "Page Name";
    StoredFacebookAd storedAd(std::make_unique<FacebookAd>(ad));

    const std::vector<std::string> exp_bodies = {"BODY", "ANOTHER BODY"};
    const std::vector<std::string> exp_captions = {"LINK CAPTION", "ANOTHER"};
    const std::vector<std::string> exp_titles = {"LINK TITLE", "ANOTHER"};
    const std::vector<std::string> exp_descriptions = {"LINK DESCRIPTION", "ANOTHER"};

    auto serialization = storedAd.Serialize();
    StoredFacebookAd copy(serialization);

    ASSERT_EQ(copy.CachedUppers().fundingEntity, "FUNDING ENTITY");
    ASSERT_EQ(copy.CachedUppers().pageName, "PAGE NAME");
    ASSERT_EQ(copy.CachedUppers().bodies, exp_bodies);
    ASSERT_EQ(copy.CachedUppers().linkDescriptions, exp_descriptions);
    ASSERT_EQ(copy.CachedUppers().linkTitles, exp_titles);
    ASSERT_EQ(copy.CachedUppers().linkCaptions, exp_captions);
}

TEST_F(TestFacebookStore, NoSuchItem) {
    ASSERT_TRUE(theStore.Get(99999).IsNull());
}

TEST_F(TestFacebookStore, InitialStore) {
    auto& ref = theStore.Store(std::make_unique<FacebookAd>(ads[0]));
    ASSERT_FALSE(ref.IsNull());

    AssertEq(ads[0], ref.ItemRef());
}

TEST_F(TestFacebookStore, RetrieveFromStore) {
    std::vector<StoredFacebookAd::KeyType> keys = {
        theStore.Store(std::make_unique<FacebookAd>(ads[0])).Key(),
        theStore.Store(std::make_unique<FacebookAd>(ads[1])).Key(),
    };
    for (size_t i = 0; i < keys.size(); ++i) {
        const auto& key = keys[i];
        const auto& ad = ads[i];
        auto& ref = theStore.Get(key);
        ASSERT_FALSE(ref.IsNull());
        AssertEq(ad, ref.ItemRef());
    }
}

TEST_F(TestFacebookStore, UpdateItem) {
    std::vector<StoredFacebookAd::KeyType> keys = {
            theStore.Store(std::make_unique<FacebookAd>(ads[0])).Key(),
            theStore.Store(std::make_unique<FacebookAd>(ads[1])).Key(),
    };

    std::vector<FacebookAd> updatedAds {
        ads[0],
        ads[1]
    };

    updatedAds[0].regionDist.try_emplace(RegionCode::ENGLAND, 1.0);
    updatedAds[1].spend.lower_bound = 3000;
    updatedAds[1].spend.upper_bound = 3999;

    theStore.Store(std::make_unique<FacebookAd>(updatedAds[0]));
    theStore.Store(std::make_unique<FacebookAd>(updatedAds[1]));


    for (size_t i = 0; i < keys.size(); ++i) {
        const auto& key = keys[i];
        const auto& ad = updatedAds[i];
        auto& ref = theStore.Get(key);
        ASSERT_FALSE(ref.IsNull());
        AssertEq(ad, ref.ItemRef());
    }

}

TEST_F(TestFacebookStore, PatchNoKeyChange) {
    StoredFacebookAd storedAd { std::make_shared<FacebookAd>(ads[0])};

    FacebookAd updatedAd(ads[0]);
    updatedAd.id = 77;

    // NOTE: This shouldn't actually happen without a code error in the actual store...
    ASSERT_THROW(storedAd.PatchStoredValues(std::move(updatedAd)), StoredFacebookAd::KeyChangeError);


}

TEST_F(TestFacebookStore, ForEach) {
    std::vector<StoredFacebookAd::KeyType> keys = {
            theStore.Store(std::make_unique<FacebookAd>(ads[0])).Key(),
            theStore.Store(std::make_unique<FacebookAd>(ads[1])).Key(),
    };
    std::vector<std::shared_ptr<const FacebookAd>> storedAds;
    theStore.ForEach([&] (const StoredFacebookAd& ad) -> auto {
        storedAds.emplace_back(ad.NewSharedRef());
        return FacebookAdStore::ScanOp::CONTINUE;
    });

    for (size_t i = 0; i < keys.size(); ++i) {
        const auto& ad = ads[i];
        const auto& copyAd = storedAds[i];
        AssertEq(ad, *copyAd);
    }
}

TEST_F(TestFacebookStore, ForEach_Stopped) {
    std::vector<StoredFacebookAd::KeyType> keys = {
            theStore.Store(std::make_unique<FacebookAd>(ads[0])).Key(),
            theStore.Store(std::make_unique<FacebookAd>(ads[1])).Key(),
    };
    std::vector<std::shared_ptr<const FacebookAd>> storedAds;
    theStore.ForEach([&] (const StoredFacebookAd& ad) -> auto {
        storedAds.emplace_back(ad.NewSharedRef());
        return FacebookAdStore::ScanOp::STOP;
    });

    ASSERT_EQ(storedAds.size(), 1);
    AssertEq(ads[0], *storedAds[0]);
}

TEST_F(TestFacebookStore, Serialize) {
    std::vector<StoredFacebookAd::KeyType> keys = {
            theStore.Store(std::make_unique<FacebookAd>(ads[0])).Key(),
            theStore.Store(std::make_unique<FacebookAd>(ads[1])).Key(),
    };

    auto serialization = theStore.Serialize();

    FacebookAdStore copy(serialization);

    for (size_t i = 0; i < keys.size(); ++i) {
        const auto& key = keys[i];
        const auto& ad = ads[i];
        auto& ref = copy.Get(key);
        ASSERT_FALSE(ref.IsNull());
        AssertEq(ad, ref.ItemRef());
    }
}

TEST_F(TestFacebookStore, InvalidSerialization) {
    std::vector<StoredFacebookAd::KeyType> keys = {
            theStore.Store(std::make_unique<FacebookAd>(ads[0])).Key(),
            theStore.Store(std::make_unique<FacebookAd>(ads[1])).Key(),
    };

    auto serialization = theStore.Serialize();
    serialization.data = "{";

    ASSERT_THROW((FacebookAdStore(serialization)), FacebookAdStore::InvalidSerialization);
}
