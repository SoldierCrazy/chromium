// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/dragdrop/os_exchange_data_provider_aurax11.h"

// Clean up X11 header polution
#undef None
#undef Bool

#include "base/message_loop/message_loop.h"
#include "base/strings/string16.h"
#include "base/strings/utf_string_conversions.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

const char kFileURL[] = "file:///home/user/file.txt";
const char kFileName[] = "/home/user/file.txt";
const char kGoogleTitle[] = "Google";
const char kGoogleURL[] = "http://www.google.com/";

namespace ui {

class OSExchangeDataProviderAuraX11Test : public testing::Test {
 public:
  OSExchangeDataProviderAuraX11Test() {}

  void AddURLList(const std::string& list_contents) {
    std::string contents_copy = list_contents;
    scoped_refptr<base::RefCountedMemory> mem(
        base::RefCountedString::TakeString(&contents_copy));

    provider.format_map_.Insert(
        provider.atom_cache_.GetAtom(ui::Clipboard::kMimeTypeURIList),
        mem);
  }

 protected:
  base::MessageLoopForUI message_loop;
  ui::OSExchangeDataProviderAuraX11 provider;
};

TEST_F(OSExchangeDataProviderAuraX11Test, MozillaURL) {
  // Check that we can get titled entries.
  provider.SetURL(GURL(kGoogleURL), base::ASCIIToUTF16(kGoogleTitle));
  {
    GURL out_gurl;
    base::string16 out_str;
    EXPECT_TRUE(provider.GetURLAndTitle(&out_gurl, &out_str));
    EXPECT_EQ(base::ASCIIToUTF16(kGoogleTitle), out_str);
    EXPECT_EQ(kGoogleURL, out_gurl.spec());
  }

  // Check that we can get non-titled entries.
  provider.SetURL(GURL(kGoogleURL), base::string16());
  {
    GURL out_gurl;
    base::string16 out_str;
    EXPECT_TRUE(provider.GetURLAndTitle(&out_gurl, &out_str));
    EXPECT_EQ(base::string16(), out_str);
    EXPECT_EQ(kGoogleURL, out_gurl.spec());
  }
}

TEST_F(OSExchangeDataProviderAuraX11Test, FilesArentURLs) {
  AddURLList(kFileURL);

  EXPECT_TRUE(provider.HasFile());
  EXPECT_FALSE(provider.HasURL());
}

TEST_F(OSExchangeDataProviderAuraX11Test, HTTPURLsArentFiles) {
  AddURLList(kGoogleURL);

  EXPECT_FALSE(provider.HasFile());
  EXPECT_TRUE(provider.HasURL());
}

TEST_F(OSExchangeDataProviderAuraX11Test, URIListWithBoth) {
  AddURLList("file:///home/user/file.txt\nhttp://www.google.com");

  EXPECT_TRUE(provider.HasFile());
  EXPECT_TRUE(provider.HasURL());

  // We should only receive the file from GetFilenames().
  std::vector<OSExchangeData::FileInfo> filenames;
  EXPECT_TRUE(provider.GetFilenames(&filenames));
  ASSERT_EQ(1u, filenames.size());
  EXPECT_EQ(kFileName, filenames[0].path.value());

  // We should only receive the URL here.
  GURL out_gurl;
  base::string16 out_str;
  EXPECT_TRUE(provider.GetURLAndTitle(&out_gurl, &out_str));
  EXPECT_EQ(base::string16(), out_str);
  EXPECT_EQ(kGoogleURL, out_gurl.spec());
}

TEST_F(OSExchangeDataProviderAuraX11Test, OnlyStringURLIsUnfiltered) {
  const base::string16 file_url = base::UTF8ToUTF16(kFileURL);
  provider.SetString(file_url);

  EXPECT_TRUE(provider.HasString());
  EXPECT_FALSE(provider.HasURL());
}

TEST_F(OSExchangeDataProviderAuraX11Test, StringAndURIListFilterString) {
  const base::string16 file_url = base::UTF8ToUTF16(kFileURL);
  provider.SetString(file_url);
  AddURLList(kFileURL);

  EXPECT_FALSE(provider.HasString());
  base::string16 out_str;
  EXPECT_FALSE(provider.GetString(&out_str));

  EXPECT_TRUE(provider.HasFile());
}

}  // namespace ui
