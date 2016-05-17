#ifndef WALLABY__HH_
#include <string>

namespace wallaby {

  class Parser {
  public:
    Parser();

    void initializeParser(std::ifstream & specification_stream);

    template<typename InstanceT>
    bool parse(const std::string & instance_file, InstanceT & output);

    template<typename InstanceT>
    bool parseFromStream(std::ifstream & instance_string, InstanceT & output);
  };

  // How can we specify the instance before the specification is read?
  // answer: metaprogramming
  Parser loadSpec(const std::string & specification_file);

  // In the form of a read file
  Parser loadSpecFromStream(std::ifstream & specification_string);
}

#endif  // WALLABY__HH_

// templated code is included here
// TODO: Employ a trick to make this header is private
#include "wallaby/wallaby_impl.hh"
