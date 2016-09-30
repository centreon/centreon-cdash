/*
** Copyright 2013,2015 Merethis
**
** All rights reserved.
*/

#ifndef CCC_XML_NODE_ITERATOR_HH
#  define CCC_XML_NODE_ITERATOR_HH

#  include <libxml/parser.h>
#  include <libxml/tree.h>
#  include "com/centreon/cdash/namespace.hh"
#  include "com/centreon/cdash/xml/node.hh"

CCC_BEGIN()

namespace xml {

  class         node_iterator {
  public:
                node_iterator(node const& nd) noexcept;
                node_iterator() noexcept;
                node_iterator(node_iterator const& it) noexcept;
      node_iterator& operator=(node_iterator const& it) noexcept;
      bool      operator==(node_iterator const& it) noexcept;
      bool      operator!=(node_iterator const& it) noexcept;

      node const&
                operator*() const noexcept;
      node&     operator*() noexcept;
      void      operator++() noexcept;
    private:
      node      _curr;

      void      _skip_non_element_nodes() noexcept;
  };
} //namespace xml

CCC_END()

#endif // !CCC_XML_NODE_ITERATOR_HH
