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

#include "com/centreon/cdash/xml/node_iterator.hh"

using namespace com::centreon::cdash::xml;

/**
 *  Constructor.
 *
 *  @param[in] nd  The node.
 */
node_iterator::node_iterator(node const& nd) noexcept
  : _curr(nd._node ? nd._node->children : nullptr) {
  _skip_non_element_nodes();
}

/**
 *  Default constructor.
 */
node_iterator::node_iterator() noexcept
  : _curr(nullptr) {}

/**
 *  Copy constructor.
 *
 *  @param[in] it  Object to copy.
 */
node_iterator::node_iterator(node_iterator const& it) noexcept {
  _curr = it._curr;
}

/**
 *  Assignment operator.
 *
 *  @param[in] it  Object to copy.
 *
 *  @return        Reference to this.
 */
node_iterator& node_iterator::operator=(node_iterator const& it) noexcept {
  if (this != &it)
    _curr = it._curr;
  return (*this);
}

/**
 *  Comparison operator.
 *
 *  @param[in] it  Object to compare.
 *
 *  @return        True if both objects are equal.
 */
bool node_iterator::operator==(node_iterator const& it) noexcept {
  return (_curr == it._curr);
}

/**
 *  Unequality comparison operator.
 *
 *  @param[in] it  Object to compare.
 *
 *  @return        True if both objects are unequal.
 */
bool node_iterator::operator!=(node_iterator const& it) noexcept {
  return (!operator==(it));
}

/**
 *  * operator.
 *
 *  @return  Reference to the node.
 */
node const&  node_iterator::operator*() const noexcept {
  return (_curr);
}

/**
 *  * operator.
 *
 *  @return  Reference to the node.
 */
node& node_iterator::operator*() noexcept {
  return (_curr);
}

/**
 *  ++Operator.
 */
void node_iterator::operator++() noexcept {
  _curr = node(_curr._node->next);
  _skip_non_element_nodes();
}

/**
 *  Skip all the non elements nodes.
 */
void node_iterator::_skip_non_element_nodes() noexcept {
  while (!_curr.null() && _curr.get_type() != node::element)
    _curr = node(_curr._node->next);
}
