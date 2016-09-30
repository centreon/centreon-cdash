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

#include "com/centreon/cdash/xml/property.hh"

using namespace com::centreon::cdash::xml;

/**
 *  Default constructor.
 */
property::property() noexcept
  : _prop(nullptr) {}

/**
 *  Constructor.
 *
 *  @param[in] nd  The attribute pointer.
 */
property::property(xmlAttrPtr nd) noexcept
  : _prop(nd) {}

/**
 *  Destructor.
 */
property::~property() noexcept {}

/**
 *  Copy constructor.
 *
 *  @param[in] nd  The object to copy.
 */
property::property(property const& nd) noexcept
  : _prop(nd._prop) {}

/**
 *  Move constructor.
 *
 *  @param[in] nd  The object to move.
 */
property::property(property&& nd) noexcept {
  _prop = nd._prop;
  nd._prop = nullptr;
}

/**
 *  Assignment operator.
 *
 *  @param[in] nd  The object to copy.
 *
 *  @return        Reference to this object.
 */
property& property::operator=(property const& nd) noexcept {
  if (this != &nd)
    _prop = nd._prop;
  return (*this);
}

/**
 *  Move assignment operator.
 *
 *  @param[in] nd  The object to move.
 *
 *  @return        Reference to this object.
 */
property& property::operator=(property&& nd) noexcept {
  if (this != &nd) {
    _prop = nd._prop;
    nd._prop = nullptr;
  }
  return (*this);
}

/**
 *  Comparison operator.
 *
 *  @param[in] nd  The object to compare with.
 *
 *  @return        True if both objects are equal.
 */
bool property::operator==(property const& nd) const noexcept {
  return (_prop == nd._prop);
}

/**
 *  Is this property null?
 *
 *  @return  True if this property is null.
 */
bool property::null() const noexcept {
  return (!_prop);
}

/**
 *  Get the name of this property.
 *
 *  @return  The name of this property.
 */
std::string property::get_name() const {
  return (_prop ? std::string((const char*)(_prop->name)) : std::string());
}

/**
 *  Get the content of this property.
 *
 *  @return  The content of this property.
 */
std::string property::get_content() const {
  return (_prop ?
            std::string((const char*)(_prop->children->content))
            : std::string());
}
