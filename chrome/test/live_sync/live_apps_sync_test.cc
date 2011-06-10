// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/test/live_sync/live_apps_sync_test.h"

#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/extensions/extension.h"

namespace {

std::string CreateFakeAppName(int index) {
  return "fakeapp" + base::IntToString(index);
}

}  // namespace

LiveAppsSyncTest::LiveAppsSyncTest(TestType test_type)
    : LiveSyncTest(test_type) {}

LiveAppsSyncTest::~LiveAppsSyncTest() {}

bool LiveAppsSyncTest::SetupClients() {
  if (!LiveSyncTest::SetupClients())
    return false;

  extension_helper_.Setup(this);
  return true;
}

bool LiveAppsSyncTest::HasSameAppsAsVerifier(int index) {
  // TODO(akalin): We may want to filter out non-apps for some tests.
  return extension_helper_.ExtensionStatesMatch(GetProfile(index), verifier());
}

bool LiveAppsSyncTest::AllProfilesHaveSameAppsAsVerifier() {
  for (int i = 0; i < num_clients(); ++i) {
    if (!HasSameAppsAsVerifier(i)) {
      LOG(ERROR) << "Profile " << i << " doesn't have the same apps as the"
                                       " verifier profile.";
      return false;
    }
  }
  return true;
}

void LiveAppsSyncTest::InstallApp(Profile* profile, int index) {
  return extension_helper_.InstallExtension(profile,
                                            CreateFakeAppName(index),
                                            Extension::TYPE_HOSTED_APP);
}

void LiveAppsSyncTest::UninstallApp(Profile* profile, int index) {
  return extension_helper_.UninstallExtension(profile,
                                              CreateFakeAppName(index));
}

void LiveAppsSyncTest::EnableApp(Profile* profile, int index) {
  return extension_helper_.EnableExtension(profile,
                                           CreateFakeAppName(index));
}

void LiveAppsSyncTest::DisableApp(Profile* profile, int index) {
  return extension_helper_.DisableExtension(profile,
                                            CreateFakeAppName(index));
}

void LiveAppsSyncTest::IncognitoEnableApp(Profile* profile, int index) {
  return extension_helper_.IncognitoEnableExtension(profile,
                                                    CreateFakeAppName(index));
}

void LiveAppsSyncTest::IncognitoDisableApp(Profile* profile, int index) {
  return extension_helper_.IncognitoDisableExtension(profile,
                                                     CreateFakeAppName(index));
}

void LiveAppsSyncTest::InstallAppsPendingForSync(
    Profile* profile) {
  extension_helper_.InstallExtensionsPendingForSync(
      profile, Extension::TYPE_HOSTED_APP);
}
