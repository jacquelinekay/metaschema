#ifndef WALLABY_IMPL__HH_

#include <fstream>
#include <sstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/phoenix/fusion/at.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include <wallaby/types.hh>

BOOST_FUSION_ADAPT_STRUCT(
  wallaby::types::Schema,
  (std::string, name)
  (std::vector<wallaby::types::Element>, children)
)

BOOST_FUSION_ADAPT_STRUCT(
  wallaby::types::Attribute,
  (wallaby::types::ValueType, type)
  (wallaby::types::VariantT, value)
)

BOOST_FUSION_ADAPT_STRUCT(
  wallaby::types::Element,
  (std::string, name)
  (wallaby::types::Requirement, requirement)
  (std::vector<wallaby::types::Attribute *>, attributes)
  (std::vector<wallaby::types::Element *>, children)
  (wallaby::types::Element *, parent)
)

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

namespace wallaby {

  template <typename Iterator>
    struct SchemaParser : qi::grammar<Iterator, types::Schema(), ascii::space_type>
  {

    qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
    qi::rule<Iterator, types::Schema(), ascii::space_type> start;
    qi::rule<Iterator, types::Attribute(), ascii::space_type> attribute;
    qi::rule<Iterator, types::Element(), ascii::space_type> element;
    qi::symbols<std::string, types::ValueType> type_;

    SchemaParser() : SchemaParser::base_type(start) {
      using qi::int_;
      using qi::lit;
      using qi::double_;
      using qi::_val;
      using qi::lexeme;
      using ascii::char_;

      using phoenix::at_c;

      quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];

      /* Valid keys are : int, double, string*/
      type_.add
          (std::string("int"), types::ValueType::INT)
          (std::string("double"), types::ValueType::DOUBLE)
          (std::string("string"), types::ValueType::STRING);

      attribute = lit("{")
        // value_type must evaluate to 
        >> "type: {" >> type_ >> "}"
        >> "value: {" >> int_ | double_ | quoted_string >> "}"
        >> "}";

      element = lit("{")
        >> "name: {" >> quoted_string >> "},"
        >> "requirement: {" >> char_ >> "},"
        >> "attributes: " >> *attribute [phoenix::push_back(at_c<2>(_val), &boost::spirit::qi::_1)]
        >> "children: " >>  *element [phoenix::push_back(at_c<3>(_val), &boost::spirit::qi::_1)]
        >> -("parent: " >> element)
        >> "}";

      start =
        lit("schema")
        >> '{'
        >> "name: {" >> quoted_string >> "},"
        >> "children:"
        >>  '{' >> *element [phoenix::push_back(at_c<1>(_val), boost::spirit::qi::_1)] >> '}'
        >>  '}';

    }
  };

  Parser::Parser() {
  }

  void Parser::initializeParser(std::ifstream & specification_stream) {
    boost::spirit::istream_iterator begin(specification_stream);
    boost::spirit::istream_iterator end;
    // Initialize grammar
    SchemaParser<decltype(begin)> parser;
    wallaby::types::Schema result;
    qi::parse(begin, end, parser, result);
  }

  // TODO: ## need to use metaprogramming to construct InstanceT's after parsing
  template<typename InstanceT>
  bool Parser::parse(const std::string & instance_file, InstanceT & output) {
    boost::filesystem::path instance_path(instance_file);
    if (!boost::filesystem::exists(instance_path)) {
      // TODO Error messages
      return false;
    }
    std::ifstream file(instance_file);
    return parseFromStream<InstanceT>(file, output);
  }

  template<typename InstanceT>
  bool Parser::parseFromStream(
      std::ifstream & instance_string, InstanceT & output)
  {
    // Spirit magic!
    return true;
  }

  // How can we specify the instance before the specification is read?
  // answer: metaprogramming
  Parser loadSpec(const std::string & specification_file) {
    if (!boost::filesystem::exists(boost::filesystem::path(specification_file))) {
      // TODO Error messages
      //return false;
      throw std::runtime_error("Couldn't create specification representation");
    }
    std::ifstream file(specification_file);
    return loadSpecFromStream(file);
  }

  // In the form of a read file
  Parser loadSpecFromStream(std::ifstream & specification_string) {
    Parser parser;
    parser.initializeParser(specification_string);
    return parser;
  }

}

#endif // WALLABY_IMPL__HH_
