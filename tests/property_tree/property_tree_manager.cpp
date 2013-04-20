/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>

#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/property_tree/factory.h>
#include <opccore/managers/property_tree/id.h>
#include <opccore/managers/property_tree/manager.h>


class PropertyTreeManagerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(PropertyTreeManagerTestCase);
  CPPUNIT_TEST(Test);
  CPPUNIT_TEST_SUITE_END();

protected:
  void Test();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PropertyTreeManagerTestCase);


void PropertyTreeManagerTestCase::Test()
{
  Common::AddonsManager::UniquePtr addons = Common::CreateAddonsManager();
  Common::AddonConfiguration config;
  config.ID = PropertyTree::ManagerID;
  config.Factory = PropertyTree::CreateAddonFactory();

  addons->Register(config);
  CPPUNIT_ASSERT_NO_THROW(addons->Start());
  PropertyTree::Manager::SharedPtr propertyTreeManager;
  CPPUNIT_ASSERT_NO_THROW(propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(*addons, PropertyTree::ManagerID));
  CPPUNIT_ASSERT(propertyTreeManager);
  CPPUNIT_ASSERT(propertyTreeManager->GetPropertyTree());
}

