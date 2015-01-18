#
# requires ruby-2.0.0 or higher to run this script.
#

require "fiddle/import"

module DLL
  extend Fiddle::Importer
  dlload "mmc.dll"
  extern "int mmc_convert(char *, char *, char *)"
  extern "int mmc_convert_string(char *, char *, char *)"
  extern "void mmc_version(char*)"
end

if ARGV[0] == nil then
  print <<EOD
Usage: ruby mmc_test.rb hoge.mid
EOD
  exit
end

outfile = ARGV[0]
mml = "abc"


buf0 = "\0" * 256
buf1 = "\0" * 256
DLL.mmc_version(buf1)
print "converting #{outfile} by using #{buf1}\n."
result = DLL.mmc_convert_string(mml, outfile, buf0)

print buf0