#!/usr/bin/perl

while(<>)
    {
    next if(!(($section, $offset, $length, $module)=m/^ (\.text|\.data|\.idata\$.|\.rsrc)\s+(\S+)\s+(\S+)\s+(\S+)\s*$/));
    @modpath=split(/\\/, $module);
    ($modfile)=$modpath[$#modpath]=~m/^([^(]+)/;
    if(substr($section, 0, 6) eq '.idata')
        {
        $section=substr($section, 0, 6);
        }
    if(!defined($DotSection{$modfile}))
        {
        $DotSection{$modfile}={};
        }

    $DotSection{$modfile}->{$section}+=hex($length);
    $DotSection{$modfile}->{'sum'}+=hex($length);
    $Sum{$section}+=hex($length);
    $Sum{'sum'}+=hex($length);
    }

$~='MAP';
$^='MAP_TOP';
for $i (sort {$DotSection{$b}->{'sum'}<=>$DotSection{$a}->{'sum'}} keys(%DotSection))
    {
    write;
    } 
$~='SUM';
write;

format MAP_TOP=
FILE                      SUM    .text    .data    .idata   .rsrc
---------------------- -------- -------- -------- -------- --------
.
format MAP=
@<<<<<<<<<<<<<<<<<<<<< @>>>>>>> @>>>>>>> @>>>>>>> @>>>>>>> @>>>>>>>
substr($i, -22), $DotSection{$i}->{'sum'}, $DotSection{$i}->{'.text'}, $DotSection{$i}->{'.data'}, $DotSection{$i}->{'.idata'}, $DotSection{$i}->{'.rsrc'}
.
format SUM=
---------------------- -------- -------- -------- -------- --------
                       @>>>>>>> @>>>>>>> @>>>>>>> @>>>>>>> @>>>>>>>
$Sum{'sum'}, $Sum{'.text'}, $Sum{'.data'}, $Sum{'.idata'}, $Sum{'.rsrc'}
.

