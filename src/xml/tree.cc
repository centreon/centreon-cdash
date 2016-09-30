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

#include "com/centreon/cdash/xml/tree.hh"

using namespace com::centreon::cdash::xml;

/**
 *  Constructor.
 */
tree::tree(xmlDocPtr doc) noexcept
  : _tree(doc) {
}

/**
 *  Destructor.
 */
tree::~tree() noexcept {
  if (_tree)
    xmlFreeDoc(_tree);
}

/**
 *  Move constructor.
 *
 *  @param[in] tr  Tree to move.
 */
tree::tree(tree&& tr) {
  _tree = tr._tree;
  tr._tree = nullptr;
}
/**
 *  Move Assignment operator.
 *
 *  @param[in] tr  Tree to move.
 *
 *  @return        Reference to this.
 */
tree& tree::operator=(tree&& tr) {
  if (this != &tr) {
    _tree = tr._tree;
    tr._tree = nullptr;
  }
  return (*this);
}

/**
 *  Is this a null tree?
 *
 *  @return  True if this is a null tree.
 */
bool tree::null() const noexcept {
  return (!_tree);
}

/**
 *  Get the root node.
 *
 *  @return       A root node.
 */
node tree::get_root_node() const noexcept {
  return (_tree ? node(xmlDocGetRootElement(_tree)) : node());
}
