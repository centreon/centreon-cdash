/*
** Copyright 2013,2015 Merethis
**
** All rights reserved.
*/

#ifndef CCC_XML_LIBRARY_HH
#  define CCC_XML_LIBRARY_HH

#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

namespace xml {

class           library {
public:
                library();
                ~library();
private:
                library(library const&) = delete;
  library&      operator=(library const&) = delete;
};

} //namespace xml

CCC_END()

#endif // !CCC_XML_LIBRARY_HH
