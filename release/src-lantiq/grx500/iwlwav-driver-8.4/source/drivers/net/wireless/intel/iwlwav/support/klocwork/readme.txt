How to check sources using Klocwork
===================================

1) Cleanup everything
hg clean

2) Setup project
make menuconfig
# Select target, select requred options, etc.

3) Create KW project
kwcheck create

4) Import additional databases
kwcheck import support/klocwork/kw_database.kb
kwcheck import support/klocwork/kw_override.h

5) build project
kwshell
make
exit

6) Run analysis
kwcheck run

5) Generate output results
kwcheck list -F detailed --report kwreport_start.log
