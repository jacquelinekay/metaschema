#ifndef WALLABY_IMPL__HH_

#include <fstream>
#include <sstream>

#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace wallaby {

  template<typename InstanceT>
  Parser<InstanceT>::Parser(const std::stringstream & specification_sstream) {
    // Initialize grammar
    // TODO Move stream parsing into a separate function so construction can be a constexpr
  }

  template<typename InstanceT>
  bool Parser<InstanceT>::parse(const std::string & instance_file, InstanceT & output) {
    boost::filesystem::path instance_path(instance_file);
    if (!boost::filesystem::exists(instance_path)) {
      // TODO Error messages
      return false;
    }
    std::ifstream file(instance_file);
    std::stringstream ss;
    ss << file.rdbuf();
    return parseFromStringStream(ss, output);
  }

  template<typename InstanceT>
  bool Parser<InstanceT>::parseFromStringStream(
      const std::stringstream & instance_string, InstanceT & output)
  {
    // Spirit magic!
    return true;
  }

  // How can we specify the instance before the specification is read?
  // answer: metaprogramming
  template<typename InstanceT>
  Parser<InstanceT> loadSpec(const std::string & specification_file) {
    if (!boost::filesystem::exists(boost::filesystem::path(specification_file))) {
      // TODO Error messages
      //return false;
      throw std::runtime_error("Couldn't create specification representation");
    }
    std::ifstream file(specification_file);
    std::stringstream ss;
    ss << file.rdbuf();
    return loadSpecFromStringStream<InstanceT>(ss);
  }

  // In the form of a read file
  // todo: universal reference or something
  template<typename InstanceT>
  Parser<InstanceT> loadSpecFromStringStream(const std::stringstream & specification_string) {
    return Parser<InstanceT>(specification_string);
  }

}

#endif // WALLABY_IMPL__HH_
