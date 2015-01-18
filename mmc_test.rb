#
# requires ruby-2.0.0 or higher to run this script.
#

require "fiddle/import"

module DLL
  extend Fiddle::Importer
  dlload "mmc.dll"
  extern "int mmc_convert(char *, char *, char *)"
  extern "void mmc_version(char*)"
end

if ARGV[0] == nil then
  print <<EOD
Usage: ruby mmc_test.rb hoge.mml
EOD
  exit
end

infile = ARGV[0]
outfile = infile.gsub(/\.mml/, '.mid')

buf0 = "\0" * 256
buf1 = "\0" * 256
DLL.mmc_version(buf1)
print "converting #{infile} to #{outfile} by using #{buf1}\n."
result = DLL.mmc_convert(infile, outfile, buf0)

print buf0