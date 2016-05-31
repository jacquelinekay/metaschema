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
#include <boost/spirit/include/support_multi_pass.hpp>

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
  (std::vector<wallaby::types::Attribute>, attributes)
  (std::vector<boost::recursive_wrapper<wallaby::types::Element>>, children)
  (boost::recursive_wrapper<wallaby::types::Element>, parent)
)

namespace spirit = boost::spirit;
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
    qi::symbols<char, types::ValueType> type_;
    qi::symbols<char, types::Requirement> requirement_;

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
          ("int", types::ValueType::INT)
          ("double", types::ValueType::DOUBLE)
          ("string", types::ValueType::STRING);

      requirement_.add
          ("1", types::Requirement::ONE)
          ("+", types::Requirement::ONE_OR_MORE)
          ("?", types::Requirement::ZERO_OR_ONE)
          ("*", types::Requirement::ZERO_OR_MORE);

      attribute = lit("{")
        // value_type must evaluate to 
        >> "type: {" >> type_ [at_c<0>(_val) = qi::_1] >> "}"
        >> "value: {" >> int_ | double_ | quoted_string [at_c<1>(_val) = qi::_1] >> "}"
        >> "}";

      element = lit("{")
        >> "name: {" >> quoted_string [at_c<0>(_val) = qi::_1] >> "},"
        >> "requirement: {" >> requirement_ [at_c<1>(_val) = qi::_1] >> "},"
        >> "attributes: " >> *attribute [phoenix::push_back(at_c<2>(_val), qi::_1)]
        >> "children: " >>  *element [phoenix::push_back(at_c<3>(_val), qi::_1)]
        >> -("parent: " >> element [at_c<4>(_val) = qi::_1])
        >> "}";

      start =
        lit("schema")
        >> '{'
        >> "name: {" >> quoted_string [at_c<0>(_val) = qi::_1] >> "},"
        >> "children:"
        >>  '{' >> *element [phoenix::push_back(at_c<1>(_val), qi::_1)] >> '}'
        >>  '}';

    }
  };

  Parser::Parser() {
  }

  void Parser::initializeParser(std::ifstream & specification_stream) {

    using BaseIteratorT = std::istreambuf_iterator<char>;
    // boost::spirit::istream_iterator begin(specification_stream);

    spirit::multi_pass<BaseIteratorT> begin = spirit::make_default_multi_pass(
      BaseIteratorT(specification_stream));
    // Initialize grammar
    SchemaParser<decltype(begin)> parser;
    wallaby::types::Schema schema;
    // qi::parse(begin, end, parser, result);
    bool result = qi::phrase_parse(begin,
      spirit::make_default_multi_pass(BaseIteratorT()),
      parser,
      ascii::space,  //skipper
      schema);
  }

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
