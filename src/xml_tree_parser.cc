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

#include <utility>
#include "com/centreon/cdash/xml/node_iterator.hh"
#include "com/centreon/cdash/xml_tree_parser.hh"
#include "com/centreon/exceptions/basic.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;

/**
 *  Constructor.
 *
 *  @param[in] tree  The tree to parse.
 */
xml_tree_parser::xml_tree_parser(xml::tree tree)
  : _tree(std::move(tree)) {}

/**
 *  Destructor.
 */
xml_tree_parser::~xml_tree_parser() noexcept {}

/**
 *  Parse the tree.
 *
 *  @param[out] sequences  A list of all the object tasks, in sequential orders.
 *  @param[out] profile    The profile to use, if found.
 *
 */
void xml_tree_parser::parse(
       std::vector<std::vector<object>>& sequences,
       std::string& profile) const {
  std::map<std::string, object> objects;
  std::vector<std::vector<std::string>> sequence_list;
  xml::node root_node = _tree.get_root_node();

  if (root_node.null())
    throw (exceptions::basic()
           << "xml_tree_parser: couldn't find the root node");

  // For each node.
  for (auto const& node : root_node) {
    // Get profile.
    if (node.get_name() == "profile") {
      profile = node.get_content();
      continue;
    }

    // Get all the objects used in the foreach attributes.
    // This is the external inheritance of this node.
    std::vector<std::vector<object>> for_each_nodes;
    for (auto const& for_each_node : node.get_children("foreach")) {
      std::vector<object> for_each_use_nodes;
      for (auto const& for_each_use_node : for_each_node.get_children("use")) {
        auto found_use_node = objects.find(for_each_use_node.get_content());
        if (found_use_node == objects.end())
          throw (exceptions::basic()
                 << "xml_tree_parser: couldn't find node '"
                 << for_each_use_node.get_content() << "'");
        for_each_use_nodes.push_back(found_use_node->second);
      }
      for_each_nodes.push_back(std::move(for_each_use_nodes));
    }

    // Get all the objects used in the sequential attribute.
    // This is the sequential inheritances of this node.
    std::vector<object> sequential_nodes;
    if (node.get_children("sequential").size() > 1)
      throw (exceptions::basic()
             << "xml_tree_parser: more than one sequential attribute for"
                " node '" << node.get_child("name").get_content() << "'");
    xml::node seq = node.get_child("sequential");
    if (!seq.null()) {
      if (node.get_name() != "task")
        throw (exceptions::basic()
               << "xml_tree_parser: sequential attribute in a non-task object"
                  " '" << node.get_child("name").get_content()) << "'";
      for (auto const& sequential_node_uses : seq.get_children("use")) {
        auto found_sequential_node
          = objects.find(sequential_node_uses.get_content());
        if (found_sequential_node == objects.end())
          throw (exceptions::basic()
                 << "xml_tree_parser: couldn't find node '"
                 << sequential_node_uses.get_content() << "'");
        sequential_nodes.push_back(found_sequential_node->second);
      }
    }

    // Create an object for all the cartesian products
    // of the external inheritances and sequential inheritances.
    // We retrieve the sequence of objects and store it into a list.
    std::vector<object> uses;
    _for_each_node(
      for_each_nodes.begin(),
      for_each_nodes.end(),
      sequential_nodes,
      node,
      uses,
      objects,
      sequence_list);
  }

  // Iterate over the list of sequences.
  // If the first object of the sequence is a task,
  // store the sequence's objects for the caller.
  for (auto const& sequence : sequence_list) {
    object const& obj = objects.at(sequence.front());
    if (obj.get_type() == "task") {
      std::vector<object> object_sequence;
      for (std::string const& object_name : sequence)
        object_sequence.emplace_back(std::move(objects.at(object_name)));
      sequences.emplace_back(std::move(object_sequence));
    }
  }
}

/**
 *  Parse an instance of a node.
 *
 *  @param[in] node         The node.
 *  @param[in] use          The external inheritance of this node.
 *  @param[in,out] objects  The object map.
 *
 *  @return                 The unique name of this node.
 */
