/*
** Copyright 2015-2016 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef CCC_XML_TREE_PARSER_HH
#  define CCC_XML_TREE_PARSER_HH

#  include <map>
#  include <vector>
#  include <string>
#  include "com/centreon/cdash/xml/tree.hh"
#  include "com/centreon/cdash/object.hh"
#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

class                   xml_tree_parser {
  public:
                        xml_tree_parser(xml::tree tree);
                        ~xml_tree_parser() noexcept;

  void                  parse(
                          std::vector<std::vector<object>>& sequences,
                          std::string& profile) const;

  private:
    xml::tree           _tree;

    std::string         _parse_node(
                          xml::node node,
                          std::vector<object> const& use,
                          std::map<std::string, object> &objects) const;
    void                _for_each_node(
                          std::vector<std::vector<object>>::const_iterator here,
                          std::vector<std::vector<object>>::const_iterator end,
                          std::vector<object> const& sequential_entries,
                          xml::node node,
                          std::vector<object>& use,
                          std::map<std::string, object> &objects,
                          std::vector<std::vector<std::string>>& sequence_list) const;

                        xml_tree_parser(xml_tree_parser const&) = delete;
    xml_tree_parser&    operator=(xml_tree_parser const&) = delete;
};

CCC_END()

#endif // !CCC_XML_TREE_PARSER_HH
