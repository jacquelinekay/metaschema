#ifndef WALLABY__HH_
#include <string>

namespace wallaby {

  template<typename InstanceT>
  class Parser {
  public:
    explicit Parser(const std::stringstream & specification_sstream);

    bool parse(const std::string & instance_file, InstanceT & output);

    bool parseFromStringStream(const std::stringstream & instance_string, InstanceT & output);
  };

  // How can we specify the instance before the specification is read?
  // answer: metaprogramming
  template<typename InstanceT>
  Parser<InstanceT> loadSpec(const std::string & specification_file);

  // In the form of a read file
  template<typename InstanceT>
  Parser<InstanceT> loadSpecFromStringStream(const std::stringstream & specification_string);
}

#endif  // WALLABY__HH_

// templated code is included here
// TODO: Employ a trick to make this header is private
#include "wallaby/wallaby_impl.hh"
