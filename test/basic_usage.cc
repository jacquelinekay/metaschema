#include <wallaby/wallaby.hh>

int main() {
  using InstanceT = void *;
  {
    wallaby::Parser<InstanceT> test_parser = wallaby::loadSpec<InstanceT>("test/schema_test.wl");

    InstanceT instance;
    bool success = test_parser.parse("test/instance_test.xml", instance);
    // assert that the API works
  }

  {
    wallaby::Parser<InstanceT> test_parser = wallaby::loadSpec<InstanceT>("test/plugin_test.wl");

    // Typenames though
    InstanceT instance;
    test_parser.parse("test/instance_test.xml", instance);
    // assert that the API works
  }

}
