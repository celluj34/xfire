#!/usr/bin/perl -w

# Just a small tool to escape #include statements
# since i'm using the output as velocity templates #include statements
# will crash it ;)

use strict;

foreach my $file (`find |grep \.vm`) {
  $file =~ s/\n//;
  system("mv $file $file.old");
  open(FILE, "$file.old") || die $!;
  open(OUT, ">$file") || die $!;

  while(<FILE>) {
    #s/\#(?!(bottom|top))/\\\#/g;
    s/\#(?=include)/\\\#/g;
    print OUT;
  }
  
  close(OUT);
  close(FILE);
  system("rm $file.old");
  print "Done $file\n";
  #system("mv $file.old $file");
}