std::string xml_tree_parser::_parse_node(
              xml::node node,
              std::vector<object> const& use,
              std::map<std::string, object> &objects) const {
  // Get type.
  std::string type = node.get_name();

  // Create object.
  object obj(type);

  // Inherit the external objects.
  for (auto const& dep : use)
    obj.inherit_macros(dep);

  // Resolve internal dependencies.
  for (auto const& dep : node.get_children("use")) {
    auto found = objects.find(dep.get_content());
    if (found != objects.end())
      obj.inherit_macros(found->second);
  }

  // Get and resolve name.
  std::string name = node.get_child("name").get_content();
  if (name.empty())
    throw (exceptions::basic()
            << "xml_tree_parser: couldn't find the name for '" << type << "'");
  name = obj.resolve_macros(std::move(name));
  obj.set_name(name);

  // Set macros.
  for (auto const& sub_node : node) {
    if (sub_node.get_name() != "name"
        && sub_node.get_name() != "use") {
      if (sub_node.get_name() != "file" && sub_node.get_name() != "returned_file")
        obj.set_macro(
              sub_node.get_name(),
              obj.resolve_macros(sub_node.get_content()));
      else {
        std::string local_filename = sub_node.get_child("in").get_content();
        std::string remote_filename = sub_node.get_child("out").get_content();
        bool resolve_macro =
            (sub_node.get_child("resolve_macro").get_content() == "true");

        if (local_filename.empty() || remote_filename.empty())
          throw (exceptions::basic()
                 << "xml_tree_parser: couldn't find local filename or "
                    "remote filename for '" << name << "'");
        if (sub_node.get_name() == "file")
          obj.add_file(
                obj.resolve_macros(local_filename),
                obj.resolve_macros(remote_filename),
                resolve_macro);
        else if (sub_node.get_name() == "returned_file")
          obj.add_returned_file(
                 obj.resolve_macros(local_filename),
                 obj.resolve_macros(remote_filename));
      }
    }
  }

  // Emplace it into the map.
  // XXX: No emplace because GCC 4.7.
  auto inserted = objects.insert(std::make_pair(name, std::move(obj)));
  if (inserted.second == false)
    throw (exceptions::basic()
           << "xml_tree_parser: object '" << name << "' already exists");

  return (std::move(name));
}

/**
 *  Used to recurse upon all the foreach and
 *  create an object for all cartesian combination.
 *
 *  @param[in] here          Used by the recursion.
 *  @param[in] end           Used by the recursion.
 *  @param[in] seq_entries   The sequential entries of this objects.
 *  @param[in] node          The xml node of the object.
 *  @param[in] use           List of external node inheritance built
 *                           by the recursion.
 *  @param[int,out] objects  Object map
 *  @param[out] sequence_list List of all sequential objects that is
 *                            constituting a sequential task.
 */
void  xml_tree_parser::_for_each_node(
        std::vector<std::vector<object>>::const_iterator here,
        std::vector<std::vector<object>>::const_iterator end,
        std::vector<object> const& seq_entries,
        xml::node node,
        std::vector<object>& use,
        std::map<std::string, object> &objects,
        std::vector<std::vector<std::string>>& sequence_list) const {
  if (here == end) {
    // Now that we have all the foreach combinations for this node,
    // we create an object for each sequential entry. If no sequential entry
    // was given, we create only one object.
    std::vector<std::string> sequence;
    if (seq_entries.size() == 0)
      sequence.emplace_back(_parse_node(node, use, objects));
    else
      for (auto const& seq_entry : seq_entries) {
       use.push_back(seq_entry);
       sequence.emplace_back(_parse_node(node, use, objects));
       use.pop_back();
      }
    sequence_list.emplace_back(std::move(sequence));
  }
  else {
    for (auto const& n : *here) {
      use.push_back(n);
      _for_each_node(
        here + 1,
        end,
        seq_entries,
        node,
        use,
        objects,
        sequence_list);
      use.pop_back();
    }
  }
}
