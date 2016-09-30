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

#ifndef CCC_ARGS_PARSER_HH
#  define CCC_ARGS_PARSER_HH

#  include "com/centreon/misc/get_options.hh"

#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

class args_parser : public misc::get_options {
  public:
                    args_parser();
                    ~args_parser() noexcept;

    void            parse(int argc, char **argv);
    virtual std::string
                    help() const;

  private:
};

CCC_END()

#endif // !CCC_ARGS_PARSER_HH
