#!/usr/bin/perl
$file = $ARGV[0];
$in_table = 0;
$table_data = "";

open(FILE,$file);

while(<FILE>)
{
    $curr_line = $_;
    if($in_table)
    {
	($command,$help) = $curr_line =~ m/goto([^;]*);?(.*)/;
	$command =~ s/(\s|_piclang|piclang_)//g;
        print "returnMe[\"$command\"] = \"$help\";\n";
    }
    if($curr_line =~ m/PICLANG_COMMAND_TABLE_MAC/){
	$in_table = 1;
	
    }
    if($curr_line =~ m/END_OF_PICLANG_COMMAND_TABLE/){$in_table = 0;}
}

open(FILE,$file);

while(<FILE>)
{
    $curr_line = $_;
    if($in_table)
    {
	($command,$help) = $curr_line =~ m/goto([^;]*);?(.*)/;
	$command =~ s/(\s|_piclang|piclang_)//g;
        print "returnMe[\"$command\"] = opcode++;\n";
    }
    if($curr_line =~ m/PICLANG_COMMAND_TABLE_MAC/){
	$in_table = 1;
	
    }
    if($curr_line =~ m/END_OF_PICLANG_COMMAND_TABLE/){$in_table = 0;}
}
