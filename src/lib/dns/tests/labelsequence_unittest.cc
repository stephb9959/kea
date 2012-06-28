// Copyright (C) 2012  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <dns/labelsequence.h>
#include <dns/name.h>
#include <exceptions/exceptions.h>

#include <gtest/gtest.h>

#include <boost/functional/hash.hpp>

#include <string>
#include <set>

using namespace isc::dns;
using namespace std;

namespace {

class LabelSequenceTest : public ::testing::Test {
public:
    LabelSequenceTest() : n1("example.org"), n2("example.com"),
                          n3("example.org"), n4("foo.bar.test.example"),
                          n5("example.ORG"), n6("ExAmPlE.org"),
                          n7("."), n8("foo.example.org.bar"),
                          ls1(n1), ls2(n2), ls3(n3), ls4(n4), ls5(n5),
                          ls6(n6), ls7(n7), ls8(n8)
    {};
    // Need to keep names in scope for at least the lifetime of
    // the labelsequences
    Name n1, n2, n3, n4, n5, n6, n7, n8;

    LabelSequence ls1, ls2, ls3, ls4, ls5, ls6, ls7, ls8;
};

// Basic equality tests
TEST_F(LabelSequenceTest, equals_sensitive) {
    EXPECT_TRUE(ls1.equals(ls1, true));
    EXPECT_FALSE(ls1.equals(ls2, true));
    EXPECT_TRUE(ls1.equals(ls3, true));
    EXPECT_FALSE(ls1.equals(ls4, true));
    EXPECT_FALSE(ls1.equals(ls5, true));
    EXPECT_FALSE(ls1.equals(ls6, true));
    EXPECT_FALSE(ls1.equals(ls7, true));
    EXPECT_FALSE(ls1.equals(ls8, true));

    EXPECT_FALSE(ls2.equals(ls1, true));
    EXPECT_TRUE(ls2.equals(ls2, true));
    EXPECT_FALSE(ls2.equals(ls3, true));
    EXPECT_FALSE(ls2.equals(ls4, true));
    EXPECT_FALSE(ls2.equals(ls5, true));
    EXPECT_FALSE(ls2.equals(ls6, true));
    EXPECT_FALSE(ls2.equals(ls7, true));
    EXPECT_FALSE(ls2.equals(ls8, true));

    EXPECT_FALSE(ls4.equals(ls1, true));
    EXPECT_FALSE(ls4.equals(ls2, true));
    EXPECT_FALSE(ls4.equals(ls3, true));
    EXPECT_TRUE(ls4.equals(ls4, true));
    EXPECT_FALSE(ls4.equals(ls5, true));
    EXPECT_FALSE(ls4.equals(ls6, true));
    EXPECT_FALSE(ls4.equals(ls7, true));
    EXPECT_FALSE(ls4.equals(ls8, true));

    EXPECT_FALSE(ls5.equals(ls1, true));
    EXPECT_FALSE(ls5.equals(ls2, true));
    EXPECT_FALSE(ls5.equals(ls3, true));
    EXPECT_FALSE(ls5.equals(ls4, true));
    EXPECT_TRUE(ls5.equals(ls5, true));
    EXPECT_FALSE(ls5.equals(ls6, true));
    EXPECT_FALSE(ls5.equals(ls7, true));
    EXPECT_FALSE(ls5.equals(ls8, true));
}

TEST_F(LabelSequenceTest, equals_insensitive) {
    EXPECT_TRUE(ls1.equals(ls1));
    EXPECT_FALSE(ls1.equals(ls2));
    EXPECT_TRUE(ls1.equals(ls3));
    EXPECT_FALSE(ls1.equals(ls4));
    EXPECT_TRUE(ls1.equals(ls5));
    EXPECT_TRUE(ls1.equals(ls6));
    EXPECT_FALSE(ls1.equals(ls7));

    EXPECT_FALSE(ls2.equals(ls1));
    EXPECT_TRUE(ls2.equals(ls2));
    EXPECT_FALSE(ls2.equals(ls3));
    EXPECT_FALSE(ls2.equals(ls4));
    EXPECT_FALSE(ls2.equals(ls5));
    EXPECT_FALSE(ls2.equals(ls6));
    EXPECT_FALSE(ls2.equals(ls7));

    EXPECT_TRUE(ls3.equals(ls1));
    EXPECT_FALSE(ls3.equals(ls2));
    EXPECT_TRUE(ls3.equals(ls3));
    EXPECT_FALSE(ls3.equals(ls4));
    EXPECT_TRUE(ls3.equals(ls5));
    EXPECT_TRUE(ls3.equals(ls6));
    EXPECT_FALSE(ls3.equals(ls7));

    EXPECT_FALSE(ls4.equals(ls1));
    EXPECT_FALSE(ls4.equals(ls2));
    EXPECT_FALSE(ls4.equals(ls3));
    EXPECT_TRUE(ls4.equals(ls4));
    EXPECT_FALSE(ls4.equals(ls5));
    EXPECT_FALSE(ls4.equals(ls6));
    EXPECT_FALSE(ls4.equals(ls7));

    EXPECT_TRUE(ls5.equals(ls1));
    EXPECT_FALSE(ls5.equals(ls2));
    EXPECT_TRUE(ls5.equals(ls3));
    EXPECT_FALSE(ls5.equals(ls4));
    EXPECT_TRUE(ls5.equals(ls5));
    EXPECT_TRUE(ls5.equals(ls6));
    EXPECT_FALSE(ls5.equals(ls7));
}

// Compare tests
TEST_F(LabelSequenceTest, compare) {
    // "example.org." and "example.org.", case sensitive
    NameComparisonResult result = ls1.compare(ls3, true);
    EXPECT_EQ(isc::dns::NameComparisonResult::EQUAL,
              result.getRelation());
    EXPECT_EQ(3, result.getCommonLabels());

    // "example.org." and "example.ORG.", case sensitive
    result = ls3.compare(ls5, true);
    EXPECT_EQ(isc::dns::NameComparisonResult::COMMONANCESTOR,
              result.getRelation());
    EXPECT_EQ(1, result.getCommonLabels());

    // "example.org." and "example.ORG.", case in-sensitive
    result = ls3.compare(ls5);
    EXPECT_EQ(isc::dns::NameComparisonResult::EQUAL,
              result.getRelation());
    EXPECT_EQ(3, result.getCommonLabels());

    Name na("a.example.org");
    Name nb("b.example.org");
    LabelSequence lsa(na);
    LabelSequence lsb(nb);

    // "a.example.org." and "b.example.org.", case in-sensitive
    result = lsa.compare(lsb);
    EXPECT_EQ(isc::dns::NameComparisonResult::COMMONANCESTOR,
              result.getRelation());
    EXPECT_EQ(3, result.getCommonLabels());

    // "example.org." and "b.example.org.", case in-sensitive
    lsa.stripLeft(1);
    result = lsa.compare(lsb);
    EXPECT_EQ(isc::dns::NameComparisonResult::SUPERDOMAIN,
              result.getRelation());
    EXPECT_EQ(3, result.getCommonLabels());

    Name nc("g.f.e.d.c.example.org");
    LabelSequence lsc(nc);

    // "g.f.e.d.c.example.org." and "b.example.org" (not absolute), case
    // in-sensitive
    lsb.stripRight(1);
    result = lsc.compare(lsb);
    EXPECT_EQ(isc::dns::NameComparisonResult::NONE,
              result.getRelation());
    EXPECT_EQ(0, result.getCommonLabels());
    EXPECT_EQ(0, result.getOrder());

    // "g.f.e.d.c.example.org." and "example.org.", case in-sensitive
    result = lsc.compare(ls1);
    EXPECT_EQ(isc::dns::NameComparisonResult::SUBDOMAIN,
              result.getRelation());
    EXPECT_EQ(3, result.getCommonLabels());

    // "e.d.c.example.org." and "example.org.", case in-sensitive
    lsc.stripLeft(2);
    result = lsc.compare(ls1);
    EXPECT_EQ(isc::dns::NameComparisonResult::SUBDOMAIN,
              result.getRelation());
    EXPECT_EQ(3, result.getCommonLabels());

    // "example.org." and "example.org.", case in-sensitive
    lsc.stripLeft(3);
    result = lsc.compare(ls1);
    EXPECT_EQ(isc::dns::NameComparisonResult::EQUAL,
              result.getRelation());
    EXPECT_EQ(3, result.getCommonLabels());

    // "." and "example.org.", case in-sensitive
    lsc.stripLeft(2);
    result = lsc.compare(ls1);
    EXPECT_EQ(isc::dns::NameComparisonResult::SUPERDOMAIN,
              result.getRelation());
    EXPECT_EQ(1, result.getCommonLabels());

    Name nd("a.b.c.isc.example.org");
    LabelSequence lsd(nd);
    Name ne("w.x.y.isc.EXAMPLE.org");
    LabelSequence lse(ne);

    // "a.b.c.isc.example.org." and "w.x.y.isc.EXAMPLE.org.",
    // case sensitive
    result = lsd.compare(lse, true);
    EXPECT_EQ(isc::dns::NameComparisonResult::COMMONANCESTOR,
              result.getRelation());
    EXPECT_EQ(2, result.getCommonLabels());

    // "a.b.c.isc.example.org." and "w.x.y.isc.EXAMPLE.org.",
    // case in-sensitive
    result = lsd.compare(lse);
    EXPECT_EQ(isc::dns::NameComparisonResult::COMMONANCESTOR,
              result.getRelation());
    EXPECT_EQ(4, result.getCommonLabels());

    // "isc.example.org." and "isc.EXAMPLE.org.", case sensitive
    lsd.stripLeft(3);
    lse.stripLeft(3);
    result = lsd.compare(lse, true);
    EXPECT_EQ(isc::dns::NameComparisonResult::COMMONANCESTOR,
              result.getRelation());
    EXPECT_EQ(2, result.getCommonLabels());

    // "isc.example.org." and "isc.EXAMPLE.org.", case in-sensitive
    result = lsd.compare(lse);
    EXPECT_EQ(isc::dns::NameComparisonResult::EQUAL,
              result.getRelation());
    EXPECT_EQ(4, result.getCommonLabels());

    Name nf("a.b.c.isc.example.org");
    LabelSequence lsf(nf);
    Name ng("w.x.y.isc.EXAMPLE.org");
    LabelSequence lsg(ng);

    // "a.b.c.isc.example.org." and "w.x.y.isc.EXAMPLE.org" (not
    // absolute), case in-sensitive
    lsg.stripRight(1);
    result = lsg.compare(lsf);
    EXPECT_EQ(isc::dns::NameComparisonResult::NONE,
              result.getRelation());
    EXPECT_EQ(0, result.getCommonLabels());

    // "a.b.c.isc.example.org" (not absolute) and
    // "w.x.y.isc.EXAMPLE.org" (not absolute), case in-sensitive
    lsf.stripRight(1);
    result = lsg.compare(lsf);
    EXPECT_EQ(isc::dns::NameComparisonResult::COMMONANCESTOR,
              result.getRelation());
    EXPECT_EQ(3, result.getCommonLabels());

    // "a.b.c.isc.example" (not absolute) and
    // "w.x.y.isc.EXAMPLE" (not absolute), case in-sensitive
    lsf.stripRight(1);
    lsg.stripRight(1);
    result = lsg.compare(lsf);
    EXPECT_EQ(isc::dns::NameComparisonResult::COMMONANCESTOR,
              result.getRelation());
    EXPECT_EQ(2, result.getCommonLabels());

    // "a.b.c" (not absolute) and
    // "w.x.y" (not absolute), case in-sensitive
    lsf.stripRight(2);
    lsg.stripRight(2);
    result = lsg.compare(lsf);
    EXPECT_EQ(isc::dns::NameComparisonResult::NONE,
              result.getRelation());
    EXPECT_EQ(0, result.getCommonLabels());

    Name nh("aexample.org");
    LabelSequence lsh(nh);
    Name ni("bexample.org");
    LabelSequence lsi(ni);

    // "aexample.org" (not absolute) and
    // "bexample.org" (not absolute), case in-sensitive
    lsh.stripRight(1);
    lsi.stripRight(1);
    result = lsh.compare(lsi);
    EXPECT_EQ(isc::dns::NameComparisonResult::COMMONANCESTOR,
              result.getRelation());
    EXPECT_EQ(1, result.getCommonLabels());

    // "aexample" (not absolute) and
    // "bexample" (not absolute), case in-sensitive
    lsh.stripRight(1);
    lsi.stripRight(1);
    result = lsh.compare(lsi);
    EXPECT_EQ(isc::dns::NameComparisonResult::NONE,
              result.getRelation());
    EXPECT_EQ(0, result.getCommonLabels());

    Name nj("example.org");
    LabelSequence lsj(nj);
    Name nk("example.org");
    LabelSequence lsk(nk);

    // "example.org" (not absolute) and
    // "example.org" (not absolute), case in-sensitive
    lsj.stripRight(1);
    lsk.stripRight(1);
    result = lsj.compare(lsk);
    EXPECT_EQ(isc::dns::NameComparisonResult::EQUAL,
              result.getRelation());
    EXPECT_EQ(2, result.getCommonLabels());

    // "example" (not absolute) and
    // "example" (not absolute), case in-sensitive
    lsj.stripRight(1);
    lsk.stripRight(1);
    result = lsj.compare(lsk);
    EXPECT_EQ(isc::dns::NameComparisonResult::EQUAL,
              result.getRelation());
    EXPECT_EQ(1, result.getCommonLabels());
}

void
getDataCheck(const char* expected_data, size_t expected_len,
             const LabelSequence& ls)
{
    size_t len;
    const char* data = ls.getData(&len);
    ASSERT_EQ(expected_len, len) << "Expected data: " << expected_data <<
                                    " name: " << ls.getName().toText();
    EXPECT_EQ(expected_len, ls.getDataLength()) <<
        "Expected data: " << expected_data <<
        " name: " << ls.getName().toText();
    for (size_t i = 0; i < len; ++i) {
        EXPECT_EQ(expected_data[i], data[i]) << "Difference at pos " << i <<
                                                ": Expected data: " <<
                                                expected_data <<
                                                " name: " <<
                                                ls.getName().toText();;
    }
}

TEST_F(LabelSequenceTest, getData) {
    getDataCheck("\007example\003org\000", 13, ls1);
    getDataCheck("\007example\003com\000", 13, ls2);
    getDataCheck("\007example\003org\000", 13, ls3);
    getDataCheck("\003foo\003bar\004test\007example\000", 22, ls4);
    getDataCheck("\007example\003ORG\000", 13, ls5);
    getDataCheck("\007ExAmPlE\003org\000", 13, ls6);
    getDataCheck("\000", 1, ls7);
};

TEST_F(LabelSequenceTest, stripLeft) {
    EXPECT_TRUE(ls1.equals(ls3));
    ls1.stripLeft(0);
    getDataCheck("\007example\003org\000", 13, ls1);
    EXPECT_TRUE(ls1.equals(ls3));
    ls1.stripLeft(1);
    getDataCheck("\003org\000", 5, ls1);
    EXPECT_FALSE(ls1.equals(ls3));
    ls1.stripLeft(1);
    getDataCheck("\000", 1, ls1);
    EXPECT_TRUE(ls1.equals(ls7));

    ls2.stripLeft(2);
    getDataCheck("\000", 1, ls2);
    EXPECT_TRUE(ls2.equals(ls7));
}

TEST_F(LabelSequenceTest, stripRight) {
    EXPECT_TRUE(ls1.equals(ls3));
    ls1.stripRight(1);
    getDataCheck("\007example\003org", 12, ls1);
    EXPECT_FALSE(ls1.equals(ls3));
    ls1.stripRight(1);
    getDataCheck("\007example", 8, ls1);
    EXPECT_FALSE(ls1.equals(ls3));

    ASSERT_FALSE(ls1.equals(ls2));
    ls2.stripRight(2);
    getDataCheck("\007example", 8, ls2);
    EXPECT_TRUE(ls1.equals(ls2));
}

TEST_F(LabelSequenceTest, stripOutOfRange) {
    EXPECT_THROW(ls1.stripLeft(100), isc::OutOfRange);
    EXPECT_THROW(ls1.stripLeft(5), isc::OutOfRange);
    EXPECT_THROW(ls1.stripLeft(4), isc::OutOfRange);
    EXPECT_THROW(ls1.stripLeft(3), isc::OutOfRange);
    getDataCheck("\007example\003org\000", 13, ls1);

    EXPECT_THROW(ls1.stripRight(100), isc::OutOfRange);
    EXPECT_THROW(ls1.stripRight(5), isc::OutOfRange);
    EXPECT_THROW(ls1.stripRight(4), isc::OutOfRange);
    EXPECT_THROW(ls1.stripRight(3), isc::OutOfRange);
    getDataCheck("\007example\003org\000", 13, ls1);
}

TEST_F(LabelSequenceTest, getLabelCount) {
    EXPECT_EQ(3, ls1.getLabelCount());
    ls1.stripLeft(0);
    EXPECT_EQ(3, ls1.getLabelCount());
    ls1.stripLeft(1);
    EXPECT_EQ(2, ls1.getLabelCount());
    ls1.stripLeft(1);
    EXPECT_EQ(1, ls1.getLabelCount());

    EXPECT_EQ(3, ls2.getLabelCount());
    ls2.stripRight(1);
    EXPECT_EQ(2, ls2.getLabelCount());
    ls2.stripRight(1);
    EXPECT_EQ(1, ls2.getLabelCount());

    EXPECT_EQ(3, ls3.getLabelCount());
    ls3.stripRight(2);
    EXPECT_EQ(1, ls3.getLabelCount());

    EXPECT_EQ(5, ls4.getLabelCount());
    ls4.stripRight(3);
    EXPECT_EQ(2, ls4.getLabelCount());

    EXPECT_EQ(3, ls5.getLabelCount());
    ls5.stripLeft(2);
    EXPECT_EQ(1, ls5.getLabelCount());
}

TEST_F(LabelSequenceTest, comparePart) {
    EXPECT_FALSE(ls1.equals(ls8));

    // strip root label from example.org.
    ls1.stripRight(1);
    // strip foo from foo.example.org.bar.
    ls8.stripLeft(1);
    // strip bar. (i.e. bar and root) too
    ls8.stripRight(2);

    EXPECT_TRUE(ls1.equals(ls8));

    // Data comparison
    size_t len;
    const char* data = ls1.getData(&len);
    getDataCheck(data, len, ls8);
}

TEST_F(LabelSequenceTest, isAbsolute) {
    ASSERT_TRUE(ls1.isAbsolute());

    ls1.stripLeft(1);
    ASSERT_TRUE(ls1.isAbsolute());
    ls1.stripRight(1);
    ASSERT_FALSE(ls1.isAbsolute());

    ASSERT_TRUE(ls2.isAbsolute());
    ls2.stripRight(1);
    ASSERT_FALSE(ls2.isAbsolute());

    ASSERT_TRUE(ls3.isAbsolute());
    ls3.stripLeft(2);
    ASSERT_TRUE(ls3.isAbsolute());
}

// The following are test data used in the getHash test below.  Normally
// we use example/documentation domain names for testing, but in this case
// we'd specifically like to use more realistic data, and are intentionally
// using real-world samples: They are the NS names of root and some top level
// domains as of this test.
const char* const root_servers[] = {
    "a.root-servers.net", "b.root-servers.net", "c.root-servers.net",
    "d.root-servers.net", "e.root-servers.net", "f.root-servers.net",
    "g.root-servers.net", "h.root-servers.net", "i.root-servers.net",
    "j.root-servers.net", "k.root-servers.net", "l.root-servers.net",
    "m.root-servers.net", NULL
};
const char* const gtld_servers[] = {
    "a.gtld-servers.net", "b.gtld-servers.net", "c.gtld-servers.net",
    "d.gtld-servers.net", "e.gtld-servers.net", "f.gtld-servers.net",
    "g.gtld-servers.net", "h.gtld-servers.net", "i.gtld-servers.net",
    "j.gtld-servers.net", "k.gtld-servers.net", "l.gtld-servers.net",
    "m.gtld-servers.net", NULL
};
const char* const jp_servers[] = {
    "a.dns.jp", "b.dns.jp", "c.dns.jp", "d.dns.jp", "e.dns.jp",
    "f.dns.jp", "g.dns.jp", NULL
};
const char* const cn_servers[] = {
    "a.dns.cn", "b.dns.cn", "c.dns.cn", "d.dns.cn", "e.dns.cn",
    "ns.cernet.net", NULL
};
const char* const ca_servers[] = {
    "k.ca-servers.ca", "e.ca-servers.ca", "a.ca-servers.ca", "z.ca-servers.ca",
    "tld.isc-sns.net", "c.ca-servers.ca", "j.ca-servers.ca", "l.ca-servers.ca",
    "sns-pb.isc.org", "f.ca-servers.ca", NULL
};

// A helper function used in the getHash test below.
void
hashDistributionCheck(const char* const* servers) {
    const size_t BUCKETS = 64;  // constant used in the MessageRenderer
    set<Name> names;
    vector<size_t> hash_counts(BUCKETS);

    // Store all test names and their super domain names (excluding the
    // "root" label) in the set, calculates their hash values, and increments
    // the counter for the corresponding hash "bucket".
    for (size_t i = 0; servers[i] != NULL; ++i) {
        const Name name(servers[i]);
        for (size_t l = 0; l < name.getLabelCount() - 1; ++l) {
            pair<set<Name>::const_iterator, bool> ret =
                names.insert(name.split(l));
            if (ret.second) {
                hash_counts[LabelSequence((*ret.first)).getHash(false) %
                            BUCKETS]++;
            }
        }
    }

    // See how many conflicts we have in the buckets.  For the testing purpose
    // we expect there's at most 2 conflicts in each set, which is an
    // arbitrary choice (it should happen to succeed with the hash function
    // and data we are using; if it's not the case, maybe with an update to
    // the hash implementation, we should revise the test).
    for (size_t i = 0; i < BUCKETS; ++i) {
        EXPECT_GE(3, hash_counts[i]);
    }
}

TEST_F(LabelSequenceTest, getHash) {
    // Trivial case.  The same sequence should have the same hash.
    EXPECT_EQ(ls1.getHash(true), ls1.getHash(true));

    // Check the case-insensitive mode behavior.
    EXPECT_EQ(ls1.getHash(false), ls5.getHash(false));

    // Check that the distribution of hash values is "not too bad" (such as
    // everything has the same hash value due to a stupid bug).  It's
    // difficult to check such things reliably.  We do some ad hoc tests here.
    hashDistributionCheck(root_servers);
    hashDistributionCheck(jp_servers);
    hashDistributionCheck(cn_servers);
    hashDistributionCheck(ca_servers);
}

}
