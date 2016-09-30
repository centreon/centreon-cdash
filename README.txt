==============
Centreon CDash
==============

This tool was designed to build C++ software at Centreon. It uses
Amazon Web Services to perform this task. Build tasks are described in
a XML configuration file (which format is explained below).


Usage
-----

$> centreon_cdash <xml_cfg_file>

The XML configuration file format is explained below.

Environment Installation
------------------------

Centreon Cdash expects the aws cli environment to be properly installed
and configured. This means configuring a profile for aws cli using:

$> aws configure

And supplying the proper access keys. Refer to amazon aws cli documentation
for more information.

Additionally, all indentity keys used by ssh to connect on the servers
needs to be added.

Configuration file
------------------

The goal of the configuration file is to define tasks that contains one
or multiple commands that should be run on a EC2 instance to perform a
build. Aside from the task objects, the configuration file uses a macro
inheritance system to easily configure tasks to be run. Objects are
one-level deep XML nodes that have one known property : /name/. This
identifier is used to uniquely identify an object. If two name collides
an error must be raised. Also the opening tag of an object name its
type. All other properties are macros that can be used to generate
other macros.

Objects
-------

Let's start with a simple compiler definition.

<?xml version="1.0" encoding="utf-8"?>
<cdash>
  <compiler>
    <name>gcc</name>
    <CC>gcc</CC>
    <CFLAGS>-Wall -Wextra</CFLAGS>
    <CXX>g++</CXX>
    <CXXFLAGS>-Wall -Wextra</CXXFLAGS>
  </compiler>
</cdash>

This object is named /gcc/, has type /compiler/ and contains macros /CC/
(value is "gcc"), /CXX/ (value is "g++"), CFLAGS and CXXFLAGS (which
values are "-Wall -Wextra").

Inheritance
-----------

Objects can inherit macros from other objects by using the /use/
directive. If two inherited variables hold the same name, the first one
is kept.

<?xml version="1.0" encoding="utf-8"?>
<cdash>
  <!-- MySQL server properties. -->
  <database>
    <name>mysql</name>
    <db_type>mysql</db_type>
    <db_host>localhost</db_host>
    <db_port>3306</db_port>
    <db_user>root</db_user>
    <db_password>merethis</db_password>
  </database>

  <!-- PostgreSQL server properties. -->
  <database>
    <name>postgresql</name>
    <db_type>postgresql</db_type>
    <db_host>localhost</db_host>
    <db_port>5432</db_port>
    <db_user>root</db_user>
    <db_password>merethis</db_password>
  </database>

  <!-- Simple Centreon Broker project, which can use any database. -->
  <project>
    <name>centreon-broker</name>
    <git>http://git.centreon.com/centreon-broker.git</git>
  </project>

  <!-- Each project has database properties and
    -- Centreon Broker properties.
    -->
  <project>
    <name>centreon-broker-mysql</name>
    <use>centreon-broker</use>
    <use>mysql</use>
  </project>
  <project>
    <name>centreon-broker-postgresql</name>
    <use>centreon-broker</use>
    <use>postgresql</use>
  </project>
</cdash>

Macros
------

Each property defined in an object is available as a macro. For example
the previous /mysql/ database definition has macros $db_type$,
$db_host$, ... which have respective values ("mysql", "localhost", ...).
Macros can be used in macro values but not on macro name. Macro
evaluation is recursive. It is perfectly valid to use macros in the name
property of an object. Finally the special macro /name/ is made
available as $name$ but also as $type.name$. This is useful to use
parent names when inheriting from multiple different types.

Foreach
-------

The /foreach/ keyword is used to instanciate multiple objects. Each
instanciated object will use one object of the foreach clause. It is
perfectly valid to use multiple /foreach/ clause in an object. In such
a case, objects are instanciated using the cartesian product of the two
/foreach/ clauses.

In the following example, we want to build each project on each target
machine and therefore instanciate 4 different tasks.

<?xml version="1.0" encoding="utf-8"?>
<cdash>
  <machine>
    <name>ubuntu</name>
    <ami>XXXXXXXXX</ami>
  </machine>
  <machine>
    <name>freebsd</name>
    <ami>XXXXXXXXX</ami>
  </machine>

  <project>
    <name>centreon-broker</name>
    <command>build_broker.sh</command>
  </project>
  <project>
    <name>centreon-engine</name>
    <command>build_engine.sh</command>
  </project>

  <task>
    <name>$machine.name$-$project.name$</name>
    <foreach>
      <use>ubuntu</use>
      <use>freebsd</use>
    </foreach>
    <foreach>
      <use>centreon-broker</use>
      <use>centreon-engine</use>
    </foreach>
  </task>
</cdash>

Sequential
----------

The /sequential/ keyword is used to instanciate multiple task that will
be executed in sequence on the same machine. There can be only one
sequential clause per task.

In the following example, we want to build each project sequentially on
the same machine. Look at how /foreach/ and /sequential/ clauses can be
used together. /foreach/ clauses are always resolved before /sequential/
ones.

<?xml version="1.0" encoding="utf-8"?>
<cdash>
  <machine>
    <name>ubuntu</name>
    <ami>XXXXXXXXX</ami>
  </machine>
  <machine>
    <name>freebsd</name>
    <ami>XXXXXXXXX</ami>
  </machine>

  <project>
    <name>centreon-broker</name>
    <command>build_broker.sh</command>
  </project>
  <project>
    <name>centreon-engine</name>
    <command>build_engine.sh</command>
  </project>

  <task>
    <name>$machine.name$-$project.name$</name>
    <foreach>
      <use>ubuntu</use>
      <use>freebsd</use>
    </foreach>
    <sequential>
      <use>centreon-broker</use>
      <use>centreon-engine</use>
    </sequential>
  </task>
</cdash>

Tasks
-----

Tasks are special objects. There are the sole objects that are directly
handled by Centreon CDash after their instanciation. Such tasks must be
some special properties that are used to perform the build. These
properties are as follow :

================= =====================================================
Property          Description
================= =====================================================
ami               Amazon Machine Image that should be used to run the
                  EC2 instance when fetched.
command           The local command to be executed remotely.
file              A file to copy before executing the command. Files
                  are configured thusly:
                  <file>
                    <in>local_path</in>
                    <out>remote_path</out>
                    <resolve_macro>true</resolve_macro>
                  </file>
                  All macros contained in the file are resolved if
                  resolve_macro is set at 'true'. Everything else (or
                  nothing) will not resolve the macro of this file.
returned_file     Like the 'file' property, but will be copied back
                  from the remote server after the command has been
                  executed.
type              The type of the amazon instance that should be used
                  i.e r3.large, m3.medium, etc.
key               The name of the key used to connect to this machine.
security_group    The name of the security group the instance should
                  belong to.
security_group_id The id of the security group the instance should
                  belong to.
should_delete     Should the instances be deleted at the end of the
                  run? 'true' or 'false'. Optional. Default to 'true'.
key_file          The key identity file (*.pem) used to connect to the
                  remote machine. Optional.
ssh_timeout       The timeout used by ssh to connect to this machine.
                  Default to 5 minutes. In seconds.
ssh_user          The user used by ssh to connect to this machine.
                  Default to 'centreon'.
ssh_port          The port used by ssh to connect to this machine.
                  Default to 22.
subnet_id         The id of the subnet to use. (VPC)
================= =====================================================
