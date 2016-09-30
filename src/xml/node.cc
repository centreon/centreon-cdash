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

#include "com/centreon/cdash/xml/node.hh"
#include "com/centreon/cdash/xml/node_iterator.hh"

using namespace com::centreon::cdash::xml;

/**
 *  Default constructor.
 */
node::node() noexcept
  : _node(nullptr) {
}

/**
 *  Constructor.
 */
node::node(xmlNodePtr nd) noexcept
  : _node(nd) {
}

/**
 *  Destructor.
 */
node::~node() noexcept {
}

/**
 *  Copy constructor.
 *
 *  @param[in] nd  The node to copy.
 */

node::node(node const& nd) noexcept {
  _node = nd._node;
}
/**
 *  Move constructor.
 *
 *  @param[in] nd  The node to move.
 */
node::node(node&& nd) noexcept {
  _node = nd._node;
  nd._node = nullptr;
}
/**
 *  Assignment operator.
 *
 *  @param[in] nd  The node to copy.
 *
 *  @return        Reference to this.
 */
node& node::operator=(node const& nd) noexcept {
  if (this != &nd)
    _node = nd._node;
  return (*this);
}
/**
 *  Assignment move operator.
 *
 *  @param[in] nd  The node to move.
 *
 *  @return        Reference to this.
 */
node& node::operator=(node&& nd) noexcept {
  if (this != &nd) {
    _node = nd._node;
    nd._node = nullptr;
  }
  return (*this);
}

/**
 *  Comparison operator.
 *
 *  @param[in] nd  The node to compare with.
 *
 *  @return  True if both node equal.
 */
bool node::operator==(node const& nd) const noexcept {
  return (_node == nd._node);
}

/**
 *  Unequality comparison operator.
 *
 *  @param[in] nd  The node to compare with.
 *
 *  @return        True if unequal.
 */
bool node::operator!=(node const& nd) const noexcept {
  return (!operator==(nd));
}

/**
 *  Is this a null node?
 *
 *  @return  True if this is a null node.
 */
bool node::null() const noexcept {
  return (!_node);
}

/**
 *  Create an iterator.
 *
 *  @return  An iterator.
 */
node::iterator node::begin() const {
  return (node::iterator(*this));
}

/**
 *  Create an end of line iterator.
 *
 *  @return  The iterator.
 */
node::iterator node::end() const {
  return (node::iterator());
}

/**
 *  Get the type of this node.
 *
 *  @return  The type of this node.
 */
node::type node::get_type() const noexcept {
  return (_node && _node->type == XML_ELEMENT_NODE ?
            element : other);
}

/**
 *  Get the name of this node.
 *
 *  @return  The name of this node.
 */
std::string node::get_name() const {
  return (_node && _node->name ?
            std::string((const char*)(_node->name)) : std::string());
}

/**
 *  Get the content of this node.
 *
 *  @return  The content of this node.
 */
std::string node::get_content() const {
  std::string ret;
  if (_node) {
    char* content = (char*)xmlNodeGetContent(_node);
    ret = content;
    ::free(content);
  }
  return (std::move(ret));
}

/**
 *  Find a subnode.
 *
 *  @param[in] name  Name of the subnode.
 *
 *  @return          The subnode, or a null node.
 */
node node::get_child(std::string const& name) const noexcept {
  for (auto const& node : *this) {
    if (node.get_name() == name)
      return (node);
  }
  return (node());
}

/**
 *  Find all the subnodes matching a name.
 *
 *  @param[in] name  Name of the subnodes.
 *
 *  @return          All the subnodes.
 */
std::vector<node> node::get_children(std::string const& name) const noexcept {
  std::vector<node> ret;
  for (auto const& node : *this) {
    if (node.get_name() == name)
      ret.push_back(node);
  }
  return (std::move(ret));
}

/**
 *  Get a property.
 *
 *  @param[in] name  The name of the property.
 *
 *  @return          The property, or a null property.
 */
property node::get_property(std::string const& name) const {
  return (_node ?
            property(xmlHasProp(_node, (const unsigned char*)name.c_str()))
            : property());
}
