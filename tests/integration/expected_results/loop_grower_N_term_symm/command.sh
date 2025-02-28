
#
# This is a command file.
#
# To make a new test, all you have to do is:
#   1.  Make a new directory under tests/
#   2.  Put a file like this (named "command") into that directory.
#
# The contents of this file will be passed to the shell (Bash or SSH),
# so any legal shell commands can go in this file.
# Or comments like this one, for that matter.
#
# Variable substiution is done using Python's printf format,
# meaning you need a percent sign, the variable name in parentheses,
# and the letter 's' (for 'string').
#
# Available variables include:
#   workdir     the directory where test input files have been copied,
#               and where test output files should end up.
#   minidir     the base directory where Mini lives
#   database    where the Mini database lives
#   bin         where the Mini binaries live
#   binext      the extension on binary files, like ".linuxgccrelease"
#   python      the full path to the Python interpretter
#
# The most important thing is that the test execute in the right directory.
# This is especially true when we're using SSH to execute on other hosts.
# All command files should start with this line:
#
cd /local/Rosetta/master/tests/integration/new/loop_grower_N_term_symm
set -e

# Do the tests actually exist?
[ -x /local/Rosetta/master/source/bin/rosetta_scripts.default.linuxgccrelease ] || exit 1

date > start-time.ignore

/local/Rosetta/master/source/bin/rosetta_scripts.default.linuxgccrelease  -database /local/Rosetta/master/database \
    -testing:INTEGRATION_TEST @flags \
    &>/dev/null

date > finish-time.ignore

test "${PIPESTATUS[0]}" != '0' && exit 1 || true