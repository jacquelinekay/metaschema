#ifndef WALLABY__TYPES__HH_

#include <vector>

#include "boost/variant.hpp"

namespace wallaby {
namespace types {
  enum class Requirement : char {
    ONE = '1',
    ONE_OR_MORE = '+',
    ZERO_OR_ONE = '?',
    ZERO_OR_MORE = '*',
  };

  enum class ValueType {
    INT,    // int64_t or int32_t
    DOUBLE, // double
    STRING, // std::string
    ELEMENT // std::vector<Element> representing children
  };

  using VariantT = boost::variant<int, double, std::string>;

  struct Attribute {
    ValueType type;
    VariantT value;
  };

  struct Element {
    std::string name;
    Requirement requirement;
    std::vector<Attribute *> attributes;
    std::vector<Element *> children;
    Element * parent;
  };

  struct Schema {
    std::string name;
    // A schema can contain an arbitrary number of elements
    std::vector<Element> children;
  };

}  // namespace types
}  // namespace wallaby

#endif  // WALLABY__TYPES__HH_

